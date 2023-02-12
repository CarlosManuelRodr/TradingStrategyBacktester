#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/loader.h"
#include "../include/backtester.h"
namespace py = pybind11;
using namespace std;

//*****************************
//*         Bindings          *
//****************************/

PYBIND11_MODULE(TradingStrategyBacktester, m) {
    m.doc() = "TradingStrategyBacktester Python bindings."; // optional module docstring

    py::class_<OCHLVData>(m, "OCHLVData")
            .def(py::init<>())
            .def(py::init<const std::string&, double, double, double, double, double>(),
                 py::arg("date"), py::arg("open"), py::arg("close"), py::arg("high"),
                 py::arg("low"), py::arg("volume"))

            .def(py::init<std::vector<std::string>>(),
                 py::arg("csvLine"))

            .def_readwrite("date", &OCHLVData::date)
            .def_readwrite("open", &OCHLVData::open)
            .def_readwrite("close", &OCHLVData::close)
            .def_readwrite("high", &OCHLVData::high)
            .def_readwrite("low", &OCHLVData::low)
            .def_readwrite("volume", &OCHLVData::volume)
            ;

    py::class_<StockData>(m, "StockData")
            .def(py::init<>())

            .def(py::init<const std::vector<std::string>&, const Indicators&, const QuantileIndicators&>(),
                 py::arg("dates"), py::arg("indicators"), py::arg("quantileIndicators"))

            .def_readwrite("dates", &StockData::dates)
            .def_readwrite("indicators", &StockData::indicators)
            .def_readwrite("quantileIndicators", &StockData::quantileIndicators)
            ;

    py::class_<Loader>(m, "Loader")
            .def_readonly_static("cacheDirectoryName",
                                 &Loader::cacheDirectoryName,
                                 "Defines the serialized data directory name.")

            .def_static("LoadRawData",
                        &Loader::LoadRawData,
                        "Load OCHLVData from csv file.",
                        py::arg("path"))

            .def_static("LoadStockdataFromRaw",
                        &Loader::LoadStockdataFromRaw,
                        "Load StockData by calculating all the technical indicators from the raw OCHLVData.",
                        py::arg("rawData"))

            .def_static("LoadStockdata",
                        &Loader::LoadStockdata,
                        "Load StockData from csv file.",
                        py::arg("path"))

            .def_static("LoadDataset",
                        &Loader::LoadDataset,
                        "Load a dataset of csv files located in the path.",
                        py::arg("path"))

            .def_static("ClearCache",
                        &Loader::ClearCache,
                        "Clear the temporary serialized data directory.",
                        py::arg("path"))
            ;

    py::class_<Evaluator>(m, "Evaluator")
            .def_static("SetStrategyEvaluatorDataset",
                        &Evaluator::SetStrategyEvaluatorDataset,
                        "Configure the dataset to be used by the strategy runner.",
                        py::arg("dataset"))

            .def_static("GetStrategyEvaluatorDataset",
                        &Evaluator::GetStrategyEvaluatorDataset,
                        "Returns the pointer to the dataset used in the strategy runner.")

            .def_static("GetStocksInDataset",
                        &Evaluator::GetStocksInDataset,
                        "Returns the list of stocks present in the strategy evaluator dataset.")

            .def_static("ValidateStrategyProgram",
                        &Evaluator::ValidateStrategyProgram,
                        "Try to compile the strategy program and report compilation errors if any.",
                        py::arg("strategyProgram"))

            .def_static("RunStrategy",
                        &Evaluator::RunStrategy,
                        "Runs the strategy program in each date available of the stock.",
                        py::arg("strategyProgram"),py::arg("stock"))

            .def_static("Date",
                        &Evaluator::Date,
                        "Date accessor.",
                        py::arg("stock"), py::arg("time"))

            .def_static("Dates",
                        &Evaluator::Dates,
                        "Dates accessor.",
                        py::arg("stock"))

            .def_static("Indicator",
                        &Evaluator::Indicator,
                        "Indicator accessor.",
                        py::arg("indicatorName"), py::arg("stock"), py::arg("time"))

            .def_static("IndQuantile",
                        &Evaluator::IndQuantile,
                        "IndQuantile accessor.",
                        py::arg("indicatorName"), py::arg("percentile"), py::arg("stock"), py::arg("time"))

            .def_static("IndicatorTimeSeries",
                        &Evaluator::IndicatorTimeSeries,
                        "IndicatorTimeSeries accessor.",
                        py::arg("indicatorName"), py::arg("stock"))

            .def_static("IndQuantileTimeSeries",
                        &Evaluator::IndQuantileTimeSeries,
                        "IndQuantileTimeSeries accessor.",
                        py::arg("indicatorName"), py::arg("percentile"), py::arg("stock"))
            ;

    py::class_<Backtester>(m, "Backtester")
            .def_static("BacktestStoplossProfittake",
                        &Backtester::BacktestStoplossProfittake,
                        "Backtester where the exit strategy is set in terms of profit taking and stop loss parameters.",
                        py::arg("strategySignals"), py::arg("stock"), py::arg("profitTake"),
                        py::arg("stopLoss"), py::arg("transactionCost"), py::arg("minibatchSize"))

            .def_static("BacktestTimestopHit",
                        &Backtester::BacktestTimestopHit,
                        "Backtester where the exit strategy is set in terms of a time period to hold onto the asset.",
                        py::arg("strategySignals"), py::arg("stock"), py::arg("timePeriod"),
                        py::arg("minibatchSize"))

            .def_static("BacktestMarketTiming",
                        &Backtester::BacktestMarketTiming,
                        "Backtester where the exit strategy is given by the strategy signals.",
                        py::arg("strategySignals"), py::arg("stock"), py::arg("minibatchSize"))
            ;
}