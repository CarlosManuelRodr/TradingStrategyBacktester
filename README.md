# TradingStrategyBacktester

## Instructions
```
INSTRUCTIONS: Backtester [options]

  -p <dataset_path>,    --path=<dataset_path>          
                    Path to dataset.
  -s <strategy>,        --strategies=<strategy>        
                    Trading strategy as a C++ string.
  -e <execution_model>, --execution=<execution_model>  
                    The trading execution model. Possible values are:
                    StopLossProfitTake, TimestopHit and MarketTiming.
  -t <value>,           --transactioncost=<value>      
                    The transaction cost as percentage. The default value is
                    0.0025.
  -r <return_type>,     --returntype=<return_type>     
                    Specifies how to calculate the returns. Possible values are:
                    SimpleReturns, LogReturns, and DivReturns.
  -k <value>,           --profittake=<value>           
                    Profit Take value used if the execution model is set to
                    StopLossProfitTake. The default value es 0.3.
  -l <value>,           --stoploss=<value>             
                    Stop Loss value used if the execution model is set to
                    StopLossProfitTake. The default value es -0.3.
  -d <value>,           --timeperiod=<value>           
                    Time Period value used if the execution model is set to
                    TimestopHit. The default value es 100.
                        --help                         
                    Show instructions.
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
