#include <doctest.h>
#include "../include/loader.h"
#include "../include/filesystem.h"
#include "../include/backtester.h"
#include "../include/evaluator.h"
using namespace std;

TEST_CASE("Test strategy results")
{
    Dataset dataset = Loader::LoadDataset("../dataset");
    set_strategy_evaluator_dataset(&dataset);

    string strategyProgram = R"(Indicator("EMA",stock,time)<Indicator("ClosePrice",stock,time))";
    vector<bool> strategyResults = run_strategy(strategyProgram, "AAPL");

    CHECK((strategyResults.at(13) == false));
    CHECK((strategyResults.at(14) == true));
}