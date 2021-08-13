#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

/// <summary>
/// Collection of miscellaneous utility functions.
/// </summary>
namespace utilities
{
    /// <summary>
    /// Obtain the keys of a map (std::map, std::unordered_map, etc...)
    /// </summary>
    /// <typeparam name="myMap">Type of the map.</typeparam>
    /// <param name="m">Reference to the map.</param>
    /// <returns>The list of keys.</returns>
    template<typename myMap> std::vector<typename myMap::key_type> Keys(const myMap& m)
    {
        std::vector<typename myMap::key_type> r;
        r.reserve(m.size());
        for (const auto& kvp : m)
        {
            r.push_back(kvp.first);
        }
        return r;
    }

    /// <summary>
    /// Obtain the values of a map (std::map, std::unordered_map, etc...)
    /// </summary>
    /// <typeparam name="myMap">Type of the map.</typeparam>
    /// <param name="m">Reference to the map.</param>
    /// <returns>The list of values.</returns>
    template<typename myMap> std::vector<typename myMap::mapped_type> Values(const myMap& m)
    {
        std::vector<typename myMap::mapped_type> r;
        r.reserve(m.size());
        for (const auto& kvp : m)
        {
            r.push_back(kvp.second);
        }
        return r;
    }

    /// <summary>
    /// Utility function to convert a string to a numeric type.
    /// </summary>
    /// <typeparam name="T">Numeric type. E.g: int, double, etc</typeparam>
    /// <param name="str">Input string.</param>
    /// <returns>Converted type.</returns>
    template <typename T> T ConvertTo(const std::string& str)
    {
        std::istringstream ss(str);
        T num;
        ss >> num;
        return num;
    }

    /// <summary>
    /// Conversion of number type to string with fixed precision.
    /// </summary>
    /// <typeparam name="T">Numeric type.</typeparam>
    /// <param name="num">Number to convert.</param>
    /// <param name="significantDigits">Number of decimal significant digits.</param>
    /// <returns>The number as a std::string.</returns>
    template <typename T> std::string NumberToString(T num, int significantDigits)
    {
        std::ostringstream streamObj;
        streamObj << std::setprecision(significantDigits);
        streamObj << num;
        return streamObj.str();
    }

    // Source: https://stackoverflow.com/a/3418285
    inline void ReplaceString(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty())
            return;

        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    inline std::vector<std::string> StringSplit(const std::string& s, const std::string& delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }
}