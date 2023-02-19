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
        stock_data: StockData = Loader.LoadStockdata(path)

        self.assertEqual(stock_data.indicators["TradingVolume"][0], 789905200.0)
        self.assertEqual(stock_data.dates[0], "\"2008-05-27\"")

    def test_dataset_loading(self):
        path = os.path.join(os.getcwd(), "..", "..", "dataset")
        dataset: dict[str, StockData] = Loader.LoadDataset(path)

        self.assertEqual(dataset.get("AAPL").indicators["TradingVolume"][0], 789905200.0)
        self.assertEqual(dataset.get("AAPL").dates[0], "\"2008-05-27\"")

    def test_serialization_integrity(self):
        dataset_path = os.path.join(os.getcwd(), "..", "..", "dataset")
        cache_path = os.path.join(os.getcwd(), "..", "..", "dataset", Loader.cacheDirectoryName)

        # Delete cache.
        Loader.ClearCache(dataset_path)
        self.assertFalse(os.path.exists(cache_path))

        # Load dataset and cache it.
        dataset1: dict[str, StockData] = Loader.LoadDataset(dataset_path)
        self.assertTrue(os.path.exists(cache_path))

        # Load cached dataset.
        dataset2: dict[str, StockData] = Loader.LoadDataset(dataset_path)
        self.assertEqual(dataset1, dataset2)


if __name__ == '__main__':
    unittest.main()
