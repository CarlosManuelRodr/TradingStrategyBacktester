/****************************
*                           *
*    Dataset Management     *
*                           *
****************************/

:Begin:
:Function:       load_dataset_from_directory
:Pattern:        BTLoadDataset[path_String]
:Arguments:      { path }
:ArgumentTypes:  { String }
:ReturnType:     Manual
:End:

:Begin:
:Function:       unload_dataset
:Pattern:        BTUnloadDataset[]
:Arguments:      Manual
:ArgumentTypes:  Manual
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_number_of_loaded_stocks
:Pattern:        BTGetNumberOfLoadedStocks[]
:Arguments:      Manual
:ArgumentTypes:  Manual
:ReturnType:     Integer
:End:

:Begin:
:Function:       get_stock_names_of_dataset
:Pattern:        BTGetStockNamesOfDataset[]
:Arguments:      Manual
:ArgumentTypes:  Manual
:ReturnType:     Manual
:End:

/****************************
*                           *
*     Dataset accesors      *
*                           *
****************************/

:Begin:
:Function:       get_date
:Pattern:        BTGetDate[stock_String, time_Integer]
:Arguments:      { stock, time }
:ArgumentTypes:  { String, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_dates_list
:Pattern:        BTGetDates[stock_String]
:Arguments:      { stock }
:ArgumentTypes:  { String }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_indicator
:Pattern:        BTGetIndicator[indicator_String, stock_String, time_Integer]
:Arguments:      { indicator, stock, time }
:ArgumentTypes:  { String, String, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_indicator_timeseries
:Pattern:        BTGetIndicatorTimeSeries[indicator_String, stock_String]
:Arguments:      { indicator, stock }
:ArgumentTypes:  { String, String }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_quantile_indicator
:Pattern:        BTGetQuantileIndicator[indicator_String, percentile_String, stock_String, time_Integer]
:Arguments:      { indicator, percentile, stock, time }
:ArgumentTypes:  { String, String, String, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_quantile_indicator_timeseries
:Pattern:        BTGetQuantileIndicatorTimeSeries[indicator_String, percentile_String, stock_String]
:Arguments:      { indicator, percentile, stock }
:ArgumentTypes:  { String, String, String }
:ReturnType:     Manual
:End:

/****************************
*                           *
*      Backtesting API      *
*                           *
****************************/

:Begin:
:Function:       get_strategy_values
:Pattern:        BTGetStrategyValues[strategy_String, stock_String]
:Arguments:      { strategy, stock }
:ArgumentTypes:  { String, String }
:ReturnType:     Manual
:End:

/*************************************
*    get_strategy_execution_data     *
*************************************/

:Begin:
:Function:       get_strategy_execution_data_stoploss_profittake
:Pattern:        BTGetStrategyExecutionDataStoplossProfitTake[strategy_String, stock_String, profitTake_Real, stopLoss_Real, transactionCost_Real]
:Arguments:      { strategy, stock, profitTake, stopLoss, transactionCost }
:ArgumentTypes:  { String, String, Real, Real, Real }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_strategy_execution_data_timestop_hit
:Pattern:        BTGetStrategyExecutionDataTimestopHit[strategy_String, stock_String, timePeriod_Integer, transactionCost_Real]
:Arguments:      { strategy, stock, timePeriod, transactionCost }
:ArgumentTypes:  { String, String, Integer, Real }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_strategy_execution_data_markettiming
:Pattern:        BTGetStrategyExecutionDataMarketTiming[strategy_String, stock_String, transactionCost_Real]
:Arguments:      { strategy, stock, transactionCost }
:ArgumentTypes:  { String, String, Real }
:ReturnType:     Manual
:End:

/*********************
*    get_returns     *
*********************/

:Begin:
:Function:       get_returns_stoploss_profittake
:Pattern:        BTGetStrategyReturnsStoplossProfitTake[returnType_String, strategy_String, stock_String, profitTake_Real, stopLoss_Real, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, stock, profitTake, stopLoss, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, String, Real, Real, Real, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_returns_timestop_hit
:Pattern:        BTGetStrategyReturnsTimestopHit[returnType_String, strategy_String, stock_String, timePeriod_Integer, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, stock, profitTake, stopLoss, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, String, Integer, Real, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_returns_markettiming
:Pattern:        BTGetStrategyReturnsMarketTiming[returnType_String, strategy_String, stock_String, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, stock, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, String, Real, Integer }
:ReturnType:     Manual
:End:

/**********************************
*    get_returns_for_all_stocks   *
**********************************/

:Begin:
:Function:       get_returns_for_all_stocks_stoploss_profittake
:Pattern:        BTGetStrategyReturnsAllStocksStoplossProfitTake[returnType_String, strategy_String, profitTake_Real, stopLoss_Real, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, profitTake, stopLoss, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, Real, Real, Real, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_returns_for_all_stocks_timestop_hit
:Pattern:        BTGetStrategyReturnsAllStocksTimestopHit[returnType_String, strategy_String, timePeriod_Integer, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, timePeriod, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, Integer, Real, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_returns_for_all_stocks_markettiming
:Pattern:        BTGetStrategyReturnsAllStocksMarketTiming[returnType_String, strategy_String, transactionCost_Real, minibatchSize_Integer: -1]
:Arguments:      { returnType, strategy, transactionCost, minibatchSize }
:ArgumentTypes:  { String, String, Real, Integer }
:ReturnType:     Manual
:End:

/****************************
*                           *
*          Testing          *
*                           *
****************************/

:Begin:
:Function:       strategy_compilation_info
:Pattern:        BTStrategyCompilationInfo[strategy_String]
:Arguments:      { strategy }
:ArgumentTypes:  { String }
:ReturnType:     Manual
:End: