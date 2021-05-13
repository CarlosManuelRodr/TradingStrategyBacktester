#include <fstream>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <digestpp.hpp>
#include "filesystem.h"
#include "dataset_loader.h"
#include "technical_indicators.h"
#include "functional_map.h"
using namespace std;

/****************************
*                           *
*      Dataset loading      *
*                           *
****************************/

vector<OCHLVData<double>> load_raw_dataset(const string path)
{
    ifstream inFile;
    inFile.open(path);

    bool readingHeaderLine = true;
    vector<string> datasetLines;
    if (inFile.is_open())
    {
        while (!inFile.eof())
        {
            string line;
            getline(inFile, line);

            if (!readingHeaderLine && !line.empty())
                datasetLines.push_back(line);
            else
                readingHeaderLine = false;
        }
    }
    else
        return vector<OCHLVData<double>>();

    vector<OCHLVData<double>> output;
    for (string line : datasetLines)
    {
        OCHLVData<double> dataLine = OCHLVData<double>(parse_csv_line<string>(line));
        if (dataLine.volume != 0 && dataLine.high != dataLine.low) // Check if the line is valid.
            output.push_back(OCHLVData<double>(parse_csv_line<string>(line)));
    }

    return output;
}

StockData load_stockdata_from_raw(vector<OCHLVData<double>> rawData)
{
    StockData stockData;
    stockData.indicators = CalculateIndicators(rawData);
    stockData.quantileIndicators = CalculateQuantileIndicators(rawData);
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
    if (file.is_open())
        return digestpp::sha256().absorb(file).hexdigest();
    else
        return "File not found";
}

StockData load_stockdata(const string path)
{
    StockData loadedStockData;

    // Check if the serialized data directory exists.
    string serializedDataDir = filename_join({ get_app_directory(), "StockData" });
    if (!directory_exist(serializedDataDir))
        create_directory("StockData");

    // Look-up for the checksum table.
    string checksumTablePath = filename_join({ get_app_directory(), "StockData", "ChecksumTable.json" });
    if (file_exist(checksumTablePath))
    {
        // If it exists, deserialize it.
        ChecksumTable checksumTable;
        ifstream is(checksumTablePath);
        cereal::JSONInputArchive iarchive(is);
        iarchive(checksumTable);
        is.close();

        // Validate checksum of file in path.
        if (checksumTable[path] == calculate_file_checksum(path))
        {
            // Deserialize dataset and return it.
            string serializedFilePath = filename_join({ get_app_directory(), "StockData", file_basename(path) + ".bin" });
            if (file_exist(serializedFilePath))
            {
                ifstream istock(serializedFilePath, ios::binary | ios::in);

                if (istock.is_open())
                {
                    cereal::BinaryInputArchive iarchive(istock);
                    iarchive(loadedStockData);
                    return loadedStockData;
                }
                else
                    throw runtime_error("Error opening serialized dataset with path: " + path);
            }
            else
                throw runtime_error("load_stockdata: cannot open serializedFilePath that is supossed to exist.");
        }
        else // Parse the new version of the file, serialize it and update the checksum.
        {
            // Parse dataset.
            vector<OCHLVData<double>> rawDataset = load_raw_dataset(path);
            loadedStockData = load_stockdata_from_raw(rawDataset);
            loadedStockData.dates = IndicatorTimeSeries(Date, rawDataset);

            // Serialize dataset.
            ofstream os(filename_join({ get_app_directory(), "StockData", file_basename(path) + ".bin" }), ios::binary | ios::out);
            cereal::BinaryOutputArchive oarchive(os);
            oarchive(loadedStockData);

            // Update checksum
            checksumTable[path] = calculate_file_checksum(path);
            ofstream checksumTableOS(filename_join({ get_app_directory(), "StockData", "ChecksumTable.json" }));
            cereal::JSONOutputArchive oChecksumTableArchive(checksumTableOS);
            oChecksumTableArchive(checksumTable);

            return loadedStockData;
        }
    }
    else // If no checksum table is found, parse the dataset and create one.
    {   
        // Parse dataset.
        vector<OCHLVData<double>> rawDataset = load_raw_dataset(path);
        loadedStockData = load_stockdata_from_raw(rawDataset);
        loadedStockData.dates = IndicatorTimeSeries(Date, rawDataset);

        // Serialize dataset.
        ofstream os(filename_join({ get_app_directory(), "StockData", file_basename(path) + ".bin" }), ios::binary | ios::out);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(loadedStockData);

        // Calculate checksum.
        ChecksumTable checksumTable;
        checksumTable[path] = calculate_file_checksum(path);

        // Serialize checksum table.
        ofstream checksumTableOS(filename_join({ get_app_directory(), "StockData", "ChecksumTable.json" }));
        cereal::JSONOutputArchive oChecksumTableArchive(checksumTableOS);
        oChecksumTableArchive(checksumTable);

        return loadedStockData;
    }
}
Dataset load_dataset(const string path)
{
    vector<string> datasetFiles = files_in_directory(path);
    vector<StockData> stockDataFromFiles = function_map<StockData, string>(load_stockdata, datasetFiles);

    Dataset dataset;
    for (unsigned i = 0; i < datasetFiles.size(); i++)
        dataset[file_basename(datasetFiles[i])] = stockDataFromFiles[i];

    return dataset;
}