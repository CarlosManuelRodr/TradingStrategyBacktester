#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include <limits>
#include <cassert>
#include "dataset.h"

// TODO: This is awful. Make it variable.
//#define windowSize 40
const int windowSize = 40;

class Indicator
{
public:

    //*****************************
    //*   Technical indicators    *
    //****************************/

    static std::string Date(const OCHLVData& data);
    static double OpenPrice(const OCHLVData& data);
    static double ClosePrice(const OCHLVData& data);
    static double HighPrice(const OCHLVData& data);
    static double LowPrice(const OCHLVData& data);
    static double TradingVolume(const OCHLVData& data);
    static double WeightedClose(const OCHLVData& data);
    static double TypicalPrice(const OCHLVData& data);
    static double MedianPrice(const OCHLVData& data);
    static double PricePercentageChangeOpenToClose(const OCHLVData& data);
    static double ClosingBias(const OCHLVData& data);
    static double ExtensionRatio(const OCHLVData& data);
    static double EMA(const OCHLVData& data, double previous);
    static double SMA(const std::vector<OCHLVData>& data);
    static double RSI(const std::vector<OCHLVData>& data);
    static double VWAP(const std::vector<OCHLVData>& data);
    static double OBV(const std::vector<OCHLVData>& data);
    static double ROC(const std::vector<OCHLVData>& data);
    static double MFI(const std::vector<OCHLVData>& data);

    /** Calculate all the available technical indicators from OCHLVData. */
    static Indicators CalculateIndicators(const std::vector<OCHLVData>& rawData);

    /** Calculate all the quantiles of technical indicators from OCHLVData. */
    static QuantileIndicators CalculateQuantileIndicators(const std::vector<OCHLVData>& rawData);

    //****************************
    //*   Quantile calculation    *
    //****************************/

    /**
     * Template for linear interpolation.
     * @tparam T Numeric type (int, double, etc...)
     * @param v0 Point 0.
     * @param v1 Point 1.
     * @param t The free parameter at interval [0, 1].
     * @return The interpolation evaluated at t.
     * @see https://stackoverflow.com/a/37708864
     */
    template<typename T> static inline double Lerp(T v0, T v1, T t)
    {
        return (1 - t) * v0 + t * v1;
    }

    /**
     * Calculates the quantile of the provided sample.
     * @tparam T Numeric type (int, double, etc...)
     * @param in The sample
     * @param percentile Percentile to calculate.
     * @return The value of the quantile.
     */
    template<typename T> static T CalculateQuantile(const std::vector<T>& in, const T percentile)
    {
        if (in.empty())
            return std::numeric_limits<T>::quiet_NaN();

        if (in.size() == 1)
            return in[0];

        std::vector<T> data = in;
        std::sort(data.begin(), data.end());

        T poi = Lerp<T>(-0.5, data.size() - 0.5, percentile);

        size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
        T quantile = data.at(left);

        return quantile;
    }

    //**********************************
    //*   Time-series of observables    *
    //**********************************/

    /**
     * Return a time series from a instant-value indicator.
     * @tparam T Indicator return type.
     * @param TechIndFunction The technical indicator function.
     * @param timeSeries The input time series.
     * @param window ize of the partition window.
     * @return A list containing the time-series of the evaluated indicator.
     */
    template<typename T>
    static std::vector<T> IndicatorTimeSeries(T (*TechIndFunction)(const std::vector<OCHLVData>&),
            std::vector<OCHLVData> timeSeries, const unsigned window)
    {
        std::vector<T> output;
        for (unsigned i = 0; i < timeSeries.size() - window; i++)
        {
            std::vector<OCHLVData> subsample(timeSeries.begin() + i, timeSeries.begin() + i + window);
            output.push_back(TechIndFunction(subsample));
        }

        return output;
    }

    /// Calculates a time series from a instant-value indicator.
    /// \tparam T Indicator return type.
    /// \param TechIndFunction Function pointer to the technical indicator.
    /// \param timeSeries List of OCHLVData.
    /// \return A list containing the time-series of the requested indicator.
    template<typename T>
    static std::vector<T> IndicatorTimeSeries(T (*TechIndFunction)(const OCHLVData&), std::vector<OCHLVData> timeSeries)
    {
        std::vector<T> output;
        for (auto & instant : timeSeries)
            output.push_back(TechIndFunction(instant));

        return output;
    }

    /// Calculate a time series from a lagged value indicator.
    /// \tparam T Indicator return type.
    /// \param TechIndFunction Function pointer to the technical indicator.
    /// \param timeSeries List of OCHLVData.
    /// \return A list containing the time-series of the indicator.
    template<typename T>
    static std::vector<T> IndicatorTimeSeries(T(*TechIndFunction)(const OCHLVData&, double), std::vector<OCHLVData> timeSeries)
    {
        std::vector<T> output;
        for (auto & instant : timeSeries)
        {
            if (output.empty())
                output.push_back(TechIndFunction(instant, instant.close));
            else
                output.push_back(TechIndFunction(instant, output.back()));
        }

        return output;
    }

    /// Return a time series of the indicator quantile.
    /// \tparam T Indicator return type.
    /// \param TechIndFunction Function pointer to the technical indicator.
    /// \param timeSeries List of OCHLVData.
    /// \param percentile The percentile used to calculate the quantile.
    /// \param window Size of the partition window.
    /// \return A list containing the time-series of the quantile indicator.
    template<typename T>
    static std::vector<T> QuantileTimeSeries(T(*TechIndFunction)(const std::vector<OCHLVData>&), std::vector<OCHLVData> timeSeries,
                                             double percentile, const unsigned window = windowSize)
    {
        std::vector<T> indicatorTimeSeries = IndicatorTimeSeries(TechIndFunction, timeSeries, window);

        std::vector<T> quantileTimeSeries;
        for (unsigned i = 0; i < indicatorTimeSeries.size() - window; i++)
        {
            std::vector<T> partition(indicatorTimeSeries.begin() + i, indicatorTimeSeries.begin() + i + window);
            quantileTimeSeries.push_back(CalculateQuantile(partition, percentile));
        }

        return quantileTimeSeries;
    }

    /// Return a time series of the indicator quantile.
    /// \tparam T Indicator return type.
    /// \param TechIndFunction Function pointer to the technical indicator.
    /// \param timeSeries List of OCHLVData.
    /// \param percentile The percentile used to calculate the quantile.
    /// \param window The size of the time-series window.
    /// \return A list containing the time-series of the requested quantile indicator.
    template<typename T>
    static std::vector<T> QuantileTimeSeries(T (*TechIndFunction)(const OCHLVData&), const std::vector<OCHLVData>& timeSeries,
                                             double percentile, const unsigned window = windowSize)
    {
        std::vector<T> indicatorTimeSeries = IndicatorTimeSeries(TechIndFunction, timeSeries);

        std::vector<T> quantileTimeSeries;
        for (unsigned i = 0; i < indicatorTimeSeries.size() - window; i++)
        {
            std::vector<T> partition(indicatorTimeSeries.begin() + i, indicatorTimeSeries.begin() + i + window);
            quantileTimeSeries.push_back(CalculateQuantile(partition, percentile));
        }

        return quantileTimeSeries;
    }

    /// Return a time series of the indicator quantile.
    /// \tparam T Indicator return type.
    /// \param TechIndFunction Function pointer to the technical indicator.
    /// \param timeSeries List of OCHLVData.
    /// \param percentile The percentile used to calculate the quantile.
    /// \param window The size of the time-series window.
    /// \return A list containing the time-series of the quantile indicator.
    template<typename T>
    static std::vector<T> QuantileTimeSeries(T (*TechIndFunction)(const OCHLVData&, T), const std::vector<OCHLVData>& timeSeries,
                                             double percentile, const unsigned window = windowSize)
    {
        std::vector<T> indicatorTimeSeries = IndicatorTimeSeries(TechIndFunction, timeSeries);

        std::vector<T> quantileTimeSeries;
        for (unsigned i = 0; i < indicatorTimeSeries.size() - window; i++)
        {
            std::vector<T> partition(indicatorTimeSeries.begin() + i, indicatorTimeSeries.begin() + i + window);
            quantileTimeSeries.push_back(CalculateQuantile(partition, percentile));
        }

        return quantileTimeSeries;
    }
};