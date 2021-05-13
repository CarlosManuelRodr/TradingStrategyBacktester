#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include <limits>
#include <cassert>
#include "dataset_types.h"
#include "global_definitions.h"

/****************************
*                           *
*   Technical indicators    *
*                           *
****************************/

std::string Date(OCHLVData<double> data);
double OpenPrice(OCHLVData<double> data);
double ClosePrice(OCHLVData<double> data);
double HighPrice(OCHLVData<double> data);
double LowPrice(OCHLVData<double> data);
double TradingVolume(OCHLVData<double> data);
double WeightedClose(OCHLVData<double> data);
double TypicalPrice(OCHLVData<double> data);
double MedianPrice(OCHLVData<double> data);
double PricePercentangeChangeOpenToClose(OCHLVData<double> data);
double ClosingBias(OCHLVData<double> data);
double ExtensionRatio(OCHLVData<double> data);
double EMA(OCHLVData<double> data, double previous);

/// <summary>
/// Calculate all the technical indicators from OCHLVData.
/// </summary>
Indicators CalculateIndicators(std::vector<OCHLVData<double>> rawData);

/// <summary>
/// Calculate all the quantiles of technical indicators from OCHLVData.
/// </summary>
QuantileIndicators CalculateQuantileIndicators(std::vector<OCHLVData<double>> rawData);

/****************************
*                           *
*   Quantile calculation    *
*                           *
****************************/

// Source: https://stackoverflow.com/a/37708864

/// <summary>
/// Template for linear interpolation.
/// </summary>
template<typename T> static inline double Lerp(T v0, T v1, T t)
{
    return (1 - t) * v0 + t * v1;
}

/// <summary>
/// Calculates the quantile of the sample provided in inData.
/// </summary>
/// <typeparam name="T">Numeric type (int, double, etc...)</typeparam>
/// <param name="inData">Vector of data.</param>
/// <param name="percentile">Percentile to calculate.</param>
/// <returns>The value of the quantile.</returns>
template<typename T> T CalculateQuantile(const std::vector<T>& inData, const T percentile)
{
    if (inData.empty())
        return std::numeric_limits<T>::quiet_NaN();

    if (inData.size() == 1)
        return inData[0];

    std::vector<T> data = inData;
    std::sort(data.begin(), data.end());

    T poi = Lerp<T>(-0.5, data.size() - 0.5, percentile);

    size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
    T quantile = data.at(left);

    return quantile;
}

/**********************************
*                                 *
*   Time-series of observables    *
*                                 *
**********************************/

/// <summary>
/// Return a time series from a instant value indicator.
/// </summary>
/// <typeparam name="T">Indicator return type.</typeparam>
/// <param name="TechIndFunction">Function pointer to the technical indicator.</param>
/// <param name="timeSeries">List of OCHLVData.</param>
/// <param name="window">Size of the partition window.</param>
/// <returns>A list containing the time series of the requested indicator.</returns>
template<typename T> std::vector<T> IndicatorTimeSeries(T (*TechIndFunction)(OCHLVData<double>), std::vector<OCHLVData<double>> timeSeries,
                                                        const unsigned window = windowSize)
{
    std::vector<T> output;
    for (unsigned i = window - 1; i < timeSeries.size(); i++)
        output.push_back(TechIndFunction(timeSeries[i]));

    return output;
}

/// <summary>
/// Return a time series from a lagged value indicator.
/// </summary>
/// <typeparam name="T">Indicator return type.</typeparam>
/// <param name="TechIndFunction">Function pointer to the technical indicator.</param>
/// <param name="timeSeries">List of OCHLVData.</param>
/// <param name="window">Size of the partition window.</param>
/// <returns>A list containing the time series of the requested indicator.</returns>
template<typename T> std::vector<T> IndicatorTimeSeries(T(*TechIndFunction)(OCHLVData<double>, double), std::vector<OCHLVData<double>> timeSeries,
                                                        const unsigned window = windowSize)
{
    std::vector<T> output;
    for (unsigned i = window - 1; i < timeSeries.size(); i++)
    {
        if (output.size() == 0)
            output.push_back(TechIndFunction(timeSeries[i], timeSeries[i].close));
        else
            output.push_back(TechIndFunction(timeSeries[i], output.back()));
    }

    return output;
}

/// <summary>
/// Return a time series of the indicator quantile.
/// </summary>
/// <typeparam name="T">Indicator return type.</typeparam>
/// <param name="TechIndFunction">Function pointer to the technical indicator.</param>
/// <param name="timeSeries">List of OCHLVData.</param>
/// <param name="percentile">The percentile used to calculate the quantile.</param>
/// <returns>A list containing the time series of the requested quantile indicator.</returns>
template<typename T> std::vector<T> QuantileTimeSeries(T (*TechIndFunction)(OCHLVData<double>), std::vector<OCHLVData<double>> timeSeries, 
                                                       double percentile)
{
    const unsigned window = windowSize;
    std::vector<T> indicatorTimeSeries = IndicatorTimeSeries(TechIndFunction, timeSeries, 1);

    std::vector<T> quantileTimeSeries;
    for (unsigned i = 0; i < indicatorTimeSeries.size() - window + 1; i++)
    {
        std::vector<T> partition(indicatorTimeSeries.begin() + i, indicatorTimeSeries.begin() + i + window);
        quantileTimeSeries.push_back(CalculateQuantile(partition, percentile));
    }

    return quantileTimeSeries;
}

/// <summary>
/// Return a time series of the indicator quantile.
/// </summary>
/// <typeparam name="T">Indicator return type.</typeparam>
/// <param name="TechIndFunction">Function pointer to the technical indicator.</param>
/// <param name="timeSeries">List of OCHLVData.</param>
/// <param name="percentile">The percentile used to calculate the quantile.</param>
/// <returns>A list containing the time series of the requested quantile indicator.</returns>
template<typename T> std::vector<T> QuantileTimeSeries(T (*TechIndFunction)(OCHLVData<double>, T), std::vector<OCHLVData<double>> timeSeries,
                                                       double percentile)
{
    const unsigned window = windowSize;
    std::vector<T> indicatorTimeSeries = IndicatorTimeSeries(TechIndFunction, timeSeries, 1);

    std::vector<T> quantileTimeSeries;
    for (unsigned i = 0; i < indicatorTimeSeries.size() - window + 1; i++)
    {
        std::vector<T> partition(indicatorTimeSeries.begin() + i, indicatorTimeSeries.begin() + i + window);
        quantileTimeSeries.push_back(CalculateQuantile(partition, percentile));
    }

    return quantileTimeSeries;
}