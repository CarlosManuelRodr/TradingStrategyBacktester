#pragma once
#include "dataset_types.h"

/**********************************
*                                 *
*  Strategy function evaluation   *
*                                 *
**********************************/

/// <summary>
/// Configure the dataset to be used by the strategy runner.
/// </summary>
/// <param name="dataset">Pointer to the dataset object.</param>
void set_strategy_evaluator_dataset(Dataset* dataset) noexcept;

/// <summary>
/// Returns the pointer to the dataset used in the strategy runner.
/// </summary>
Dataset* get_strategy_evaluator_dataset() noexcept;

/// <summary>
/// Try to compile the strategy program and report compilation errors if any.
/// </summary>
/// <param name="strategyProgram">Program as string.</param>
/// <returns>The result of the test.</returns>
std::string test_strategy_program(std::string strategyProgram);

/// <summary>
/// Runs the strategy program in each date available of the stock.
/// </summary>
/// <param name="strategyProgram">A string with the program to run.</param>
/// <param name="stock">Name of the stock in the dataset.</param>
/// <returns>The result of the evaluations.</returns>
std::vector<bool> run_strategy(const std::string& strategyProgram, const std::string& stock);

/// <summary>
/// Runs the strategy program in each date of all the stocks in the dataset.
/// </summary>
/// <param name="strategyProgram">A string with the program to run.</param>
/// <param name="pararellize">Run strategy for each stock in parallel?.</param>
/// <returns>The result of the evaluations.</returns>
std::vector<std::vector<bool>>run_strategy_for_all_stocks(const std::string& strategyProgram, bool pararellize = true);

/****************************
*                           *
*    Observable accesors    *
*                           *
****************************/

std::string Date(std::string stock, int time);
std::vector<std::string> Dates(std::string stock);
double Indicator(std::string indicatorName, std::string stock, int time);
double IndQuantile(std::string indicatorName, std::string percentile, std::string stock, int time);
std::vector<double>& IndicatorTimeSeries(std::string indicatorName, std::string stock);
std::vector<double>& IndQuantileTimeSeries(std::string indicatorName, std::string percentile, std::string stock);