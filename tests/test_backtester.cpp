#include <doctest.h>
#include "../include/loader.h"
#include "../include/backtester.h"
using namespace std;
using namespace backtester;

TEST_CASE("Test strategy results")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    const string strategyProgram = R"(Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time))";
    vector<bool> strategyResults = Evaluator::RunStrategy(strategyProgram, "AAPL");

    CHECK((strategyResults.at(13) == false));
    CHECK((strategyResults.at(14) == true));
}

TEST_CASE("Test backtester")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    const string strategyProgram = R"(Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time))";
    CHECK((Evaluator::ValidateStrategyProgram(strategyProgram).first == true));

    vector<bool> strategyResults = Evaluator::RunStrategy(strategyProgram, "AAPL");
    vector<ExecutionData> executionData = Backtester::BacktestStoplossProfittake(strategyResults, "AAPL",
                                                                                 0.05, 0.05, 0.05);

    vector<double> returns = Returns::SimpleReturns(executionData, 0.05);
    double total = VectorOps::Total(returns);

    CHECK((abs(total) > 0));

}

TEST_CASE("Test backtester ind quantile")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    const string strategyProgram = R"(Indicator("EMA", stock, time) < IndQuantile("ClosePrice", "0.75",  stock, time))";
    CHECK((Evaluator::ValidateStrategyProgram(strategyProgram).first == true));

    vector<bool> strategyResults = Evaluator::RunStrategy(strategyProgram, "AAPL");
    vector<ExecutionData> executionData = Backtester::BacktestStoplossProfittake(strategyResults, "AAPL",
                                                                                 0.05, 0.05, 0.05);

    vector<double> returns = Returns::SimpleReturns(executionData, 0.05);
    double total = VectorOps::Total(returns);

    CHECK((abs(total) > 0));
}

TEST_CASE("Test parallel backtester")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    const string strategyProgram = R"(Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time))";
    CHECK((Evaluator::ValidateStrategyProgram(strategyProgram).first == true));

    map<string, vector<bool>> strategyResults = Evaluator::RunStrategyAllStocks(strategyProgram);

    vector<double> returns;
    for (auto const& [key, val] : strategyResults)
    {
        vector<ExecutionData> executionData = Backtester::BacktestStoplossProfittake(val, key,
                                                                                     0.05, 0.05, 0.05);

        double ret = VectorOps::Total(Returns::SimpleReturns(executionData, 0.05));
        returns.push_back(ret);
        cout << "Stock: " << key << ", Returns: " << ret << endl;
    }

    double totalRet = VectorOps::Total(returns);
    CHECK((abs(totalRet) > 0));
}