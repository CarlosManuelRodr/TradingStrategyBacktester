import unittest
import os

from TradingStrategyBacktester.TradingStrategyBacktester import *


class TestLoader(unittest.TestCase):
    def test_raw_dataset_loader(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset", "AAPL.csv")
        raw_dataset: list[OCHLVData] = Loader.LoadRawData(path)

        self.assertEqual(raw_dataset[0].volume, 1345674400.0)
        self.assertEqual(raw_dataset[1].date, "\"2008-02-01\"")

    def test_stock_data_loader(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset", "AAPL.csv")
        print(path)
        stock_data: StockData = Loader.LoadStockdata(path)

        self.assertEqual(stock_data.indicators["TradingVolume"][0], 789905200.0)



if __name__ == '__main__':
    unittest.main()
