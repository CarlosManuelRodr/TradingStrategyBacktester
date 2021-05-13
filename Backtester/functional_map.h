#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "progress_bar.h"

/// <summary>
/// A C++11 implementation of a thread pool taken from: https://github.com/progschj/ThreadPool
/// TODO: std::result_of is deprecated in C++17. Move to a C++17 implementation. The current workaround
/// is to define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS in the preproccesor.
/// </summary>
class ThreadPool
{
public:
    ThreadPool(size_t);
    template<class F, class... Args> auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    // Need to keep track of threads so we can join them.
    std::vector<std::thread> workers;

    // The task queue.
    std::queue<std::function<void()>> tasks;

    // Synchronization.
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// The constructor just launches some amount of workers.
inline ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
            );
}

// Add new work item to the pool.
template<class F, class... Args> auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // Don't allow enqueueing after stopping the pool.
        if (stop)
            throw std::runtime_error("Enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// The destructor joins all threads.
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

/// <summary>
/// Pararellized map function.
/// </summary>
/// <typeparam name="R">Return type.</typeparam>
/// <typeparam name="T">Argument type.</typeparam>
/// <param name="f">Function pointer to map.</param>
/// <param name="arg">List of arguments where the map is applied.</param>
/// <param name="showProgress">Show the CLI progress bar?</param>
/// <returns>A list containing the results of the mapping.</returns>
template<typename R, typename T> std::vector<R> parallel_function_map(std::function<R(T)> f, std::vector<T> arg, bool showProgress = true)
{
    unsigned int hc = std::thread::hardware_concurrency();

    ThreadPool pool(hc);
    std::vector<std::future<R>> result;
    std::vector<R> result_values;

    for (unsigned i = 0; i < arg.size(); i++)
        result.push_back(pool.enqueue(f, arg[i]));

    for (auto& r : result)
    {
        if (showProgress)
            progress_bar((double)(&r - &result[0]), 0.0, (double) result.size(), 1.0);

        result_values.push_back(r.get());
    }

    if (showProgress)
        progress_bar(1.0);

    return result_values;
}

/// <summary>
/// Simple map function.
/// </summary>
/// <typeparam name="R">Return type.</typeparam>
/// <typeparam name="T">Argument type.</typeparam>
/// <param name="f">Function pointer to map.</param>
/// <param name="arg">List of arguments where the map is applied.</param>
/// <param name="showProgress">Show the CLI progress bar?</param>
/// <returns>A list containing the results of the mapping.</returns>
template<typename R, typename T> std::vector<R> function_map(std::function<R(T)> f, std::vector<T> arg, bool showProgress = true)
{
    std::vector<R> result_values;

    for (unsigned i = 0; i < arg.size(); i++)
    {
        if (showProgress)
            progress_bar((double) i, 0.0, (double) arg.size(), 1.0);

        result_values.push_back(f(arg[i]));
    }

    if (showProgress)
        progress_bar(1.0);

    return result_values;
}