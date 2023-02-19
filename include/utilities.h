#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>
#include "vector_ops.h"

namespace backtester
{
    /** Collection of miscellaneous utility functions. */
    class Utilities
    {
    public:

        /**
         * Obtain the keys of a map (std::map, std::unordered_map, etc...)
         * @tparam myMap Type of the map.
         * @param m Reference to the map.
         * @return The list of keys.
         */
        template<typename myMap> static std::vector<typename myMap::key_type> Keys(const myMap& m)
        {
            std::vector<typename myMap::key_type> r;
            r.reserve(m.size());

            for (const auto& kvp : m)
                r.push_back(kvp.first);

            return r;
        }

        /**
         * Obtain the keys of a map (std::map, std::unordered_map, etc...)
         * @tparam myMap Type of the map.
         * @param m Reference to the map.
         * @return The list of keys.
         */
        template<typename myMap> static std::vector<typename myMap::mapped_type> Values(const myMap& m)
        {
            std::vector<typename myMap::mapped_type> r;
            r.reserve(m.size());
            for (const auto& kvp : m)
                r.push_back(kvp.second);

            return r;
        }

        /**
         * Utility function to convert a string to a numeric type.
         * @tparam T Numeric type. E.g: int, double, etc
         * @param str Input string.
         * @return Converted type.
         */
        template <typename T> static T ConvertTo(const std::string& str)
        {
            std::istringstream ss(str);
            T num;
            ss >> num;
            return num;
        }

        /**
         * Conversion of number type to string with fixed precision.
         * @tparam T The numeric type.
         * @param num Number to convert.
         * @param significantDigits Number of decimal significant digits.
         * @return The number as a std::string.
         */
        template <typename T> static std::string NumberToString(T num, int significantDigits)
        {
            std::ostringstream streamObj;
            streamObj << std::setprecision(significantDigits);
            streamObj << num;
            return streamObj.str();
        }

        /**
         * Replace all occurrences of a string.
         * @param str The input string.
         * @param from The sequence to be replaced.
         * @param to The replace sequence.
         * @see https://stackoverflow.com/a/3418285
         */
        inline static void ReplaceString(std::string& str, const std::string& from, const std::string& to)
        {
            if (from.empty())
                return;

            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'.
            }
        }

        /**
         * Split a string by a delimiter.
         * @param s The string.
         * @param delimiter The delimiter.
         * @return A vector of the split strings.
         */
        inline static std::vector<std::string> StringSplit(const std::string& s, const std::string& delimiter)
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

        /**
         * Parse csv line to a vector.
         * @tparam T Cast type of the line element.
         * @param in Csv formatted line.
         * @param ignoreNested If false, it won't separate by commas in nested structures.
         * @return A list with the parsed elements.
         */
        template<typename T> static std::vector<T> ParseCsvLine(const std::string& in, const bool ignoreNested = true)
        {
            const std::string& input = in;
            const std::vector<char> beginNestSymbols = { '(', '[' };
            const std::vector<char> endNestSymbols = { ')', ']' };
            int nestLevel = 0;

            std::vector<T> out;

            size_t startPos = 0;

            for (size_t i = 0; i < input.length(); i++)
            {
                char currentCharacter = input.at(i);
                if (!ignoreNested && VectorOps::VectorContainsQ(beginNestSymbols, currentCharacter))
                    nestLevel++;

                if (!ignoreNested && VectorOps::VectorContainsQ(endNestSymbols, currentCharacter))
                    nestLevel--;

                if (nestLevel == 0 && input.at(i) == ',')
                {
                    out.push_back(ConvertTo<T>(input.substr(startPos, i - startPos)));
                    startPos = i + 1;
                }

                if (i + 1 == input.length())
                {
                    T converted = (input[in.length() - 1] == '\n' || input[in.length() - 1] == '\r') ?
                                  ConvertTo<T>(input.substr(startPos, input.length() - startPos - 1)) :
                                  ConvertTo<T>(input.substr(startPos, input.length() - startPos));
                    out.push_back(converted);
                }
            }
            return out;
        }

        /**
         * Simple functional mapping as a shortcut for applying a function to all the elements of a vector.
         * @tparam R The return type.
         * @tparam T The parameter type.
         * @param f The function pointer.
         * @param arg The vector where the function will be applied.
         * @return The mapped list.
         */
        template<typename R, typename T> static std::vector<R> map(std::function<R(T)> f, std::vector<T> arg)
        {
            std::vector<R> result_values;
            for (unsigned i = 0; i < arg.size(); i++)
                result_values.push_back(f(arg[i]));

            return result_values;
        }
    };
}