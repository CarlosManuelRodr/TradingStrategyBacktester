#include "technical_indicators.h"
using namespace std;

/****************************
*                           *
*   Technical indicators    *
*                           *
****************************/

string Date(OCHLVData<double> data)
{
    return data.date;
}
double OpenPrice(OCHLVData<double> data)
{
    return data.open;
}
double ClosePrice(OCHLVData<double> data)
{
    return data.close;
}
double HighPrice(OCHLVData<double> data)
{
    return data.high;
}
double LowPrice(OCHLVData<double> data)
{
    return data.low;
}
double TradingVolume(OCHLVData<double> data)
{
    return data.volume;
}
double WeightedClose(OCHLVData<double> data)
{
    return (2.0 * data.close + data.high + data.low) / 4.0;
}
double TypicalPrice(OCHLVData<double> data)
{
    return (data.close + data.high + data.low) / 3.0;
}
double MedianPrice(OCHLVData<double> data)
{
    return (data.high + data.low) / 2.0;
}
double PricePercentangeChangeOpenToClose(OCHLVData<double> data)
{
    return 100.0 * (data.close / data.open - 1.0);
}
double ClosingBias(OCHLVData<double> data)
{
    return 100.0 * ((data.close - data.low) / (data.high - data.low));
}
double ExtensionRatio(OCHLVData<double> data)
{
    return 100.0 * ((data.close - data.open) / (data.high - data.low));
}
double EMA(OCHLVData<double> data, double previous)
{
    const double alpha = 2.0 / (windowSize + 1.0);
    return alpha * (data.close - previous) + previous;
}

/****************************
*                           *
*   Indicator calculation   *
*                           *
****************************/

Indicators CalculateIndicators(vector<OCHLVData<double>> rawData)
{
    Indicators ind;
    ind["OpenPrice"] = IndicatorTimeSeries(OpenPrice, rawData);
    ind["ClosePrice"] = IndicatorTimeSeries(ClosePrice, rawData);
    ind["HighPrice"] = IndicatorTimeSeries(HighPrice, rawData);
    ind["LowPrice"] = IndicatorTimeSeries(LowPrice, rawData);
    ind["TradingVolume"] = IndicatorTimeSeries(TradingVolume, rawData);
    ind["WeightedClose"] = IndicatorTimeSeries(WeightedClose, rawData);
    ind["TypicalPrice"] = IndicatorTimeSeries(TypicalPrice, rawData);
    ind["MedianPrice"] = IndicatorTimeSeries(MedianPrice, rawData);
    ind["PricePercentangeChangeOpenToClose"] = IndicatorTimeSeries(PricePercentangeChangeOpenToClose, rawData);
    ind["ClosingBias"] = IndicatorTimeSeries(ClosingBias, rawData);
    ind["ExtensionRatio"] = IndicatorTimeSeries(ExtensionRatio, rawData);
    ind["EMA"] = IndicatorTimeSeries(EMA, rawData);
    return ind;
}
QuantileIndicators CalculateQuantileIndicators(vector<OCHLVData<double>> rawData)
{
    const vector<double> allPercentiles { 0.05, 0.15, 0.25, 0.75, 0.85, 0.95 };
    QuantileIndicators outputQuantileIndicators;

    for (double percentile : allPercentiles)
    {
        Indicators ind;
        ind["OpenPrice"] = QuantileTimeSeries<double>(OpenPrice, rawData, percentile);
        ind["ClosePrice"] = QuantileTimeSeries(ClosePrice, rawData, percentile);
        ind["HighPrice"] = QuantileTimeSeries(HighPrice, rawData, percentile);
        ind["LowPrice"] = QuantileTimeSeries(LowPrice, rawData, percentile);
        ind["TradingVolume"] = QuantileTimeSeries(TradingVolume, rawData, percentile);
        ind["WeightedClose"] = QuantileTimeSeries(WeightedClose, rawData, percentile);
        ind["TypicalPrice"] = QuantileTimeSeries(TypicalPrice, rawData, percentile);
        ind["MedianPrice"] = QuantileTimeSeries(MedianPrice, rawData, percentile);
        ind["PricePercentangeChangeOpenToClose"] = QuantileTimeSeries(PricePercentangeChangeOpenToClose, rawData, percentile);
        ind["ClosingBias"] = QuantileTimeSeries(ClosingBias, rawData, percentile);
        ind["ExtensionRatio"] = QuantileTimeSeries(ExtensionRatio, rawData, percentile);
        ind["EMA"] = QuantileTimeSeries(EMA, rawData, percentile);

        const string percentileString = utilities::NumberToString(percentile, 2);
        outputQuantileIndicators[percentileString] = ind;
    }

    return outputQuantileIndicators;
}