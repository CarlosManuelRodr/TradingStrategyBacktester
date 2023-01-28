#include <doctest.h>
#include "../include/loader.h"
#include "../include/filesystem.h"
#include "../include/backtester.h"
using namespace std;

TEST_CASE("Test strategy results")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    string strategyProgram = R"(Indicator("EMA",stock,time)<Indicator("ClosePrice",stock,time))";
    vector<bool> strategyResults = Evaluator::RunStrategy(strategyProgram, "AAPL");

    CHECK((strategyResults.at(13) == false));
    CHECK((strategyResults.at(14) == true));
}

TEST_CASE("Test backtester")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    Evaluator::SetStrategyEvaluatorDataset(dataset);

    string strategyProgram = R"(Indicator("EMA",stock,time)<Indicator("ClosePrice",stock,time))";
    vector<bool> strategyResults = Evaluator::RunStrategy(strategyProgram, "AAPL");

    std::vector<ExecutionData> executionData = Backtester::BacktestStoplossProfittake(strategyResults, "AAPL",
                                                                                      0.05, 0.05, 0.05);

    std::vector<double> returns = Returns::SimpleReturns(executionData, 0.05);
    double total = VectorOps::Total(returns);

    CHECK((abs(total) > 0));
}