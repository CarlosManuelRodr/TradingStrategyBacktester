/**
 *  @file   mathlink_interface.cpp
 *  @brief  Defines the API to connect the module to Mathematica.
 *  @author CarlosManuelRodr
 *  @date   2021-05-04
 ***************************************************************/

#include <vector>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iterator>
#include <unordered_map>
#include "dataset_loader.h"
#include "backtester_models.h"
#include "filesystem.h"
#include "mathlink.h"

#if defined(WINDOWS_MATHLINK)
#include <windows.h>
#endif
using namespace std;

/****************************
*                           *
*    Global definitions     *
*                           *
****************************/

unordered_map<string, Dataset> dataset;

/****************************
*                           *
*         Utilities         *
*                           *
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

bool is_stock_in_dataset(string stockName)
{
    Dataset* currentDataset = get_strategy_evaluator_dataset();

    if (currentDataset->empty())
        return false;

    return !(currentDataset->find(stockName) == currentDataset->end());
}

auto get_return_function(const char* returnFunctionName)
{
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = nullptr;
    const string returnFunctionNameString = string(returnFunctionName);
    if (returnFunctionNameString == "SimpleReturns")
        returnFunction = calculate_simple_returns;
    else if (returnFunctionNameString == "LogReturns")
        returnFunction = calculate_log_returns;
    else if (returnFunctionNameString == "DivReturns")
        returnFunction = calculate_div_returns;

    return returnFunction;
}

/****************************
*                           *
*    Dataset Management     *
*                           *
****************************/

void load_dataset_from_directory(char const* path, char const* identifier)
{
    string stringPath(path);
    utilities::ReplaceString(stringPath, "\\\\", "\\");

    if (directory_exist(stringPath))
    {
        // Get all the files.
        vector<string> files = files_in_directory(stringPath);

        // Check if they all have a CSV extension.
        bool isDatasetValid = true;
        for (string file : files)
        {
            if (file_extension(file) != "csv")
            {
                isDatasetValid = false;
                break;
            }
        }

        if (isDatasetValid)
        {
            string identifierString(identifier);
            if (identifierString == "")
                identifierString = "Main";

            dataset[identifierString] = load_dataset(stringPath);
            set_strategy_evaluator_dataset(&dataset[identifierString]);

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
        set_strategy_evaluator_dataset(&dataset[identifierString]);

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
    vector<string> stocks = get_stocks_in_dataset();
    return (int)stocks.size();
}

void get_stock_names_of_dataset()
{
    vector<string> stocks = get_stocks_in_dataset();
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
*     Dataset accesors      *
*                           *
****************************/

void get_date(char const* stock, int time)
{
    if (is_stock_in_dataset(stock))
    {
        string date = Date(stock, time);
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
        vector<string> dates = Dates(stock);
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
        double indValue = Indicator(indicatorName, stock, time);
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
        vector<double> indTS = IndicatorTimeSeries(indicatorName, stock);
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
        double indValue = IndQuantile(indicatorName, percentile, stock, time);
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
        vector<double> indTS = IndQuantileTimeSeries(indicatorName, percentile, stock);
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
        vector<bool> signals = run_strategy(strategyFunc, stock);

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
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_stoploss_profittake(signals, stock, profitTake, stopLoss, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (StrategyExecutionData d : sed)
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
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_timestop_hit(signals, stock, timePeriod, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (StrategyExecutionData d : sed)
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
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_markettiming(signals, stock, transactionCost);

        MLPutFunction(stdlink, "List", (int)sed.size());
        for (StrategyExecutionData d : sed)
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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_stoploss_profittake(signals, stock, profitTake, stopLoss, transactionCost, minibatchSize);
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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_timestop_hit(signals, stock, timePeriod, transactionCost, minibatchSize);
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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if (is_stock_in_dataset(stock) && (int)strategyFunctionString.length() > 0)
    {
        vector<bool> signals = run_strategy(strategyFunc, stock);
        vector<StrategyExecutionData> sed = backtest_markettiming(signals, stock, transactionCost, minibatchSize);
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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = get_stocks_in_dataset();
        vector<vector<bool>> allStockSignals = run_strategy_for_all_stocks(strategyFunc);
        vector<vector<StrategyExecutionData>> allBacktests = backtest_stoploss_profittake_allstocks(allStockSignals, profitTake,
                                                                                                    stopLoss, transactionCost, 
                                                                                                    minibatchSize);

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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = get_stocks_in_dataset();
        vector<vector<bool>> allStockSignals = run_strategy_for_all_stocks(strategyFunc);
        vector<vector<StrategyExecutionData>> allBacktests = backtest_timestop_hit_allstocks(allStockSignals, timePeriod,
                                                                                             transactionCost, minibatchSize);

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
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = get_return_function(returnFunctionName);
    if (returnFunction == nullptr)
    {
        MLPutNull(stdlink);
        return;
    }

    string strategyFunctionString = strategyFunc;
    if ((int)strategyFunctionString.length() > 0)
    {
        vector<string> stocks = get_stocks_in_dataset();
        vector<vector<bool>> allStockSignals = run_strategy_for_all_stocks(strategyFunc);
        vector<vector<StrategyExecutionData>> allBacktests = backtest_markettiming_allstocks(allStockSignals, transactionCost, minibatchSize);

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
        string testResult = test_strategy_program(strategyFunctionString);
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