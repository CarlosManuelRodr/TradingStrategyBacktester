/**
 *  @file   mathlink_interface.cpp
 *  @brief  Defines the API to connect the module to Mathematica.
 *  @author CarlosManuelRodr
 *  @date   2021-05-04
 ***************************************************************/

#include <vector>
#include <string>
#include <unordered_map>
#include "loader.h"
#include "backtester.h"
#include "filesystem.h"
#include "mathlink.h"

#if defined(WINDOWS_MATHLINK)
#include <windows.h>
#endif
using namespace std;
using namespace backtester;

/****************************
*    Global definitions     *
****************************/

unordered_map<string, Dataset> dataset;

/****************************
*         Utilities         *
****************************/

void MLPutStringList(MLINK mlp, vector<string> stringList)
{
    MLPutFunction(mlp, "List", (int)stringList.size());
    for (string& s : stringList)
    {
        MLPutString(mlp, s.c_str());
    }
}

void MLPutNull(MLINK mlp)
{
    MLPutSymbol(stdlink, "Null");
    MLEndPacket(stdlink);
}

bool is_stock_in_dataset(const string& stockName)
{
    Dataset currentDataset = Evaluator::GetStrategyEvaluatorDataset();

    if (currentDataset.empty())
        return false;

    return !(currentDataset.find(stockName) == currentDataset.end());
}

auto get_return_function(const char* returnFunctionName)
{
    vector<double>(*returnFunction)(const vector<ExecutionData>&, double) = nullptr;
    const string returnFunctionNameString = string(returnFunctionName);
    if (returnFunctionNameString == "SimpleReturns")
        returnFunction = Returns::SimpleReturns;
    else if (returnFunctionNameString == "LogReturns")
        returnFunction = Returns::LogReturns;
    else if (returnFunctionNameString == "DivReturns")
        returnFunction = Returns::DivReturns;

    return returnFunction;
}

/****************************
*    Dataset Management     *
****************************/

void load_dataset_from_directory(char const* path, char const* identifier)
{
    string stringPath(path);
    Utilities::ReplaceString(stringPath, "\\\\", "\\");

    if (FileSystem::DirectoryExist(stringPath))
    {
        // Get all the files.
        vector<string> files = FileSystem::FilesInDirectory(stringPath);

        // Check if they all have a CSV extension.
        bool isDatasetValid = true;
        for (const string& file : files)
        {
            if (FileSystem::FileExtension(file) != "csv")
            {
                isDatasetValid = false;
                break;
            }
        }

        if (isDatasetValid)
        {
            string identifierString(identifier);
            if (identifierString.empty())
                identifierString = "Main";

            dataset[identifierString] = Loader::LoadDataset(stringPath);
            Evaluator::SetStrategyEvaluatorDataset(dataset[identifierString]);

            MLPutSymbol(stdlink, "True");
            MLEndPacket(stdlink);
        }
        else
        {
            MLPutSymbol(stdlink, "False");
            MLEndPacket(stdlink);
        }
    }
    else
    {
        MLPutSymbol(stdlink, "False");
        MLEndPacket(stdlink);
    }
}

void switch_to_dataset(char const* identifier)
{
    string identifierString(identifier);
    if (dataset.find(identifierString) != dataset.end())
    {
        Evaluator::SetStrategyEvaluatorDataset(dataset[identifierString]);

        MLPutSymbol(stdlink, "True");
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "False");
        MLEndPacket(stdlink);
    }
}

void unload_dataset(char const* identifier)
{
    string identifierString(identifier);

    if (dataset.find(identifierString) != dataset.end())
    {
        dataset.erase(identifierString);
        MLPutSymbol(stdlink, "True");
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "False");
        MLEndPacket(stdlink);
    }
}

int get_number_of_loaded_stocks()
{
    vector<string> stocks = Evaluator::GetStocksInDataset();
    return (int)stocks.size();
}

void get_stock_names_of_dataset()
{
    vector<string> stocks = Evaluator::GetStocksInDataset();
    if (!stocks.empty())
    {
        MLPutStringList(stdlink, stocks);
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

/****************************
*                           *
*     Dataset accessors      *
*                           *
****************************/

void get_date(char const* stock, int time)
{
    if (is_stock_in_dataset(stock))
    {
        string date = Evaluator::Date(stock, time);
        MLPutString(stdlink, date.c_str());
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_dates_list(char const* stock)
{
    if (is_stock_in_dataset(stock))
    {
        vector<string> dates = Evaluator::Dates(stock);
        MLPutStringList(stdlink, dates);
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_indicator(char const* indicatorName, char const* stock, int time)
{
    if (is_stock_in_dataset(stock))
    {
        double indValue = Evaluator::Indicator(indicatorName, stock, time);
        MLPutReal(stdlink, indValue);
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_indicator_timeseries(char const* indicatorName, char const* stock)
{
    if (is_stock_in_dataset(stock))
    {
        vector<double> indTS = Evaluator::IndicatorTimeSeries(indicatorName, stock);
        MLPutRealList(stdlink, indTS.data(), (int)indTS.size());
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_quantile_indicator(char const* indicatorName, char const* percentile, char const* stock, int time)
{
    if (is_stock_in_dataset(stock))
    {
        double indValue = Evaluator::IndQuantile(indicatorName, percentile, stock, time);
        MLPutReal(stdlink, indValue);
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_quantile_indicator_timeseries(char const* indicatorName, char const* percentile, char const* stock)
{
    if (is_stock_in_dataset(stock))
    {
        vector<double> indTS = Evaluator::IndQuantileTimeSeries(indicatorName, percentile, stock);
        MLPutRealList(stdlink, indTS.data(), (int)indTS.size());
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

/****************************
*                           *
*      Backtesting API      *
*                           *
****************************/

void get_strategy_values(char const* strategyFunc, char const* stock)
{
    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);

        MLPutFunction(stdlink, "List", (int)signals.size());
        for (bool s : signals)
        {
            const string value = s ? "True" : "False";
            MLPutSymbol(stdlink, value.c_str());
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

/*************************************
*                                    *
*    get_strategy_execution_data     *
*                                    *
*************************************/

/// Returns the date and price where trading signals are executed.
void get_strategy_execution_data_stoploss_profittake(char const* strategyFunc, char const* stock, double profitTake, double stopLoss,
                                                     double transactionCost)
{
    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestStoplossProfittake(signals, stock, profitTake, stopLoss, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (ExecutionData d : sed)
        {
            MLPutFunction(stdlink, "Association", 4);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Signal");
            MLPutString(stdlink, (d.signalType == StrategySignal::Buy) ? "Buy" : "Sell");

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Time");
            MLPutString(stdlink, d.time.c_str());

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "TimeIndex");
            MLPutInteger(stdlink, d.timeIndex);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Price");
            MLPutReal(stdlink, d.price);
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_strategy_execution_data_timestop_hit(char const* strategyFunc, char const* stock, int timePeriod, double transactionCost)
{
    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestTimestopHit(signals, stock, timePeriod, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (ExecutionData d : sed)
        {
            MLPutFunction(stdlink, "Association", 4);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Signal");
            MLPutString(stdlink, (d.signalType == StrategySignal::Buy) ? "Buy" : "Sell");

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Time");
            MLPutString(stdlink, d.time.c_str());

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "TimeIndex");
            MLPutInteger(stdlink, d.timeIndex);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Price");
            MLPutReal(stdlink, d.price);
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_strategy_execution_data_markettiming(char const* strategyFunc, char const* stock, double transactionCost)
{
    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestMarketTiming(signals, stock, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (ExecutionData d : sed)
        {
            MLPutFunction(stdlink, "Association", 4);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Signal");
            MLPutString(stdlink, (d.signalType == StrategySignal::Buy) ? "Buy" : "Sell");

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Time");
            MLPutString(stdlink, d.time.c_str());

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "TimeIndex");
            MLPutInteger(stdlink, d.timeIndex);

            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, "Price");
            MLPutReal(stdlink, d.price);
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

/*********************
*                    *
*    get_returns     *
*                    *
*********************/

void get_returns_stoploss_profittake(char const* returnFunctionName, char const* strategyFunc, char const* stock,
                                     double profitTake, double stopLoss, double transactionCost, int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestStoplossProfittake(signals, stock, profitTake, stopLoss, transactionCost, minibatchSize);
        vector<double> returns = returnFunction(sed, transactionCost);
        MLPutRealList(stdlink, returns.data(), (int)returns.size());
        MLEndPacket(stdlink);
    }
    else
        MLPutNull(stdlink);
}

void get_returns_timestop_hit(char const* returnFunctionName, char const* strategyFunc, char const* stock,
                              int timePeriod, double transactionCost, int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestTimestopHit(signals, stock, timePeriod, minibatchSize);
        vector<double> returns = returnFunction(sed, transactionCost);
        MLPutRealList(stdlink, returns.data(), (int)returns.size());
        MLEndPacket(stdlink);
    }
    else
        MLPutNull(stdlink);
}

void get_returns_markettiming(char const* returnFunctionName, char const* strategyFunc, char const* stock, double transactionCost,
                              int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = Evaluator::RunStrategy(strategyFunc, stock);
        vector<ExecutionData> sed = Backtester::BacktestMarketTiming(signals, stock, minibatchSize);
        vector<double> returns = returnFunction(sed, transactionCost);
        MLPutRealList(stdlink, returns.data(), (int)returns.size());
        MLEndPacket(stdlink);
    }
    else
        MLPutNull(stdlink);
}

/***********************************
*                                  *
*    get_returns_for_all_stocks    *
*                                  *
***********************************/

void get_returns_for_all_stocks_stoploss_profittake(char const* returnFunctionName, char const* strategyFunc, double profitTake,
                                                    double stopLoss, double transactionCost, int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = Evaluator::GetStocksInDataset();
        auto allStockSignals = Evaluator::RunStrategyAllStocks(strategyFunc);

        vector<vector<ExecutionData>> allBacktests;
        for (auto const& [key, val] : allStockSignals)
        {
            allBacktests.push_back(Backtester::BacktestStoplossProfittake(val, key, profitTake,
                                                                          stopLoss, transactionCost, minibatchSize));
        }


        MLPutFunction(stdlink, "Association", (int)stocks.size());
        for (unsigned i = 0; i < stocks.size(); i++)
        {
            vector<double> returns = returnFunction(allBacktests[i], transactionCost);
            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, stocks[i].c_str());
            MLPutRealList(stdlink, returns.data(), (int)returns.size());
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_returns_for_all_stocks_timestop_hit(char const* returnFunctionName, char const* strategyFunc, int timePeriod,
                                             double transactionCost, int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = Evaluator::GetStocksInDataset();
        auto allStockSignals = Evaluator::RunStrategyAllStocks(strategyFunc);

        vector<vector<ExecutionData>> allBacktests;
        for (auto const& [key, val] : allStockSignals)
        {
            allBacktests.push_back(Backtester::BacktestTimestopHit(val, key, timePeriod,
                                                                   minibatchSize));
        }

        MLPutFunction(stdlink, "Association", (int)stocks.size());
        for (unsigned i = 0; i < stocks.size(); i++)
        {
            vector<double> returns = returnFunction(allBacktests[i], transactionCost);
            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, stocks[i].c_str());
            MLPutRealList(stdlink, returns.data(), (int)returns.size());
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

void get_returns_for_all_stocks_markettiming(char const* returnFunctionName, char const* strategyFunc, double transactionCost, int minibatchSize)
{
    auto returnFunction = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = Evaluator::GetStocksInDataset();
        auto allStockSignals = Evaluator::RunStrategyAllStocks(strategyFunc);

        vector<vector<ExecutionData>> allBacktests;
        for (auto const &[key, val]: allStockSignals)
            allBacktests.push_back(Backtester::BacktestMarketTiming(val, key, minibatchSize));


        MLPutFunction(stdlink, "Association", (int)stocks.size());
        for (unsigned i = 0; i < stocks.size(); i++)
        {
            vector<double> returns = returnFunction(allBacktests[i], transactionCost);
            MLPutFunction(stdlink, "Rule", 2);
            MLPutString(stdlink, stocks[i].c_str());
            MLPutRealList(stdlink, returns.data(), (int)returns.size());
        }
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutSymbol(stdlink, "Null");
        MLEndPacket(stdlink);
    }
}

/****************************
*                           *
*          Testing          *
*                           *
****************************/

/// <summary>
/// Try to compile the strategy program and report compilation errors if any.
/// </summary>
/// <param name="strategyFunc">Program as string.</param>
void strategy_compilation_info(char const* strategyFunc)
{
    string strategyFunctionString = strategyFunc;
    if (strategyFunctionString.length() > 0)
    {
        string testResult = Evaluator::ValidateStrategyProgram(strategyFunctionString).second;
        MLPutString(stdlink, testResult.c_str());
        MLEndPacket(stdlink);
    }
    else
    {
        MLPutString(stdlink, "Failed: There is no strategy function.");
        MLEndPacket(stdlink);
    }
}

/****************************
*                           *
*   Mathlink entry point    *
*                           *
****************************/

#if defined(WINDOWS_MATHLINK)

extern HWND MLInitializeIcon(HINSTANCE hinstCurrent, int nCmdShow);

int WINAPI WinMain(HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
    char buff[512];
    char* buff_start = buff;
    char* argv[32];
    char** argv_end = argv + 32;

    hinstPrevious = hinstPrevious; /* suppress warning */

    if (!MLInitializeIcon(hinstCurrent, nCmdShow)) return 1;
    MLScanString(argv, &argv_end, &lpszCmdLine, &buff_start);
    return MLMain((int)(argv_end - argv), argv);
}

#else

int main(int argc, char* argv[])
{
    return MLMain(argc, argv);
}

#endif