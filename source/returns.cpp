#include <cmath>
#include "returns.h"
using namespace std;

enum class CalculateSimpleReturnsState
{
    ExpectingBuy, ExpectingSell
};

double simple_returns(double pSell, double pBuy, double transactionCost)
{
    return pSell * (1 - transactionCost) - pBuy * (1 + transactionCost);
}

double log_returns(double pSell, double pBuy, double transactionCost)
{
    return log(pSell * (1 - transactionCost)) - log(pBuy * (1 + transactionCost));
}

double div_returns(double pSell, double pBuy, double transactionCost)
{
    return (pSell * (1 - transactionCost)) / (pBuy * (1 + transactionCost));
}

vector<double> calculate_returns(double (*returnFunction)(double, double, double),
                                 const vector<ExecutionData>& executionData, double transactionCost)
{
    CalculateSimpleReturnsState state = CalculateSimpleReturnsState::ExpectingBuy;
    vector<double> returns;
    double buyPrice = 0.0;
    double sellPrice;

    for (const ExecutionData& execution : executionData)
    {
        if (state == CalculateSimpleReturnsState::ExpectingBuy)
        {
            if (execution.signalType == StrategySignal::Buy)
            {
                buyPrice = execution.price;
                state = CalculateSimpleReturnsState::ExpectingSell;
            }
            else
                throw runtime_error("Found sell signal while expecting buy signal.");
        }
        else
        {
            if (execution.signalType == StrategySignal::Sell) {
                sellPrice = execution.price;
                returns.push_back(returnFunction(sellPrice, buyPrice, transactionCost));
                state = CalculateSimpleReturnsState::ExpectingBuy;
            } else
                throw runtime_error("Found buy signal while expecting sell signal.");
        }
    }

    return returns;
}

vector<double> Returns::SimpleReturns(const vector<ExecutionData>& executionData, double transactionCost)
{
    return calculate_returns(simple_returns, executionData, transactionCost);
}

vector<double> Returns::LogReturns(const vector<ExecutionData>& executionData, double transactionCost)
{
    return calculate_returns(log_returns, executionData, transactionCost);
}

vector<double> Returns::DivReturns(const vector<ExecutionData>& executionData, double transactionCost)
{
    return calculate_returns(div_returns, executionData, transactionCost);
}