#include <doctest.h>
#include "../include/loader.h"
#include "../include/filesystem.h"
using namespace std;
using namespace backtester;

TEST_CASE("Test raw dataset loading")
{
    string aaplStockPath = FileSystem::FilenameJoin({ "../dataset", "AAPL.csv" });
    vector<OCHLVData> rawDataset = Loader::LoadRawData(aaplStockPath);

    CHECK((rawDataset.at(0).volume == 1345674400.0));
    CHECK((rawDataset.at(1).date == "\"2008-02-01\""));
}

TEST_CASE("Test stock data loading")
{
    string aaplStockPath = FileSystem::FilenameJoin({ "../dataset", "AAPL.csv" });
    StockData stockData = Loader::LoadStockdata(aaplStockPath);

    CHECK((stockData.indicators.at("TradingVolume").at(0) == 789905200.0));
    CHECK((stockData.dates.at(0) == "\"2008-05-27\""));
}

TEST_CASE("Test dataset loading")
{
    Dataset dataset = Loader::LoadDataset("../dataset");

    CHECK((dataset.at("AAPL").indicators.at("TradingVolume").at(0) == 789905200.0));
    CHECK((dataset.at("AAPL").dates.at(0) == "\"2008-05-27\""));
}

TEST_CASE("Test serialization integrity")
{
    string cachePath = FileSystem::FilenameJoin({ "../dataset", Loader::cacheDirectoryName });
    Loader::ClearCache("../dataset");
    CHECK((FileSystem::DirectoryExist(cachePath) == false));

    // Load dataset and cache it.
    Dataset dataset1 = Loader::LoadDataset("../dataset");
    CHECK((FileSystem::DirectoryExist(cachePath) == true));

    // Load cached dataset.
    Dataset dataset2 = Loader::LoadDataset("../dataset");

    CHECK((dataset1 == dataset2));
}