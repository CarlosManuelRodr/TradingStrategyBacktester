#pragma once
#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptstdstring.h>
#include "dataset.h"

//***********************************
//*  Strategy function evaluation   *
//**********************************/

class Evaluator
{
private:
    static std::string strategyToFunction(const std::string &strategy);
    static void messageCallback(const asSMessageInfo *msg, void *param);
    static void scriptingEngineLog(const std::string &log);
    static void scriptingEngineException(const std::string &exception);
    static std::string scriptingEngineLogStr(const std::string &log);
    static std::string scriptingEngineExceptionStr(const std::string &exception);
    static std::string scriptingEngineProgram(const std::string &program);
    static void registerInterface(asIScriptEngine *engine);
    static asIScriptEngine *startAngelscriptEngine();
    static asIScriptFunction *compileAngelscriptStrategy(asIScriptEngine *engine, const std::string &strategyProgram);
    static bool executeAngelscriptStrategy(asIScriptEngine *engine, asIScriptFunction *func, std::string stock, int dayIndex);

public:
    /**
     * Configure the dataset to be used by the strategy runner.
     * @param dataset Pointer to the dataset object.
     */
    static void SetStrategyEvaluatorDataset(Dataset& dataset) noexcept;

    /** Returns the pointer to the dataset used in the strategy runner. */
    static Dataset* GetStrategyEvaluatorDataset() noexcept;

    /** Returns the list of stocks present in the strategy evaluator dataset. */
    static std::vector<std::string> GetStocksInDataset() noexcept;

    /**
     * Try to compile the strategy program and report compilation errors if any.
     * @param strategyProgram Program as string.
     * @return The result of the validation.
     */
    static std::string ValidateStrategyProgram(const std::string& strategyProgram);

    /**
     * Runs the strategy program in each date available of the stock.
     * @param strategyProgram A string with the program to run.
     * @param stock Name of the stock in the dataset.
     * @return The result of the evaluations.
     */
    static std::vector<bool> RunStrategy(const std::string& strategyProgram, const std::string& stock);

    //*****************************
    //*    Observable accessors   *
    //****************************/

    static std::string Date(const std::string& stock, size_t time);
    static std::vector<std::string> Dates(const std::string& stock);
    static double Indicator(const std::string& indicatorName, const std::string& stock, size_t time);
    static double IndQuantile(const std::string& indicatorName, const std::string& percentile, const std::string& stock, size_t time);
    static std::vector<double>& IndicatorTimeSeries(const std::string& indicatorName, const std::string& stock);
    static std::vector<double>& IndQuantileTimeSeries(const std::string& indicatorName, const std::string& percentile, const std::string& stock);
};