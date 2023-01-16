#include <doctest.h>
#include "../include/loader.h"
#include "../include/filesystem.h"
using namespace std;


TEST_CASE("Test dataset loading")
{
    string aaplStockPath = FileSystem::FilenameJoin({ "../dataset", "AAPL.csv" });
    vector<OCHLVData> rawDataset = Loader::LoadRawDataset(aaplStockPath);

    CHECK((rawDataset.at(0).volume == 1345674400.0));
    CHECK((rawDataset.at(1).date == "\"2008-02-01\""));
}