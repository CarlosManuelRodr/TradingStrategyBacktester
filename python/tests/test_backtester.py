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
        strategy_results: list[bool] = Evaluator.RunStrategy(strategy_program, "AAPL")

        # noinspection PyTypeChecker
        execution_data = Backtester.BacktestStoplossProfittake(strategy_results, "AAPL", 0.05, 0.05, 0.05, -1)
        returns: list[float] = Returns.SimpleReturns(execution_data, 0.05)

        self.assertGreater(abs(sum(returns)), 0)


if __name__ == '__main__':
    unittest.main()
