///
/// @file: StringHelpers.hpp
/// @description: helper functions for working with std::strings
///
/// @date: 2026-02-02
/// @author: Moritz Pirer
///

#ifndef STRING_HELPERS_HPP
#define STRING_HELPERS_HPP

#include <string>
#include <vector>
#include <optional>

namespace StringHelpers {

    ///// FORMATTING

    /// @brief return a string of length total_width with str on the left, filled with pad_with. If str is 
    ///  longer than total_width, the first total_width chars of str will be chosen
    /// @param str the string to left-align
    /// @param total_width how wide the new string should be, including the padding 
    /// @param pad_with the char to fill the space with
    /// @return a string with str on the left 
    std::string leftAlign(const std::string& str, unsigned int total_width, char pad_with = ' ');

    /// @brief return a string of length total_width with str on the right, filled with pad_with. If str is
    ///     longer than total_width, str will be truncated
    /// @param str the string to right-align
    /// @param total_width how wide the new string should be, including the padding 
    /// @param pad_with the char to fill the space with
    /// @return a string with str on the right
    std::string rightAlign(const std::string& str, unsigned int total_width, char pad_with = ' ');

    /// @brief splits the given paragraph into a vector of chunks that are at
    ///     most as long as the text area is wide (i.e that don't need to wrap)
    /// @param paragraph the paragraph to split 
    /// @param start_column where in the paragraph to start (before that column is ignored) 
    /// @return the split vector   
    std::vector<std::string> splitIntoRows(const std::string& paragraph,
        int start_column, int max_length);

    /// @brief formats number as a string and inserts seperator every group_size digits, starting on the right, e.g.
    ///     '10 000' if group size is 3 and seperator is space
    /// @param number the number to format
    /// @param group_size how many digits may be grouped together before a seperator is inserted
    /// @param seperator the char that seperates groups
    /// @return the number formatted as a string
    std::string addSeperators(int number, int group_size, char seperator = ' ');

    /// @brief converts the string to all-uppercase in-place
    void uppercase(std::string& str);

    /// @brief converts the string to all-lowercase in-place
    void lowercase(std::string& str);

    ///// CONTENT TESTING

    /// @brief counts the number of words in the given string, with any amount of whitespace countin
    ///     as a delimiter
    /// @param str the string to count words in
    /// @return the number of words in the string
    int countWords(const std::string& str);

    bool caselessEquals(const std::string& a, const std::string& b);
    /// @brief returns the first character in str that is not a space (' '),
    ///     or std::nullopt if str doesn't contain a space
    std::optional<char> firstNonSpace(const std::string& str);

    /// @brief checks if the first character of str is first.
    /// @return true if str starts with first, false otherwise (empty string returns false)
    bool startsWith(const std::string& str, char first);

    /// @brief checks if the first non-space character of str is first.
    bool startsWithIgnoringWhitespace(const std::string& str, char first);

    /// @brief checks if start is a prefix for str when ignoring leading spaces
    bool startsWithIgnoringWhitespace(const std::string& str, const std::string& start);
    
    /// @brief checks if all characters of str are either whitespace or comparison
    bool consistsOnlyOfIgnoringWhitespace(const std::string& str, char comparison);


    bool consistsOnlyOf(const std::string& str, char comparison);

    /// @brief counts the number of spaces before the first non-space character or the end of the string
    size_t countLeadingSpaces(const std::string& str);

    /// @brief finds the first character of str that is not a digit 0-9 and returns its index, or std::string::npos if 
    ///     no such character exists
    size_t findFirstNonDigit(const std::string& str);

    
    bool isNumberLiteral(const std::string& str, std::optional<char> number_seperator = std::nullopt);
    bool isBinaryNumber(const std::string& str, std::optional<char> number_seperator = std::nullopt);
    bool isOctalNumber(const std::string& str, std::optional<char> number_seperator = std::nullopt);
    bool isHexNumber(const std::string& str, std::optional<char> number_seperator = std::nullopt);
    bool isDecimalNumber(const std::string& str, std::optional<char> number_seperator = std::nullopt);

}; //namespace StringHelpers

#endif //STRING_HELPERS_HPP
