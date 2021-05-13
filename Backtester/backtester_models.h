#pragma once
#include <string>
#include <vector>
#include "strategy_evaluator.h"

/*********************
*                    *
*     Data types     *
*                    *
*********************/

enum class StrategySignal
{
    Buy, Sell
};

/// <summary>
/// Contains information about the execution of an order.
/// </summary>
struct StrategyExecutionData
{
    StrategySignal signalType;
    std::string time;
    unsigned timeIndex;
    double price;
};

/****************************
*                           *
*    Returns calculation    *
*                           *
****************************/

/// <summary>
/// Calculate a list of simple returns from the results of backtesting.
/// </summary>
/// <param name="executionData">Results of backtesting the strategy.</param>
std::vector<double> calculate_simple_returns(std::vector<StrategyExecutionData> executionData, double transactionCost);

/// <summary>
/// Calculate a list of logarithmic returns from the results of backtesting.
/// </summary>
/// <param name="executionData">Results of backtesting the strategy.</param>
std::vector<double> calculate_log_returns(std::vector<StrategyExecutionData> executionData, double transactionCost);

/// <summary>
/// Calculate a list of percentage returns from the results of backtesting.
/// </summary>
/// <param name="executionData">Results of backtesting the strategy.</param>
std::vector<double> calculate_div_returns(std::vector<StrategyExecutionData> executionData, double transactionCost);

/*********************
*                    *
*     Backtester     *
*                    *
*********************/

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is set in terms of profit taking and stop loss parameters.
/// </summary>
/// <param name="strategySignals">List of signals returned from the evaluation of the strategy function.</param>
/// <param name="stock">The name of the stock to backtest.</param>
/// <param name="profitTake">Percentage profit to exit the position.</param>
/// <param name="stopLoss">Percentage loss to exit the position.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<StrategyExecutionData> backtest_stoploss_profittake(const std::vector<bool>& strategySignals, std::string stock,
                                                                double profitTake, double stopLoss, double transactionCost,
                                                                int minibatchSize = -1);

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is set in terms of profit taking and stop loss parameters.
/// </summary>
/// <param name="strategySignals">List of lists of signals returned from the evaluations of the strategy function in multiple stocks.</param>
/// <param name="profitTake">Percentage profit to exit the position.</param>
/// <param name="stopLoss">Percentage loss to exit the position.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<std::vector<StrategyExecutionData>> backtest_stoploss_profittake_allstocks(const std::vector<std::vector<bool>>& strategySignals,
                                                                                       double profitTake, double stopLoss, double transactionCost,
                                                                                       int minibatchSize = -1);

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is set in terms of a time period to hold onto the asset.
/// </summary>
/// <param name="strategySignals">List of signals returned from the evaluation of the strategy function.</param>
/// <param name="stock">The name of the stock to backtest.</param>
/// <param name="timePeriod">Time period to hold an asset.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// /// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<StrategyExecutionData> backtest_timestop_hit(const std::vector<bool>& strategySignals, std::string stock,
                                                         int timePeriod, double transactionCost, int minibatchSize = -1);

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is set in terms of a time period to hold onto the asset.
/// </summary>
/// <param name="strategySignals">List of lists of signals returned from the evaluations of the strategy function in multiple stocks.</param>
/// <param name="timePeriod">Time period to hold an asset.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<std::vector<StrategyExecutionData>> backtest_timestop_hit_allstocks(const std::vector<std::vector<bool>>& strategySignals,
                                                                                int timePeriod, double transactionCost,
                                                                                int minibatchSize = -1);

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is given by the strategy signals.
/// </summary>
/// <param name="strategySignals">List of signals returned from the evaluation of the strategy function.</param>
/// <param name="stock">The name of the stock to backtest.</param>
/// <param name="timePeriod">Time period to hold an asset.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// /// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<StrategyExecutionData> backtest_markettiming(const std::vector<bool>& strategySignals, std::string stock,
                                                         double transactionCost, int minibatchSize = -1);

/// <summary>
/// Simulates the operation of the strategy in the stock market given the historical data in the dataset. The exit strategy
/// is given by the strategy signals.
/// </summary>
/// <param name="strategySignals">List of lists of signals returned from the evaluations of the strategy function in multiple stocks.</param>
/// <param name="transactionCost">Percentage cost of a transaction.</param>
/// <param name="minibatchSize">Size of the minibatch sample. By default is set to -1 which means that all the data will be used.</param>
/// <returns>A list containing all the StrategyExecutionData.</returns>
std::vector<std::vector<StrategyExecutionData>> backtest_markettiming_allstocks(const std::vector<std::vector<bool>>& strategySignals,
                                                                                double transactionCost, int minibatchSize = -1);