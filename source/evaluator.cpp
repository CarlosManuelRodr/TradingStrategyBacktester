#include <cassert>
#include <utility>
#include "evaluator.h"
#include "thread_pool.h"
using namespace std;
using namespace backtester;

/****************************
*     Dataset reference     *
****************************/

Dataset Evaluator::evaluatorDataset;

void Evaluator::SetStrategyEvaluatorDataset(Dataset dataset) noexcept
{
    evaluatorDataset = std::move(dataset);
}

Dataset Evaluator::GetStrategyEvaluatorDataset() noexcept
{
    return evaluatorDataset;
}

vector<string> Evaluator::GetStocksInDataset() noexcept
{
    return Utilities::Keys(evaluatorDataset);
}

/**************************************
* Observable accessors implementation *
**************************************/

string Evaluator::Date(const string& stock, int time)
{
    return evaluatorDataset[stock].dates[time];
}

vector<string> Evaluator::Dates(const string& stock)
{
    return evaluatorDataset[stock].dates;
}

double Evaluator::Indicator(const string& indicatorName, const string& stock, int time)
{
    return evaluatorDataset[stock].indicators[indicatorName][time];
}

double Evaluator::IndQuantile(const string& indicatorName, const string& percentile, const string& stock, int time)
{
    return evaluatorDataset[stock].quantileIndicators[percentile][indicatorName][time];
}

vector<double>& Evaluator::IndicatorTimeSeries(const string& indicatorName, const string& stock)
{
    return evaluatorDataset[stock].indicators[indicatorName];
}

vector<double>& Evaluator::IndQuantileTimeSeries(const string& indicatorName, const string& percentile, const string& stock)
{
    return evaluatorDataset[stock].quantileIndicators[percentile][indicatorName];
}

/****************************
*     Angescript engine     *
****************************/

const string program_part_A = R""""(bool execute(string stock, int time)
{
    if ()"""";

const string program_part_B = R""""()
    {
        return true;
    }
    else
        return false;
})"""";

string Evaluator::strategyToFunction(const string& strategy)
{
    return program_part_A + strategy + program_part_B;
}

void Evaluator::messageCallback(const asSMessageInfo* msg, [[maybe_unused]] void* param)
{
    const char* type = "[Scripting engine ERR]: ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "[Scripting engine WARN]: ";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "[Scripting engine INFO]: ";

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void Evaluator::scriptingEngineLog(const string& log)
{
    cout << "[Scripting engine LOG]: " << log << endl;
}

void Evaluator::scriptingEngineException(const string& exception)
{
    cout << "[Scripting engine EXCEPTION]: " << exception << endl;
}

string Evaluator::scriptingEngineLogStr(const string& log)
{
    return string("[Scripting engine LOG]: ") + log + "\n";
}

string Evaluator::scriptingEngineExceptionStr(const string& exception)
{
    return string("[Scripting engine EXCEPTION]: ") + exception + "\n";
}

string Evaluator::scriptingEngineProgram(const string& program)
{
    return string("\n[Strategy program]: ") + program + "\n";
}

void Evaluator::registerInterface(asIScriptEngine* engine)
{
    int r;
    r = engine->RegisterGlobalFunction("double Indicator(string, string, int)",
                                       asFUNCTIONPR(Evaluator::Indicator, (const string&, const string&, int), double),
                                       asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("double IndQuantile(string, string, string, int)",
                                       asFUNCTIONPR(Evaluator::IndQuantile, (const string&, const string&, const string&, int), double),
                                       asCALL_CDECL);
    assert(r >= 0);
}

asIScriptEngine* Evaluator::startAngelscriptEngine()
{
    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    if (engine == nullptr)
    {
        scriptingEngineLog("Couldn't start scripting engine.");
        return nullptr;
    }

    // Set the message callback to receive information on errors in human readable form.
    const int r = engine->SetMessageCallback(asFUNCTION(messageCallback), nullptr, asCALL_CDECL);
    if (r < 0)
    {
        scriptingEngineLog("Unrecoverable error while starting the engine.");
        return nullptr;
    }

    RegisterStdString(engine);
    registerInterface(engine);
    engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
    engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);

    return engine;
}

asIScriptFunction* Evaluator::compileAngelscriptStrategy(asIScriptEngine* engine, const string& strategyProgram)
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
        scriptingEngineLog("Unrecoverable error while starting a new module.");
        return nullptr;
    }
    r = builder.AddSectionFromMemory("StrategyFunction", strategyProgram.c_str(), (unsigned) strategyProgram.size());
    if (r < 0)
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        scriptingEngineLog("Please correct the errors in the script and try again.");
        return nullptr;
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        scriptingEngineLog("Please correct the errors in the script and try again.");
        return nullptr;
    }

    // Find the function that is to be called.
    const asIScriptModule* mod = engine->GetModule("StrategyModule");
    if (mod == nullptr)
    {
        scriptingEngineLog("Error getting StrategyModule.");
        return nullptr;
    }
    asIScriptFunction* func = mod->GetFunctionByDecl("bool execute(string, int)");
    if (func == nullptr)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        scriptingEngineLog(
                "The script must have the function 'bool execute(string, int)'. Please add it and try again.");
        return nullptr;
    }

    return func;
}

bool Evaluator::executeAngelscriptStrategy(asIScriptEngine* engine, asIScriptFunction* func, string stock, int dayIndex)
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
*  Strategy function evaluation   *
**********************************/

vector<bool> Evaluator::RunStrategy(const string& strategyProgram, const string& stock)
{
    vector<bool> strategyResults;
    const unsigned timePoints = (unsigned) Dates(stock).size();

    // Create the script engine
    string strategyFunction = strategyToFunction(strategyProgram);
    asIScriptEngine* engine = startAngelscriptEngine();
    asIScriptFunction* func = compileAngelscriptStrategy(engine, strategyFunction);

    for (int i = 0; i < timePoints; i++)
    {
        bool result = executeAngelscriptStrategy(engine, func, stock, i);
        strategyResults.push_back(result);
    }

    engine->ShutDownAndRelease();

    return strategyResults;
}

std::map<std::string, std::vector<bool>> Evaluator::RunStrategyAllStocks(const string &strategyProgram)
{
    // Create thread pool.
    vector<string> stocks = GetStocksInDataset();
    BS::thread_pool pool;
    vector<future<vector<bool>>> results;

    asPrepareMultithread();

    // Push tasks to thread pool and wait for them to finish.
    for (const auto& stock : stocks)
        results.push_back(pool.submit(Evaluator::RunStrategy, strategyProgram, stock));

    pool.wait_for_tasks();

    // Return the results of the completed tasks.
    map<string, vector<bool>> output;
    for (size_t i = 0; i < stocks.size(); i++)
        output[stocks[i]] = results[i].get();

    asUnprepareMultithread();

    return output;
}


/****************************
*          Testing          *
****************************/

std::pair<bool, std::string> Evaluator::ValidateStrategyProgram(const string& strategyProgram)
{
    string strategyFunction = strategyToFunction(strategyProgram);

    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    if (engine == nullptr)
        return std::make_pair(false, "Couldn't start scripting engine.");

    // Set the message callback to receive information on errors in human-readable form.
    int r = engine->SetMessageCallback(asFUNCTION(messageCallback), nullptr, asCALL_CDECL);
    if (r < 0)
    {
        string log = scriptingEngineLogStr("Unrecoverable error while starting the engine.");
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }

    RegisterStdString(engine);
    registerInterface(engine);

    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "StrategyModule");
    if (r < 0)
    {
        string log = scriptingEngineLogStr("Unrecoverable error while starting a new module.");
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }
    r = builder.AddSectionFromMemory("StrategyFunction", strategyFunction.c_str(), (unsigned)strategyFunction.size());
    if (r < 0)
    {
        string log = scriptingEngineLogStr("Please correct the errors in the script and try again.");
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        string log = scriptingEngineLogStr("Please correct the errors in the script and try again.");
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }

    // Find the function that is to be called.
    const asIScriptModule* mod = engine->GetModule("StrategyModule");
    if (mod == nullptr)
    {
        string log = "Error getting StrategyModule.";
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }
    const asIScriptFunction* func = mod->GetFunctionByDecl("bool execute(string, int)");
    if (func == nullptr)
    {
        string log = scriptingEngineLogStr("The script must have the function 'bool execute(string, int)'.");
        return std::make_pair(false, log + scriptingEngineProgram(strategyFunction));
    }

    return std::make_pair(true, "The strategy program is valid.");
}