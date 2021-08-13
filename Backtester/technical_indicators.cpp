#include "technical_indicators.h"
#include "vector_ops.h"
using namespace std;
using namespace vectorops;

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
double RSI(const vector<OCHLVData<double>>& data)
{
    vector<double> returns;
    for (unsigned i = 0; i < data.size() - 1; i++)
        returns.push_back(data[i + 1].close - data[i].close);

    const unsigned u = (unsigned) count_if(returns.begin(), returns.end(), [](double r) {return r >= 0.0; });
    const unsigned d = (unsigned) count_if(returns.begin(), returns.end(), [](double r) {return r < 0.0; });

    return 100.0 - (100.0 / (1.0 + ((double)u / (double)d)));
}
double VWAP(const vector<OCHLVData<double>>& data)
{
    double weightedPrice = 0.0;
    double accumulatedVolume = 0.0;

    for (auto& datapoint : data)
    {
        weightedPrice += datapoint.close * datapoint.volume;
        accumulatedVolume += datapoint.volume;
    }

    return weightedPrice / accumulatedVolume;
}
double SMA(const vector<OCHLVData<double>>& data)
{
    double sum = 0.0;
    for (auto& datapoint : data)
        sum += datapoint.close;

    return sum / ((double) data.size());
}
double OBV(const vector<OCHLVData<double>>& data)
{
    double obv = 0.0;
    for (unsigned i = 1; i < data.size(); i++)
    {
        if (data[i].close > data[i - 1].close)
            obv += data[i].volume;
        else if (data[i].close < data[i - 1].close)
            obv -= data[i].volume;
    }
    return obv;
}
double ROC(const vector<OCHLVData<double>>& data)
{
    return 100.0 * ((data.back().close - data.front().close) / data.front().close);
}
double MFI(const vector<OCHLVData<double>>& data)
{
    double positiveMoneyFlow = 0.0;
    double negativeMoneyFlow = 0.0;

    for (unsigned i = 1; i < data.size(); i++)
    {
        double todayTP = TypicalPrice(data[i]);
        double yesterdayTP = TypicalPrice(data[i - 1]);

        if (todayTP > yesterdayTP)
            positiveMoneyFlow += todayTP * data[i].volume;
        else
            negativeMoneyFlow += todayTP * data[i].volume;
    }

    if (negativeMoneyFlow != 0.0)
    {
        double moneyFlowRatio = positiveMoneyFlow / negativeMoneyFlow;
        double moneyFlowIndex = 100.0 - (100.0 / (1.0 + moneyFlowRatio));
        return moneyFlowIndex;
    }
    else
        return 100.0;
}

/****************************
*                           *
*   Indicator calculation   *
*                           *
****************************/

Indicators CalculateIndicators(vector<OCHLVData<double>> rawData)
{
    Indicators ind;
    ind["OpenPrice"] = Drop(IndicatorTimeSeries(OpenPrice, rawData), 2 * windowSize);
    ind["ClosePrice"] = Drop(IndicatorTimeSeries(ClosePrice, rawData), 2 * windowSize);
    ind["HighPrice"] = Drop(IndicatorTimeSeries(HighPrice, rawData), 2 * windowSize);
    ind["LowPrice"] = Drop(IndicatorTimeSeries(LowPrice, rawData), 2 * windowSize);
    ind["TradingVolume"] = Drop(IndicatorTimeSeries(TradingVolume, rawData), 2 * windowSize);
    ind["WeightedClose"] = Drop(IndicatorTimeSeries(WeightedClose, rawData), 2 * windowSize);
    ind["TypicalPrice"] = Drop(IndicatorTimeSeries(TypicalPrice, rawData), 2 * windowSize);
    ind["MedianPrice"] = Drop(IndicatorTimeSeries(MedianPrice, rawData), 2 * windowSize);
    ind["PricePercentangeChangeOpenToClose"] = Drop(IndicatorTimeSeries(PricePercentangeChangeOpenToClose, rawData), 2 * windowSize);
    ind["ClosingBias"] = Drop(IndicatorTimeSeries(ClosingBias, rawData), 2 * windowSize);
    ind["ExtensionRatio"] = Drop(IndicatorTimeSeries(ExtensionRatio, rawData), 2 * windowSize);
    ind["SMA"] = Drop(IndicatorTimeSeries(SMA, rawData, windowSize), windowSize);
    ind["EMA"] = Drop(IndicatorTimeSeries(EMA, rawData), 2 * windowSize);
    ind["RSI"] = Drop(IndicatorTimeSeries(RSI, rawData, windowSize),  windowSize);
    ind["VWAP"] = Drop(IndicatorTimeSeries(VWAP, rawData, windowSize), windowSize);
    ind["OBV"] = Drop(IndicatorTimeSeries(OBV, rawData, windowSize), windowSize);
    ind["ROC"] = Drop(IndicatorTimeSeries(ROC, rawData, windowSize), windowSize);
    ind["MFI"] = Drop(IndicatorTimeSeries(MFI, rawData, windowSize), windowSize);
    return ind;
}
QuantileIndicators CalculateQuantileIndicators(vector<OCHLVData<double>> rawData)
{
    const vector<double> allPercentiles { 0.05, 0.15, 0.25, 0.75, 0.85, 0.95 };
    QuantileIndicators outputQuantileIndicators;

    for (double percentile : allPercentiles)
    {
        Indicators ind;
        ind["OpenPrice"] = Drop(QuantileTimeSeries<double>(OpenPrice, rawData, percentile), windowSize);
        ind["ClosePrice"] = Drop(QuantileTimeSeries(ClosePrice, rawData, percentile), windowSize);
        ind["HighPrice"] = Drop(QuantileTimeSeries(HighPrice, rawData, percentile), windowSize);
        ind["LowPrice"] = Drop(QuantileTimeSeries(LowPrice, rawData, percentile), windowSize);
        ind["TradingVolume"] = Drop(QuantileTimeSeries(TradingVolume, rawData, percentile), windowSize);
        ind["WeightedClose"] = Drop(QuantileTimeSeries(WeightedClose, rawData, percentile), windowSize);
        ind["TypicalPrice"] = Drop(QuantileTimeSeries(TypicalPrice, rawData, percentile), windowSize);
        ind["MedianPrice"] = Drop(QuantileTimeSeries(MedianPrice, rawData, percentile), windowSize);
        ind["PricePercentangeChangeOpenToClose"] = Drop(QuantileTimeSeries(PricePercentangeChangeOpenToClose, rawData, percentile), windowSize);
        ind["ClosingBias"] = Drop(QuantileTimeSeries(ClosingBias, rawData, percentile), windowSize);
        ind["ExtensionRatio"] = Drop(QuantileTimeSeries(ExtensionRatio, rawData, percentile), windowSize);
        ind["SMA"] = QuantileTimeSeries(SMA, rawData, percentile);
        ind["EMA"] = Drop(QuantileTimeSeries(EMA, rawData, percentile), windowSize);
        ind["RSI"] = QuantileTimeSeries(RSI, rawData, percentile);
        ind["VWAP"] = QuantileTimeSeries(VWAP, rawData, percentile);
        ind["OBV"] = QuantileTimeSeries(OBV, rawData, percentile);
        ind["ROC"] = QuantileTimeSeries(ROC, rawData, percentile);
        ind["MFI"] = QuantileTimeSeries(MFI, rawData, percentile);

        const string percentileString = utilities::NumberToString(percentile, 2);
        outputQuantileIndicators[percentileString] = ind;
    }

    return outputQuantileIndicators;
}