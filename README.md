# TradingStrategyBacktester

This is a very fast trading strategy backtester written in C++. It supports strategies based on numerical comparisons of combinations of technical indicators, quantiles of technical indicators (Comparative Quantile Analysis), and simple logical operations.

Trading strategies are expressed as C++-like functions, for example:
```
IndQuantile("RSI", "0.25", stock, time) <= IndQuantile("MFI", "0.05", stock, time) || Indicator("TypicalPrice", stock, time) > Indicator("ClosePrice", stock, time)
```

The program expects a dataset contained in a directory where multiple data files belonging to different stocks are stored in a `.csv` file with the following format:

![DataFormat](https://github.com/CarlosManuelRodr/TradingStrategyBacktester/raw/main/Img/DataFormat.png)

The program returns a list of return values for the specified trading strategy.

![ExampleImage](https://github.com/CarlosManuelRodr/TradingStrategyBacktester/raw/main/Img/Example.png)

## Features
- Fast execution of strategies: Each strategy is compiled to low-level executable code, which in turn is executed in parallel for every stock of the dataset.
- Fast loading: On the first execution, all the possible values of technical indicators for each stock will be calculated and serialized for faster loading on the next call.
- Multiple execution models.
- Mathematica Mathlink support.

### Available functions
At this moment, it provides access to technical indicators using the `Indicator("indName", stock, time)` function. To compare current values with past values of the indicator you can use the `IndQuantile("indName", percentile, stock, time)` function.

### Technical indicators
The following indicators are currently supported:
```
OpenPrice, ClosePrice, HighPrice, LowPrice, TradingVolume, WeightedClose, TypicalPrice, MedianPrice, PricePercentangeChangeOpenToClose, ClosingBias, ExtensionRatio, SMA, EMA, RSI, VWAP, OBV, ROC, MFI.
```

## Instructions
```
INSTRUCTIONS: Backtester [options]

  -p <dataset_path>,    --path=<dataset_path>          Path to dataset.
  -s <strategy>,        --strategies=<strategy>        Trading strategy as a C++ string.
  -e <execution_model>, --execution=<execution_model>  The trading execution model. Possible values are: StopLossProfitTake, TimestopHit and MarketTiming.
  -t <value>,           --transactioncost=<value>      The transaction cost as percentage. The default value is 0.0025.
  -r <return_type>,     --returntype=<return_type>     Specifies how to calculate the returns. Possible values are: SimpleReturns, LogReturns, and DivReturns.
  -k <value>,           --profittake=<value>           Profit Take value used if the execution model is set to StopLossProfitTake. The default value is 0.3.
  -l <value>,           --stoploss=<value>             Stop Loss value used if the execution model is set to StopLossProfitTake. The default value is -0.3.
  -d <value>,           --timeperiod=<value>           Time Period value used if the execution model is set to TimestopHit. The default value is 100.
                        --help                         Show instructions.
```

### Examples
Example command for Linux:
```
./Backtester --path="../TestDataset" --strategy="IndQuantile(\"RSI\",\"0.25\",stock,time)<=IndQuantile(\"MFI\",\"0.05\ 
\",stock,time)" --execution="StopLossProfitTake"
```

Example command for Windows:
```
.\Backtester.exe --path="..\\TestDataset\\" --strategy="Indicator(\"TypicalPrice\",stock,time)>IndQuantile(\"TypicalPrice\",\"0.25\",stock,time)" --execution="StopLossProfitTake"
```

## Building

On Linux-based system just use the `Build.sh` script. All dependences are included in the repo. If you wish to build the Mathlink version, use the `MathlinkBuild.sh` script.

On Windows you can compile by using the Visual Studio solution. If you want to compile the Mathlink version you may need to modify the installation path of Mathematica on the build directories of the project:

![ExampleImage](https://github.com/CarlosManuelRodr/TradingStrategyBacktester/raw/main/Img/WindowsBuildConfigure.png)

## Credits
- [Angelscript](https://www.angelcode.com/angelscript/) Is the scripting backend where each strategy runs.
- [The Lean Mean C++ Option Parser](http://optionparser.sourceforge.net/) Is used for the CLI version.
- [ThreadPool](https://github.com/progschj/ThreadPool) Is used as safe parallelization method.
- [Cereal](https://uscilab.github.io/cereal/) for serialization of the dataset.