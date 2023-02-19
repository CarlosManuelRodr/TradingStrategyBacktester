#include <fstream>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <digestpp.hpp>
#include "filesystem.h"
#include "loader.h"
#include "indicators.h"
#include "utilities.h"
using namespace std;
using namespace backtester;

vector<OCHLVData> Loader::LoadRawData(const string& path)
{
    ifstream inFile;
    inFile.open(path);

    bool readingHeaderLine = true;
    vector<string> datasetLines;

    if (!inFile.is_open())
        throw std::runtime_error("Unable to open file.");

    while (!inFile.eof())
    {
        string line;
        getline(inFile, line);

        if (!readingHeaderLine && !line.empty())
            datasetLines.push_back(line);
        else
            readingHeaderLine = false;
    }

    vector<OCHLVData> output;
    for (const string& line : datasetLines)
    {
        OCHLVData dataLine = OCHLVData(Utilities::ParseCsvLine<string>(line));
        if (dataLine.volume != 0 && dataLine.high != dataLine.low) // Check if the line is valid.
            output.emplace_back(Utilities::ParseCsvLine<string>(line));
    }

    return output;
}

StockData Loader::LoadStockdataFromRaw(const vector<OCHLVData>& rawData)
{
    StockData stockData;
    stockData.indicators = Indicator::CalculateIndicators(rawData);
    stockData.quantileIndicators = Indicator::CalculateQuantileIndicators(rawData);
    return stockData;
}

/// <summary>
/// The checksum table is a hashmap that maps the path of a file to its sha256 checksum.
/// It is used to re-serialize in the event of a change in the dataset file.
/// </summary>
using ChecksumTable = map<string, string>;

string calculate_file_checksum(const string& path)
{
    ifstream file(path, ios_base::in | ios_base::binary);
    return file.is_open() ? digestpp::sha256().absorb(file).hexdigest() : "File not found";
}

StockData Loader::LoadStockdata(const string& path)
{
    StockData loadedStockData;
    const string datasetDirectory = FileSystem::FileDirectory(path);

    // Check if the serialized data directory exists.
    string serializedDataDir = FileSystem::FilenameJoin({ datasetDirectory, cacheDirectoryName });
    if (!FileSystem::DirectoryExist(serializedDataDir))
        FileSystem::CreateDirectory(serializedDataDir);

    // Look-up for the checksum table.
    string checksumTablePath = FileSystem::FilenameJoin({ serializedDataDir, "ChecksumTable.json" });
    if (FileSystem::FileExist(checksumTablePath))
    {
        // If it exists, deserialize it.
        ChecksumTable checksumTable;
        ifstream is(checksumTablePath);
        cereal::JSONInputArchive jsonInputArchive(is);
        jsonInputArchive(checksumTable);
        is.close();

        // Validate checksum of file in path.
        if (checksumTable[path] == calculate_file_checksum(path))
        {
            // Deserialize dataset and return it.
            string serializedFilePath = FileSystem::FilenameJoin({ serializedDataDir, FileSystem::FileBasename(path) + ".bin" });
            if (FileSystem::FileExist(serializedFilePath))
            {
                ifstream istock(serializedFilePath, ios::binary | ios::in);

                if (istock.is_open())
                {
                    cereal::BinaryInputArchive binaryInputArchive(istock);
                    binaryInputArchive(loadedStockData);
                    return loadedStockData;
                }
                else
                    throw runtime_error("Error opening serialized dataset with path: " + path + ".");
            }
            else
                throw runtime_error("Cannot open serializedFilePath.");
        }
        else // Parse the new version of the file, serialize it and update the checksum.
        {
            // Parse dataset.
            vector<OCHLVData> rawDataset = LoadRawData(path);
            loadedStockData = LoadStockdataFromRaw(rawDataset);
            loadedStockData.dates = VectorOps::Drop(Indicator::IndicatorTimeSeries(Indicator::Date, rawDataset), 2 * windowSize);

            // Serialize dataset.
            ofstream os(FileSystem::FilenameJoin({ serializedDataDir, FileSystem::FileBasename(path) + ".bin" }), ios::binary | ios::out);
            cereal::BinaryOutputArchive oarchive(os);
            oarchive(loadedStockData);

            // Update checksum
            checksumTable[path] = calculate_file_checksum(path);
            ofstream checksumTableOS(FileSystem::FilenameJoin({ serializedDataDir, "ChecksumTable.json" }));
            cereal::JSONOutputArchive oChecksumTableArchive(checksumTableOS);
            oChecksumTableArchive(checksumTable);

            return loadedStockData;
        }
    }
    else // If no checksum table is found, parse the dataset and create one.
    {   
        // Parse dataset.
        vector<OCHLVData> rawDataset = LoadRawData(path);
        loadedStockData = LoadStockdataFromRaw(rawDataset);
        loadedStockData.dates = VectorOps::Drop(Indicator::IndicatorTimeSeries(Indicator::Date, rawDataset), 2 * windowSize);

        // Serialize dataset.
        ofstream os(FileSystem::FilenameJoin({ serializedDataDir, FileSystem::FileBasename(path) + ".bin" }), ios::binary | ios::out);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(loadedStockData);

        // Calculate checksum.
        ChecksumTable checksumTable;
        checksumTable[path] = calculate_file_checksum(path);

        // Serialize checksum table.
        ofstream checksumTableOS(FileSystem::FilenameJoin({ serializedDataDir, "ChecksumTable.json" }));
        cereal::JSONOutputArchive oChecksumTableArchive(checksumTableOS);
        oChecksumTableArchive(checksumTable);

        return loadedStockData;
    }
}

Dataset Loader::LoadDataset(const string& path)
{
    vector<string> datasetFiles = FileSystem::FilesInDirectory(path);
    vector<StockData> stockDataFromFiles = Utilities::map<StockData, string>(LoadStockdata, datasetFiles);

    Dataset dataset;
    for (unsigned i = 0; i < datasetFiles.size(); i++)
        dataset[FileSystem::FileBasename(datasetFiles[i])] = stockDataFromFiles[i];

    return dataset;
}

void Loader::ClearCache(const string& path)
{
    string serializedDataDir = FileSystem::FilenameJoin({ path, cacheDirectoryName });
    if (FileSystem::DirectoryExist(serializedDataDir))
        FileSystem::Delete(serializedDataDir);
}
