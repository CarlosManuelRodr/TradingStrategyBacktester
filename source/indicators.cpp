#include "indicators.h"
#include "vector_ops.h"
using namespace std;

//*****************************
//*   Technical indicators    *
//****************************/

string Indicator::Date(const OCHLVData& data)
{
    return data.date;
}
double Indicator::OpenPrice(const OCHLVData& data)
{
    return data.open;
}
double Indicator::ClosePrice(const OCHLVData& data)
{
    return data.close;
}
double Indicator::HighPrice(const OCHLVData& data)
{
    return data.high;
}
double Indicator::LowPrice(const OCHLVData& data)
{
    return data.low;
}
double Indicator::TradingVolume(const OCHLVData& data)
{
    return data.volume;
}
double Indicator::WeightedClose(const OCHLVData& data)
{
    return (2.0 * data.close + data.high + data.low) / 4.0;
}
double Indicator::TypicalPrice(const OCHLVData& data)
{
    return (data.close + data.high + data.low) / 3.0;
}
double Indicator::MedianPrice(const OCHLVData& data)
{
    return (data.high + data.low) / 2.0;
}
double Indicator::PricePercentageChangeOpenToClose(const OCHLVData& data)
{
    return 100.0 * (data.close / data.open - 1.0);
}
double Indicator::ClosingBias(const OCHLVData& data)
{
    return 100.0 * ((data.close - data.low) / (data.high - data.low));
}
double Indicator::ExtensionRatio(const OCHLVData& data)
{
    return 100.0 * ((data.close - data.open) / (data.high - data.low));
}
double Indicator::EMA(const OCHLVData& data, double previous)
{
    const double alpha = 2.0 / (windowSize + 1.0);
    return alpha * (data.close - previous) + previous;
}
double Indicator::RSI(const vector<OCHLVData>& data)
{
    vector<double> returns;
    for (unsigned i = 0; i < data.size() - 1; i++)
        returns.push_back(data[i + 1].close - data[i].close);

    const auto u = (unsigned) count_if(returns.begin(), returns.end(), [](double r) {return r >= 0.0; });
    const auto d = (unsigned) count_if(returns.begin(), returns.end(), [](double r) {return r < 0.0; });

    return 100.0 - (100.0 / (1.0 + ((double)u / (double)d)));
}
double Indicator::VWAP(const vector<OCHLVData>& data)
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
double Indicator::SMA(const vector<OCHLVData>& data)
{
    double sum = 0.0;
    for (auto& datapoint : data)
        sum += datapoint.close;

    return sum / ((double) data.size());
}
double Indicator::OBV(const vector<OCHLVData>& data)
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
double Indicator::ROC(const vector<OCHLVData>& data)
{
    return 100.0 * ((data.back().close - data.front().close) / data.front().close);
}
double Indicator::MFI(const vector<OCHLVData>& data)
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
*   Indicator calculation   *
****************************/

Indicators Indicator::CalculateIndicators(const vector<OCHLVData>& rawData)
{
    Indicators ind;
    ind["OpenPrice"] = VectorOps::Drop(IndicatorTimeSeries(OpenPrice, rawData), 2 * windowSize);
    ind["ClosePrice"] = VectorOps::Drop(IndicatorTimeSeries(ClosePrice, rawData), 2 * windowSize);
    ind["HighPrice"] = VectorOps::Drop(IndicatorTimeSeries(HighPrice, rawData), 2 * windowSize);
    ind["LowPrice"] = VectorOps::Drop(IndicatorTimeSeries(LowPrice, rawData), 2 * windowSize);
    ind["TradingVolume"] = VectorOps::Drop(IndicatorTimeSeries(TradingVolume, rawData), 2 * windowSize);
    ind["WeightedClose"] = VectorOps::Drop(IndicatorTimeSeries(WeightedClose, rawData), 2 * windowSize);
    ind["TypicalPrice"] = VectorOps::Drop(IndicatorTimeSeries(TypicalPrice, rawData), 2 * windowSize);
    ind["MedianPrice"] = VectorOps::Drop(IndicatorTimeSeries(MedianPrice, rawData), 2 * windowSize);
    ind["PricePercentageChangeOpenToClose"] = VectorOps::Drop(IndicatorTimeSeries(PricePercentageChangeOpenToClose, rawData), 2 * windowSize);
    ind["ClosingBias"] = VectorOps::Drop(IndicatorTimeSeries(ClosingBias, rawData), 2 * windowSize);
    ind["ExtensionRatio"] = VectorOps::Drop(IndicatorTimeSeries(ExtensionRatio, rawData), 2 * windowSize);
    ind["SMA"] = VectorOps::Drop(IndicatorTimeSeries(SMA, rawData, windowSize), windowSize);
    ind["EMA"] = VectorOps::Drop(IndicatorTimeSeries(EMA, rawData), 2 * windowSize);
    ind["RSI"] = VectorOps::Drop(IndicatorTimeSeries(RSI, rawData, windowSize),  windowSize);
    ind["VWAP"] = VectorOps::Drop(IndicatorTimeSeries(VWAP, rawData, windowSize), windowSize);
    ind["OBV"] = VectorOps::Drop(IndicatorTimeSeries(OBV, rawData, windowSize), windowSize);
    ind["ROC"] = VectorOps::Drop(IndicatorTimeSeries(ROC, rawData, windowSize), windowSize);
    ind["MFI"] = VectorOps::Drop(IndicatorTimeSeries(MFI, rawData, windowSize), windowSize);
    return ind;
}
QuantileIndicators Indicator::CalculateQuantileIndicators(const vector<OCHLVData>& rawData)
{
    const vector<double> allPercentiles { 0.05, 0.15, 0.25, 0.75, 0.85, 0.95 };
    QuantileIndicators outputQuantileIndicators;

    for (double percentile : allPercentiles)
    {
        Indicators ind;
        ind["OpenPrice"] = VectorOps::Drop(QuantileTimeSeries<double>(OpenPrice, rawData, percentile), windowSize);
        ind["ClosePrice"] = VectorOps::Drop(QuantileTimeSeries(ClosePrice, rawData, percentile), windowSize);
        ind["HighPrice"] = VectorOps::Drop(QuantileTimeSeries(HighPrice, rawData, percentile), windowSize);
        ind["LowPrice"] = VectorOps::Drop(QuantileTimeSeries(LowPrice, rawData, percentile), windowSize);
        ind["TradingVolume"] = VectorOps::Drop(QuantileTimeSeries(TradingVolume, rawData, percentile), windowSize);
        ind["WeightedClose"] = VectorOps::Drop(QuantileTimeSeries(WeightedClose, rawData, percentile), windowSize);
        ind["TypicalPrice"] = VectorOps::Drop(QuantileTimeSeries(TypicalPrice, rawData, percentile), windowSize);
        ind["MedianPrice"] = VectorOps::Drop(QuantileTimeSeries(MedianPrice, rawData, percentile), windowSize);
        ind["PricePercentageChangeOpenToClose"] = VectorOps::Drop(QuantileTimeSeries(PricePercentageChangeOpenToClose, rawData, percentile), windowSize);
        ind["ClosingBias"] = VectorOps::Drop(QuantileTimeSeries(ClosingBias, rawData, percentile), windowSize);
        ind["ExtensionRatio"] = VectorOps::Drop(QuantileTimeSeries(ExtensionRatio, rawData, percentile), windowSize);
        ind["SMA"] = QuantileTimeSeries(SMA, rawData, percentile);
        ind["EMA"] = VectorOps::Drop(QuantileTimeSeries(EMA, rawData, percentile), windowSize);
        ind["RSI"] = QuantileTimeSeries(RSI, rawData, percentile);
        ind["VWAP"] = QuantileTimeSeries(VWAP, rawData, percentile);
        ind["OBV"] = QuantileTimeSeries(OBV, rawData, percentile);
        ind["ROC"] = QuantileTimeSeries(ROC, rawData, percentile);
        ind["MFI"] = QuantileTimeSeries(MFI, rawData, percentile);

        const string percentileString = Utilities::NumberToString(percentile, 2);
        outputQuantileIndicators[percentileString] = ind;
    }

    return outputQuantileIndicators;
}