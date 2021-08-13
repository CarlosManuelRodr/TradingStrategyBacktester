#include <iostream>
#include <vector>
#include <algorithm>
#include "dataset_loader.h"
#include "backtester_models.h"
#include "optionparser.h"
using namespace std;

/****************************
*                           *
*       Option Parser       *
*                           *
****************************/

struct Arg : public option::Arg
{
    static void PrintError(const char* msg1, const option::Option& opt, const char* msg2)
    {
#if defined(_WIN32)
        fprintf(stderr, "%s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
#endif
    }

    static option::ArgStatus Required(const option::Option& option, bool msg)
    {
        if (option.arg != 0)
            return option::ARG_OK;

        if (msg) PrintError("Option '", option, "' requires an argument.\n");
        return option::ARG_ILLEGAL;
    }
};

enum  optionIndex
{
    UNKNOWN, DATASET_PATH, STRATEGY, EXECUTIONMODEL, TRANSACTIONCOST, RETURNTYPE, PROFITTAKE, STOPLOSS, TIMEPERIOD, HELP
};

const option::Descriptor usage[] =
{
    {UNKNOWN, 0, "", "",Arg::None, "INSTRUCTIONS: Backtester [options]\n"},
    {DATASET_PATH, 0,"p", "path", Arg::Required, "  -p <dataset_path>, \t--path=<dataset_path>  \tPath to dataset." },
    {STRATEGY,  0, "s", "strategy", Arg::Required, "  -s <strategy>, \t--strategies=<strategy>  \tTrading strategy as a C++ string." },
    {EXECUTIONMODEL,  0, "e", "execution", Arg::Required, "  -e <execution_model>, \t--execution=<execution_model>  \tThe trading execution model. Possible values are: StopLossProfitTake, TimestopHit and MarketTiming." },
    {TRANSACTIONCOST,  0, "t", "transactioncost", Arg::None, "  -t <value>, \t--transactioncost=<value>  \tThe transaction cost as percentage. The default value is 0.0025." },
    {RETURNTYPE,  0, "r", "returntype", Arg::None, "  -r <return_type>, \t--returntype=<return_type>  \tSpecifies how to calculate the returns. Possible values are: SimpleReturns, LogReturns, and DivReturns." },
    {PROFITTAKE, 0,"k", "profittake", Arg::None, "  -k <value>, \t--profittake=<value>  \tProfit Take value used if the execution model is set to StopLossProfitTake. The default value es 0.3." },
    {STOPLOSS, 0,"l", "stoploss", Arg::None, "  -l <value>, \t--stoploss=<value>  \tStop Loss value used if the execution model is set to StopLossProfitTake. The default value es -0.3." },
    {TIMEPERIOD, 0,"d", "timeperiod", Arg::None, "  -d <value>, \t--timeperiod=<value>  \tTime Period value used if the execution model is set to TimestopHit. The default value es 100." },
    {HELP, 0,"", "help", Arg::None,    "  \t--help  \tShow instructions." },
    {0,0,0,0,0,0}
};

int main(int argc, char* argv[])
{
    // Global parameters
    string datasetPath = "";
    string strategy = "";
    string executionModel = "";
    string returnType = "";
    double transactionCost = 0.0025;
    double profitTake = 0.3;
    double stopLoss = -0.3;
    int timePeriod = 100;

    // Argument parser
    argc -= (argc > 0); argv += (argc > 0);
    const option::Stats  stats(static_cast<const option::Descriptor*>(usage), argc, argv);
    vector<option::Option> options(stats.options_max);
    vector<option::Option> buffer(stats.buffer_max);
    option::Parser parse(static_cast<const option::Descriptor*>(usage), argc, argv, &options.at(0), &buffer.at(0));

    if (parse.error())
        return 1;

    if (options.at(HELP) || argc == 0)
    {
        option::printUsage(cout, static_cast<const option::Descriptor*>(usage));
        return 0;
    }

    for (option::Option* opt = options.at(UNKNOWN); opt; opt = opt->next())
        cout << "Unknown option: " << string(opt->name, opt->namelen) << "\n";

    for (int i = 0; i < parse.optionsCount(); ++i)
    {
        const option::Option& opt = buffer.at(i);
        switch (opt.index())
        {
        case DATASET_PATH:
            datasetPath = opt.arg;
            break;
        case STRATEGY:
            strategy = opt.arg;
            break;
        case EXECUTIONMODEL:
            executionModel = opt.arg;
            break;
        case TRANSACTIONCOST:
            transactionCost = stod(opt.arg);
            break;
        case RETURNTYPE:
            returnType = opt.arg;
            break;
        case PROFITTAKE:
            profitTake = stod(opt.arg);
            break;
        case STOPLOSS:
            stopLoss = stod(opt.arg);
            break;
        case TIMEPERIOD:
            timePeriod = stoi(opt.arg);
            break;
        default:
            break;
        }
    }

    // Load dataset.
    utilities::ReplaceString(datasetPath, "\\\\", "\\");
    Dataset dataset = load_dataset(datasetPath);
    set_strategy_evaluator_dataset(&dataset);
    vector<string> stockList = utilities::Keys(dataset);
    replace(strategy.begin(), strategy.end(), '\\', '\"');

    // Run the strategy function.
    vector<vector<bool>> allStockSignals = run_strategy_for_all_stocks(strategy);

    // Backtest.
    vector<vector<StrategyExecutionData>> allBacktests;
    if (executionModel == "StopLossProfitTake")
        allBacktests = backtest_stoploss_profittake_allstocks(allStockSignals, profitTake, stopLoss, transactionCost);
    else if (executionModel == "TimestopHit")
        allBacktests = backtest_timestop_hit_allstocks(allStockSignals, timePeriod, transactionCost);
    else if (executionModel == "MarketTiming")
        allBacktests = backtest_markettiming_allstocks(allStockSignals, transactionCost);
    else
    {
        cout << "Invalid execution model: " + executionModel << endl;
        return 1;
    }

    // Show result of backtest.
    vector<double>(*returnFunction)(vector<StrategyExecutionData>, double) = nullptr;
    if (returnType == "SimpleReturns")
        returnFunction = calculate_simple_returns;
    else if (returnType == "LogReturns")
        returnFunction = calculate_log_returns;
    else if (returnType == "DivReturns")
        returnFunction = calculate_div_returns;
    else
        returnFunction = calculate_simple_returns;

    for (unsigned i = 0; i < stockList.size(); i++)
    {
        vector<double> returns = returnFunction(allBacktests[i], 0.0025);
        cout << "Stock: " << stockList[i] << ", Returns: ";
        for (unsigned j = 0; j < returns.size(); j++)
        {
            if (j + 1 != returns.size())
                cout << returns[j] << ", ";
            else
                cout << returns[j] << endl;
        }

    }
}