import unittest
import os
from TradingStrategyBacktester.TradingStrategyBacktester import *


class TestBacktester(unittest.TestCase):
    def test_strategy_results(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)
        Evaluator.SetStrategyEvaluatorDataset(dataset)

        strategy_program: str = 'Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time)'
        strategy_results: list[bool] = Evaluator.RunStrategy(strategy_program, "AAPL")

        self.assertEqual(strategy_results[13], False)
        self.assertEqual(strategy_results[14], True)

    def test_backtester(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)
        Evaluator.SetStrategyEvaluatorDataset(dataset)

        strategy_program: str = 'Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time)'
        self.assertTrue(Evaluator.ValidateStrategyProgram(strategy_program))

        strategy_results: list[bool] = Evaluator.RunStrategy(strategy_program, "AAPL")

        execution_data: list[ExecutionData] = Backtester.BacktestStoplossProfittake(strategy_results, "AAPL",
                                                                                    0.05, 0.05, 0.05, -1)
        returns: list[float] = Returns.SimpleReturns(execution_data, 0.05)

        self.assertGreater(abs(sum(returns)), 0)

    def test_backtester_ind_quantile(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)
        Evaluator.SetStrategyEvaluatorDataset(dataset)

        strategy_program: str = 'Indicator("EMA", stock, time) < IndQuantile("ClosePrice", "0.75",  stock, time)'
        self.assertTrue(Evaluator.ValidateStrategyProgram(strategy_program))

        strategy_results: list[bool] = Evaluator.RunStrategy(strategy_program, "AAPL")

        execution_data: list[ExecutionData] = Backtester.BacktestStoplossProfittake(strategy_results, "AAPL",
                                                                                    0.05, 0.05, 0.05, -1)
        returns: list[float] = Returns.SimpleReturns(execution_data, 0.05)

        self.assertGreater(abs(sum(returns)), 0)

    def test_parallel_backtester(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)
        Evaluator.SetStrategyEvaluatorDataset(dataset)

        strategy_program: str = 'Indicator("EMA", stock, time) < Indicator("ClosePrice", stock, time)'
        strategy_results: dict[str, list[bool]] = Evaluator.RunStrategyAllStocks(strategy_program)

        returns = []
        for stock in strategy_results:
            execution_data: list[ExecutionData] = Backtester.BacktestStoplossProfittake(strategy_results.get(stock),
                                                                                        stock, 0.05, 0.05, 0.05, -1)
            ret = sum(Returns.SimpleReturns(execution_data, 0.05))
            print("Stock: " + stock + ", Return: " + str(ret))
            returns.append(ret)

        self.assertGreater(abs(sum(returns)), 0)



if __name__ == '__main__':
    unittest.main()
