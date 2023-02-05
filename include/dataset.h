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

//*****************************
//*    Raw data container     *
//****************************/

/** Container for candlestick data containing Open, Close, High, Low and Volume. */
struct OCHLVData
{
    std::string date;
    double open{}, close{}, high{}, low{}, volume{};

    /** Default constructor. */
    OCHLVData() = default;

    /** Explicit constructor. */
    OCHLVData(const std::string& date, double open, double close, double high, double low, double volume)
    {
        this->date = date;
        this->open = open;
        this->close = close;
        this->high = high;
        this->low = low;
        this->volume = volume;
    }

    explicit OCHLVData(std::vector<std::string> csvLine)
    {
        if (csvLine.size() == 6)
        {
            date = csvLine[0];
            open = Utilities::ConvertTo<double>(csvLine[1]);
            high = Utilities::ConvertTo<double>(csvLine[2]);
            low = Utilities::ConvertTo<double>(csvLine[3]);
            close = Utilities::ConvertTo<double>(csvLine[4]);
            volume = Utilities::ConvertTo<double>(csvLine[5]);
        }
        else
        {
            date = "";
            open = std::numeric_limits<double>::quiet_NaN();
            close = std::numeric_limits<double>::quiet_NaN();
            high = std::numeric_limits<double>::quiet_NaN();
            low = std::numeric_limits<double>::quiet_NaN();
            volume = std::numeric_limits<double>::quiet_NaN();
        }
    }
};

//*****************************
//*      Dataset storage      *
//****************************/

/** Indicators is an alias to an unordered_map that maps the name of an indicator to its double value. */
using Indicators = std::unordered_map<std::string, std::vector<double>>;

/** QuantileIndicators is an alias to an unordered_map that maps a percentile value to Indicators. */
using QuantileIndicators = std::unordered_map<std::string, Indicators>;

/** Serializable structure that contains all the data and indicators of a single stock. */
struct StockData
{
    std::vector<std::string> dates;
    Indicators indicators;
    QuantileIndicators quantileIndicators;

    /** Default constructor. */
    StockData() = default;

    /** Explicit constructor. */
    StockData(const std::vector<std::string>& dates, const Indicators& indicators, const QuantileIndicators& quantileIndicators)
    {
        this->dates = dates;
        this->indicators = indicators;
        this->quantileIndicators = quantileIndicators;
    }

    /** Serialization hook. */
    template <class Archive> void serialize(Archive& ar)
    {
        ar(dates, indicators, quantileIndicators);
    }
};

/** Dataset is an alias to an unordered_map that maps the name of a stock to its StockData. */
using Dataset = std::map<std::string, StockData>;