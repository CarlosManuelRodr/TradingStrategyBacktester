#pragma once
#include "dataset.h"

//***********************************
//*  Strategy function evaluation   *
//**********************************/

/**
 * Configure the dataset to be used by the strategy runner.
 * @param dataset Pointer to the dataset object.
 */
void set_strategy_evaluator_dataset(Dataset* dataset) noexcept;

/** Returns the pointer to the dataset used in the strategy runner. */
Dataset* get_strategy_evaluator_dataset() noexcept;

/** Returns the list of stocks present in the strategy evaluator dataset. */
std::vector<std::string> get_stocks_in_dataset() noexcept;

/**
 * Try to compile the strategy program and report compilation errors if any.
 * @param strategyProgram Program as string.
 * @return The result of the validation.
 */
std::string validate_strategy_program(const std::string& strategyProgram);

/**
 * Runs the strategy program in each date available of the stock.
 * @param strategyProgram A string with the program to run.
 * @param stock Name of the stock in the dataset.
 * @return The result of the evaluations.
 */
std::vector<bool> run_strategy(const std::string& strategyProgram, const std::string& stock);

//*****************************
//*    Observable accessors   *
//****************************/

std::string Date(const std::string& stock, size_t time);
std::vector<std::string> Dates(const std::string& stock);
double Indicator(const std::string& indicatorName, const std::string& stock, size_t time);
double IndQuantile(const std::string& indicatorName, const std::string& percentile, const std::string& stock, size_t time);
std::vector<double>& IndicatorTimeSeries(const std::string& indicatorName, const std::string& stock);
std::vector<double>& IndQuantileTimeSeries(const std::string& indicatorName, const std::string& percentile, const std::string& stock);