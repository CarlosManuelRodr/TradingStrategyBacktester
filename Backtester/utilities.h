#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
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

    /// <summary>
    /// Check if the vector v contains the key.
    /// </summary>
    template<typename T> bool VectorContainsQ(const std::vector<T>& v, const T& key)
    {
        return std::find(v.begin(), v.end(), key) != v.end();
    }

    /// <summary>
    /// Return the sum of all the elements of the vector.
    /// </summary>
    /// <typeparam name="T">Numeric type.</typeparam>
    template<typename T> T Total(const std::vector<T>& v)
    {
        const T init = static_cast<T>(0);
        return std::accumulate(v.begin(), v.end(), init);
    }

    /// <summary>
    /// Return the multiplication of all the elements of the vector.
    /// </summary>
    /// <typeparam name="T">Numeric type.</typeparam>
    template<typename T> T Multiply(const std::vector<T>& v)
    {
        const T init = static_cast<T>(1);
        return std::accumulate(v.begin(), v.end(), init, [](T n1, T n2) { return n1 * n2; });
    }
}