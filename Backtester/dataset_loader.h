#pragma once
#include <vector>
#include <string>
#include "dataset_types.h"
#include "csv_parser.h"

/****************************
*                           *
*      Dataset loading      *
*                           *
****************************/

/// <summary>
/// Load OCHLVData from csv file. The expected format is a csv with the first row
/// with the following attributes: "date","open","high","low","close","volume".
/// </summary>
/// <param name="path">Absolute path to csv file.</param>
/// <returns>Vector of OCHLVData entries.</returns>
std::vector<OCHLVData<double>> load_raw_dataset(const std::string path);

/// <summary>
/// Load StockData by calculating all the technical indicators from the raw OCHLVData.
/// </summary>
/// <param name="rawData">List of OCHLVData</param>
/// <returns>A StockData structure with all the calculated technical indicators.</returns>
StockData load_stockdata_from_raw(std::vector<OCHLVData<double>> rawData);

/// <summary>
/// Load StockData from csv file. Upon loading, it serializes the generated object for reuse
/// and fast loading in the next program execution.
/// </summary>
/// <param name="path">Absolute path to csv file.</param>
/// <returns>A StockData structure with all the calculated technical indicators.</returns>
StockData load_stockdata(const std::string path);

/// <summary>
/// Load a dataset of csv files located in the path.
/// </summary>
/// <param name="path">Absolute path to directory containing the dataset as csv files.</param>
/// <returns>A Dataset object.</returns>
Dataset load_dataset(const std::string path);