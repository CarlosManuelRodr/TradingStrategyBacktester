#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptarray.h>
#include <scriptstdstring.h>
#include <cassert>
#include <cmath>
#include <random>
#include "strategy_evaluator.h"
#include "functional_map.h"
using namespace std;

/****************************
*                           *
*     Dataset reference     *
*                           *
****************************/

Dataset* strategyRunnerDataset = nullptr;

void set_strategy_evaluator_dataset(Dataset* dataset) noexcept
{
    strategyRunnerDataset = dataset;
}

Dataset* get_strategy_evaluator_dataset() noexcept
{
    return strategyRunnerDataset;
}

/****************************
*                           *
* Observable implementation *
*                           *
****************************/

string Date(string stock, int time)
{
    return (*strategyRunnerDataset)[stock].dates[time];
}

vector<string> Dates(string stock)
{
    return (*strategyRunnerDataset)[stock].dates;
}

double Indicator(string indicatorName, string stock, int time)
{
    return (*strategyRunnerDataset)[stock].indicators[indicatorName][time];
}

double IndQuantile(string indicatorName, string percentile, string stock, int time)
{
    return (*strategyRunnerDataset)[stock].quantileIndicators[percentile][indicatorName][time];
}

vector<double>& IndicatorTimeSeries(string indicatorName, string stock)
{
    return (*strategyRunnerDataset)[stock].indicators[indicatorName];
}

vector<double>& IndQuantileTimeSeries(string indicatorName, string percentile, string stock)
{
    return (*strategyRunnerDataset)[stock].quantileIndicators[percentile][indicatorName];
}

/****************************
*                           *
*     Angescript engine     *
*                           *
****************************/

const string program_part_A = R""""(bool strategy_result(string stock, int time)
{
    if ()"""";

const string program_part_B = R""""()
    {
        return true;
    }
    else
        return false;
})"""";

string strategy_to_function(const string& strategy) noexcept
{
    return program_part_A + strategy + program_part_B;
}

void message_callback(const asSMessageInfo* msg, void* param) noexcept
{
    const char* type = "[Scripting engine ERR]: ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "[Scripting engine WARN]: ";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "[Scripting engine INFO]: ";

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void scripting_engine_log(string log)
{
    cout << "[Scripting engine LOG]: " << log << endl;
}

void scripting_engine_exception(string exception)
{
    cout << "[Scripting engine EXCEPTION]: " << exception << endl;
}

string scripting_engine_log_str(string log)
{
    return string("[Scripting engine LOG]: ") + log + "\n";
}

string scripting_engine_exception_str(string exception)
{
    return string("[Scripting engine EXCEPTION]: ") + exception + "\n";
}

string scripting_engine_program(string program)
{
    return string("\n[Strategy program]: ") + program + "\n";
}

void register_interface(asIScriptEngine* engine)
{
    if (engine == nullptr)
        return;

    int r;
    r = engine->RegisterGlobalFunction("double Indicator(string, string, int)", 
                                       asFUNCTIONPR(Indicator, (string, string, int), double),
                                       asCALL_CDECL); 
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("double IndQuantile(string, string, string, int)",
                                       asFUNCTIONPR(IndQuantile, (string, string, string, int), double),
                                       asCALL_CDECL);
    assert(r >= 0);
}

asIScriptEngine* start_angelscript_engine()
{
    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    if (engine == nullptr)
    {
        scripting_engine_log("Couldn't start scripting engine.");
        return nullptr;
    }

    // Set the message callback to receive information on errors in human readable form.
    const int r = engine->SetMessageCallback(asFUNCTION(message_callback), 0, asCALL_CDECL);
    if (r < 0)
    {
        scripting_engine_log("Unrecoverable error while starting the engine.");
        return nullptr;
    }

    RegisterStdString(engine);
    register_interface(engine);
    engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
    engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);
    return engine;
}

asIScriptFunction* compile_angelscript_strategy(asIScriptEngine* engine, string strategyProgram)
{
    if (engine == nullptr)
        return nullptr;

    // The CScriptBuilder helper is an add-on that loads the file,
    // performs a pre-processing pass if necessary, and then tells
    // the engine to build a script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, "StrategyModule");
    if (r < 0)
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        scripting_engine_log("Unrecoverable error while starting a new module.");
        return nullptr;
    }
    r = builder.AddSectionFromMemory("StrategyFunction", strategyProgram.c_str(), (unsigned) strategyProgram.size());
    if (r < 0)
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        scripting_engine_log("Please correct the errors in the script and try again.");
        return nullptr;
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        // An error occurred. Instruct the script writer to fix the 
        // compilation errors that were listed in the output stream.
        scripting_engine_log("Please correct the errors in the script and try again.");
        return nullptr;
    }

    // Find the function that is to be called. 
    const asIScriptModule* mod = engine->GetModule("StrategyModule");
    if (mod == nullptr)
    {
        scripting_engine_log("Error getting StrategyModule.");
        return nullptr;
    }
    asIScriptFunction* func = mod->GetFunctionByDecl("bool strategy_result(string, int)");
    if (func == nullptr)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        scripting_engine_log("The script must have the function 'bool strategy_result(string, int)'. Please add it and try again.");
        return nullptr;
    }

    return func;
}

bool execute_angelscript_strategy(asIScriptEngine* engine, asIScriptFunction* func, string stock, int dayIndex)
{
    // Create our context, prepare it, and then execute
    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(func);

    ctx->SetArgObject(0, &stock);
    ctx->SetArgDWord(1, dayIndex);

    int r = ctx->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        // Call to null value during execution
        if (r == asEXECUTION_EXCEPTION)
        {
            ctx->Release();
            return false;
        }
    }

    // Retrieve the return value from the context
    const bool returnValue = ctx->GetReturnByte();
    ctx->Release();

    asThreadCleanup();

    return returnValue;
}

/**********************************
*                                 *
*  Strategy function evaluation   *
*                                 *
**********************************/

vector<bool> run_strategy(const string& strategyProgram, const string& stock)
{
    vector<bool> strategyResults;
    const unsigned timePoints = (unsigned) Dates(stock).size();

    // Create the script engine
    string strategyFunction = strategy_to_function(strategyProgram);
    asIScriptEngine* engine = start_angelscript_engine();
    asIScriptFunction* func = compile_angelscript_strategy(engine, strategyFunction);

    for (unsigned i = 0; i < timePoints; i++)
    {
        bool result = execute_angelscript_strategy(engine, func, stock, i);
        strategyResults.push_back(result);
    }

    engine->ShutDownAndRelease();

    return strategyResults;
}

vector<vector<bool>> run_strategy_for_all_stocks_st(const string& strategyProgram)
{
    vector<vector<bool>> strategyResults;
    vector<string> stocks = utilities::Keys(*strategyRunnerDataset);

    for (string& s : stocks)
        strategyResults.push_back(run_strategy(strategyProgram, s));

    return strategyResults;
}

vector<vector<bool>> run_strategy_for_all_stocks_mt(const string& strategyProgram)
{
    const unsigned int hc = thread::hardware_concurrency();
    ThreadPool pool(hc);
    vector<future<vector<bool>>> result;
    vector<vector<bool>> strategyResults;
    vector<string> stocks = utilities::Keys(*strategyRunnerDataset);

    asPrepareMultithread();

    for (string& s : stocks)
        result.push_back(pool.enqueue(run_strategy, strategyProgram, s));

    for (auto& r : result)
        strategyResults.push_back(r.get());

    asUnprepareMultithread();

    return strategyResults;
}

vector<vector<bool>>run_strategy_for_all_stocks(const string& strategyProgram, bool pararellize)
{
    if (pararellize)
        return run_strategy_for_all_stocks_mt(strategyProgram);
    else
        return run_strategy_for_all_stocks_st(strategyProgram);
}

/****************************
*                           *
*          Testing          *
*                           *
****************************/

string test_strategy_program(string strategyProgram)
{
    string strategyFunction = strategy_to_function(strategyProgram);

    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    if (engine == nullptr)
    {
        return "Couldn't start scripting engine.";
    }

    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(message_callback), 0, asCALL_CDECL);
    if (r < 0)
    {
        string log = scripting_engine_log_str("Unrecoverable error while starting the engine.");
        return log + scripting_engine_program(strategyFunction);
    }

    RegisterStdString(engine);
    register_interface(engine);

    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "StrategyModule");
    if (r < 0)
    {
        string log = scripting_engine_log_str("Unrecoverable error while starting a new module.");
        return log + scripting_engine_program(strategyFunction);
    }
    r = builder.AddSectionFromMemory("StrategyFunction", strategyFunction.c_str(), (unsigned)strategyFunction.size());
    if (r < 0)
    {
        string log = scripting_engine_log_str("Please correct the errors in the script and try again.");
        return log + scripting_engine_program(strategyFunction);
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        string log = scripting_engine_log_str("Please correct the errors in the script and try again.");
        return log + scripting_engine_program(strategyFunction);
    }

    // Find the function that is to be called. 
    const asIScriptModule* mod = engine->GetModule("StrategyModule");
    if (mod == nullptr)
    {
        string log = "Error getting StrategyModule.";
        return log + scripting_engine_program(strategyFunction);
    }
    const asIScriptFunction* func = mod->GetFunctionByDecl("bool strategy_result(string, int)");
    if (func == nullptr)
    {
        string log = scripting_engine_log_str("The script must have the function 'bool strategy_result(string, int)'.");
        return log + scripting_engine_program(strategyFunction);
    }

    return string("The strategy program is OK.");
}