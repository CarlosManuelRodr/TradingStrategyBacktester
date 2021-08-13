#pragma once
#include <vector>
#include <algorithm>
#include <numeric>

namespace vectorops
{
    template<typename T> std::vector<T> Take(const std::vector<T>& v, const int n)
    {
        return std::vector<T>(v.begin(), v.begin() + n);
    }

    template<typename T> std::vector<T> Take(const std::vector<T>& v, int first, int last)
    {
        if (first < 0)
            first = v.size() - first;
        if (last < 0)
            last = v.size() - last;

        return std::vector<T>(v.begin() + first, v.begin() + last);
    }

    template<typename T> std::vector<T> Drop(const std::vector<T>& v, int n)
    {
        if (n > 0)
            return std::vector<T>(v.begin() + n, v.end());
        else
            return std::vector<T>(v.begin(), v.end() + n);
    }

    /// <summary>
    /// Check if the vector v contains the key.
    /// </summary>
    template<typename T> bool VectorContainsQ(const std::vector<T>& v, const T& key)
    {
        return std::find(v.begin(), v.end(), key) != v.end();
    }

    /// <summary>
    /// Return the sum of all the elements of the vector.
    /// </summary>
    /// <typeparam name="T">Numeric type.</typeparam>
    template<typename T> T Total(const std::vector<T>& v)
    {
        const T init = static_cast<T>(0);
        return std::accumulate(v.begin(), v.end(), init);
    }

    /// <summary>
    /// Return the multiplication of all the elements of the vector.
    /// </summary>
    /// <typeparam name="T">Numeric type.</typeparam>
    template<typename T> T Multiply(const std::vector<T>& v)
    {
        const T init = static_cast<T>(1);
        return std::accumulate(v.begin(), v.end(), init, [](T n1, T n2) { return n1 * n2; });
    }

    /// <summary>
    /// Elementwise vector addition
    /// </summary>
    /// <typeparam name="T">Numeric type</typeparam>
    /// <param name="a">First vector</param>
    /// <param name="b">Second vector</param>
    /// <returns>A new vector that is the sum a + b.</returns>
    template<typename T> std::vector<T> Add(const std::vector<T>& a, const std::vector<T>& b)
    {
        std::vector<T> result;
        std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), [](T a, T b) { return a + b; });
        return result;
    }

    /// <summary>
    /// Elementwise vector substraction
    /// </summary>
    /// <typeparam name="T">Numeric type</typeparam>
    /// <param name="a">First vector</param>
    /// <param name="b">Second vector</param>
    /// <returns>A new vector that is the substraction a - b.</returns>
    template<typename T> std::vector<T> Substract(const std::vector<T>& a, const std::vector<T>& b)
    {
        std::vector<T> result;
        std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), [](T a, T b) { return a - b; });
        return result;
    }

    template<typename T> std::vector<T> Substract(std::vector<T> a, const T b)
    {
        for (T& el : a)
            el -= b;
        return a;
    }


    /// <summary>
    /// Applies the UnitSign function to all the elements of the vector.
    /// </summary>
    template<typename T> std::vector<int> Sign(const std::vector<T>& v)
    {
        std::vector<int> result;
        result.resize(v.size());
        std::transform(v.begin(), v.end(), result.begin(), [](T val) {return (T(0) < val) - (val < T(0)); });

        // Fix 0 sign that appears in badly formatted data sets.
        for (unsigned i = 1; i < result.size(); i++)
        {
            if (result[i] == 0)
                result[i] = result[i - 1];
        }

        return result;
    }

    template<typename T> std::vector<int> CountSequences(const std::vector<T>& v)
    {
        std::vector<int> sequenceLengths;

        T sequenceElement = v.front();
        int sequenceLengthCounter = 0;
        for (T var : v)
        {
            if (var == sequenceElement)
                sequenceLengthCounter++;
            else
            {
                sequenceLengths.push_back(sequenceLengthCounter);
                sequenceLengthCounter = 1;
                sequenceElement = var;
            }
        }
        return sequenceLengths;
    }
}