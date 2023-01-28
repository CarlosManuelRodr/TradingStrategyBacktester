#pragma once
#include <string>
#include <vector>
#include "returns.h"

enum class TimingStrategySignal
{
    NeutralHold,
    EnterHold,
    ExitHold,
    Enter,
    Exit
};

struct Transition
{
    TimingStrategySignal parent;
    TimingStrategySignal child;
    bool input;
};

class Backtester
{
private:
    static size_t findEntryPoint(const std::vector<bool>& strategySignals, unsigned cursor);
    static size_t exitPosition(const std::string& stock, unsigned entryTime, double profitTake, double stopLoss,
                               double transactionCost);
    static std::vector<TimingStrategySignal> getTimingStrategySignals(std::vector<bool> strategyValues);

public:

    /**
     * Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit
     * strategy is set in terms of profit taking and stop loss parameters.
     * @param strategySignals List of signals returned from the evaluation of the strategy function.
     * @param stock The name of the stock to backtest.
     * @param profitTake Percentage profit to exit the position.
     * @param stopLoss Percentage loss to exit the position.
     * @param transactionCost Percentage cost of a transaction.
     * @param minibatchSize Size of the minibatch sample. By default is set to -1 which means that all the data will be
     *                      used.
     * @return A list containing all the ExecutionData.
     */
    static std::vector<ExecutionData>
    BacktestStoplossProfittake(const std::vector<bool>& strategySignals, const std::string& stock, double profitTake,
                               double stopLoss, double transactionCost, int minibatchSize = -1);

    /**
     * Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit
     * strategy is set in terms of a time period to hold onto the asset.
     * @param strategySignals List of signals returned from the evaluation of the strategy function.
     * @param stock The name of the stock to backtest.
     * @param timePeriod Time period to hold an asset.
     * @param transactionCost Percentage cost of a transaction.
     * @param minibatchSize Size of the minibatch sample. By default is set to -1 which means that all the data will be
     *                      used.
     * @return A list containing all the ExecutionData.
     */
    static std::vector<ExecutionData>
    BacktestTimestopHit(const std::vector<bool>& strategySignals, const std::string& stock, int timePeriod,
                        int minibatchSize = -1);

    /**
     * Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit
     * strategy is given by the strategy signals.
     * @param strategySignals List of signals returned from the evaluation of the strategy function.
     * @param stock The name of the stock to backtest.
     * @param transactionCost Percentage cost of a transaction.
     * @param minibatchSize Percentage cost of a transaction.
     * @return A list containing all the ExecutionData.
     */
    static std::vector<ExecutionData>
    BacktestMarketTiming(const std::vector<bool>& strategySignals, const std::string& stock, int minibatchSize = -1);

};