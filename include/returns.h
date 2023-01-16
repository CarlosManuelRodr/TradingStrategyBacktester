#pragma once
#include "evaluator.h"

//**********************
//*     Data types     *
//*********************/

enum class StrategySignal
{
    Buy, Sell
};

/** Contains information about the execution of an order. */
struct StrategyExecutionData
{
    StrategySignal signalType;
    std::string time;
    unsigned timeIndex;
    double price;
};

//*****************************
//*    Returns calculation    *
//****************************/

class Returns {
public:

    /**
     * Calculate a list of simple returns from the results of backtesting.
     * @param executionData Results of backtesting the strategy.
     * @param transactionCost The percentage cost of each transaction.
     * @return The returns.
     */
    static std::vector<double> SimpleReturns(const std::vector<StrategyExecutionData>& executionData, double transactionCost);

    /**
     * Calculate a list of logarithmic returns from the results of backtesting.
     * @param executionData Results of backtesting the strategy.
     * @param transactionCost The percentage cost of each transaction.
     * @return The returns.
     */
    static std::vector<double> LogReturns(const std::vector<StrategyExecutionData>& executionData, double transactionCost);

    /**
     * Calculate a list of percentage returns from the results of backtesting.
     * @param executionData Results of backtesting the strategy.
     * @param transactionCost The percentage cost of each transaction.
     * @return The returns.
     */
    static std::vector<double> DivReturns(const std::vector<StrategyExecutionData>& executionData, double transactionCost);

};