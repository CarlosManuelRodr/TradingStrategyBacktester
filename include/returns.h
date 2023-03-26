#pragma once
#include "evaluator.h"

namespace backtester
{
    //**********************
    //*     Data types     *
    //*********************/

    enum class StrategySignal
    {
        Buy, Sell
    };

    /** Contains information about the execution of an order. */
    struct ExecutionData
    {
        /** The type of signal. */
        StrategySignal signalType;

        /** The time when the signal is executed. */
        std::string time;

        /** The index of the time when the signal is executed. */
        unsigned timeIndex;

        /** The price at which the transaction is executed. */
        double price;

        /**
         * Get a string representation of the strategySignal.
         */
        [[nodiscard]]
        static std::string StrategySignalToString(StrategySignal strategySignal)
        {
            return strategySignal == StrategySignal::Buy ? "Buy" : "Sell";
        }

        /**
         * Get string representation.
         */
        [[nodiscard]]
        std::string ToString() const
        {
            return "ExecutionData(signalType=" + StrategySignalToString(signalType) + ", time=" + time +
                   ", timeIndex=" + std::to_string(timeIndex) + ", price=" + std::to_string(price) + ")";
        }

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
        static std::vector<double> SimpleReturns(const std::vector<ExecutionData>& executionData, double transactionCost);

        /**
         * Calculate a list of logarithmic returns from the results of backtesting.
         * @param executionData Results of backtesting the strategy.
         * @param transactionCost The percentage cost of each transaction.
         * @return The returns.
         */
        static std::vector<double> LogReturns(const std::vector<ExecutionData>& executionData, double transactionCost);

        /**
         * Calculate a list of percentage returns from the results of backtesting.
         * @param executionData Results of backtesting the strategy.
         * @param transactionCost The percentage cost of each transaction.
         * @return The returns.
         */
        static std::vector<double> DivReturns(const std::vector<ExecutionData>& executionData, double transactionCost);

    };
}