#include <doctest.h>
#include "../include/filesystem.h"
using namespace std;

TEST_CASE("Test app directory")
{
    string aaplStockPath = FileSystem::FilenameJoin({ "../dataset", "AAPL.csv" });
    string datasetDirectoryPath = FileSystem::FileDirectory(aaplStockPath);

    CHECK((datasetDirectoryPath == "../dataset"));
}