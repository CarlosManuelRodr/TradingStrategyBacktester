import unittest
import os
from TradingStrategyBacktester.TradingStrategyBacktester import *


class TestBacktester(unittest.TestCase):
    def test_strategy_results(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)
        Evaluator.SetStrategyEvaluatorDataset(dataset)

        strategy_program: str = 'Indicator("EMA",stock,time) < Indicator("ClosePrice",stock,time)'
        strategy_results: list[bool] = Evaluator.RunStrategy(strategy_program, "AAPL")

        self.assertEqual(strategy_results[13], False)
        self.assertEqual(strategy_results[14], True)


if __name__ == '__main__':
    unittest.main()
