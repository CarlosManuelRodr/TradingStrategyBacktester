#pragma once
#include <vector>
#include <string>
#include "dataset.h"

//*****************************
//*      Dataset loading      *
//****************************/

class Loader {
public:

    /**
     * Load OCHLVData from csv file. The expected format is a csv with the first row
     * with the following attributes: "date","open","high","low","close","volume".
     * @param path Absolute path to csv file.
     * @return Vector of OCHLVData entries.
     */
    static std::vector<OCHLVData> LoadRawData(const std::string &path);

    /**
     * Load StockData by calculating all the technical indicators from the raw OCHLVData.
     * @param rawData List of OCHLVData.
     * @return A StockData structure with all the calculated technical indicators.
     */
    static StockData LoadStockdataFromRaw(const std::vector<OCHLVData> &rawData);

    /**
     * Load StockData from csv file. Upon loading, it serializes the generated object for reuse
     * and fast loading in the next program execution.
     * @param path Absolute path to csv file.
     * @return A StockData structure with all the calculated technical indicators.
     */
    static StockData LoadStockdata(const std::string &path);

    /**
     * Load a dataset of csv files located in the path.
     * @param path Absolute path to directory containing the dataset as csv files.
     * @return A Dataset object.
     */
    static Dataset LoadDataset(const std::string &path);

};