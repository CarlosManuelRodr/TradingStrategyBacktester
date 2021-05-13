#pragma once
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <limits>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include "utilities.h"

/****************************
*                           *
*    Raw data container     *
*                           *
****************************/

/// <summary>
/// Container for candlestick data containing Open, Close, High, Low and Volume.
/// </summary>
template <typename NumericType> struct OCHLVData
{
    std::string date;
    NumericType open, close, high, low, volume;

    /// <summary>
    /// Empty constructor.
    /// </summary>
    OCHLVData()
    {
        date = "";
        open = std::numeric_limits<NumericType>::quiet_NaN();
        close = std::numeric_limits<NumericType>::quiet_NaN();;
        high = std::numeric_limits<NumericType>::quiet_NaN();;
        low = std::numeric_limits<NumericType>::quiet_NaN();;
        volume = std::numeric_limits<NumericType>::quiet_NaN();;
    }

    /// <summary>
    /// Explicit constructor.
    /// </summary>
    OCHLVData(std::string pdate, NumericType popen, NumericType pclose, NumericType phigh, NumericType plow, NumericType pvolume)
    {
        date = pdate;
        open = popen;
        close = pclose;
        high = phigh;
        low = plow;
        volume = pvolume;
    }

    /// <summary>
    /// Constructor that uses a parsed line from a csv file.
    /// </summary>
    /// <param name="csvLine">Parsed line separated into a list of strings.</param>
    OCHLVData(std::vector<std::string> csvLine)
    {
        if (csvLine.size() == 6)
        {
            date = csvLine[0];
            open = utilities::ConvertTo<NumericType>(csvLine[1]);
            high = utilities::ConvertTo<NumericType>(csvLine[2]);
            low = utilities::ConvertTo<NumericType>(csvLine[3]);
            close = utilities::ConvertTo<NumericType>(csvLine[4]);
            volume = utilities::ConvertTo<NumericType>(csvLine[5]);
        }
        else
        {
            date = "";
            open = std::numeric_limits<NumericType>::quiet_NaN();;
            close = std::numeric_limits<NumericType>::quiet_NaN();;
            high = std::numeric_limits<NumericType>::quiet_NaN();;
            low = std::numeric_limits<NumericType>::quiet_NaN();;
            volume = std::numeric_limits<NumericType>::quiet_NaN();;
        }
    }
};


/****************************
*                           *
*      Dataset storage      *
*                           *
****************************/

/// <summary>
/// Indicators is an alias to an unordered_map that maps the name of an indicator to its double value.
/// </summary>
using Indicators = std::unordered_map<std::string, std::vector<double>>;

/// <summary>
/// QuantileIndicators is an alias to an unordered_map that maps a percentile value to Indicators.
/// </summary>
using QuantileIndicators = std::unordered_map<std::string, Indicators>;

/// <summary>
/// Serializable structure that contains all the data and indicators of a stock.
/// </summary>
struct StockData
{
    std::vector<std::string> dates;
    Indicators indicators;
    QuantileIndicators quantileIndicators;

    template <class Archive> void serialize(Archive& ar)
    {
        ar(dates, indicators, quantileIndicators);
    }
};

/// <summary>
/// Dataset is an alias to an unordered_map that maps the name of a stock to its StockData.
/// </summary>
using Dataset = std::map<std::string, StockData>;