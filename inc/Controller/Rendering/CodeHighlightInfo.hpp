///
/// @file: CodeHighlightInfo.hpp
/// @description: description
///
/// @date: 2026-03-24
/// @author: Moritz Pirer
///

#ifndef CODE_HIGHLIGHT_INFO_HPP
#define CODE_HIGHLIGHT_INFO_HPP

#include <vector>
#include <string>
#include <unordered_set>

struct CodeHighlightInfo {
    std::unordered_set<std::string> m_language_names = std::unordered_set<std::string>();
    std::vector<std::string> m_file_extensions;
    std::unordered_set<std::string> m_language_keywords = std::unordered_set<std::string>();
    std::unordered_set<std::string> m_language_builtins = std::unordered_set<std::string>();

    std::vector<std::pair<std::string, std::string>> m_string_indicators = std::vector<std::pair<std::string, std::string>>();

    std::string m_token_splitters = "";
    std::vector<std::string> m_token_groups = std::vector<std::string>();

    std::unordered_set<std::string> m_rest_of_line_comments = std::unordered_set<std::string>();
    std::vector<std::pair<std::string, std::string>> m_ranged_comments = std::vector<std::pair<std::string, std::string>>();

    std::optional<char> m_number_seperator = std::nullopt;

    CodeHighlightInfo() = default;
    CodeHighlightInfo(const CodeHighlightInfo&) = default;
    ~CodeHighlightInfo() = default;

    static CodeHighlightInfo testPython();

    std::string debug() {
        return std::to_string(m_language_names.size()) + " names\n"
            +   std::to_string(m_file_extensions.size()) + " extensions\n"
            +   std::to_string(m_language_keywords.size()) + " keywords\n"
            +   std::to_string(m_language_builtins.size()) + " builtins\n"
            +   std::to_string(m_string_indicators.size()) + " string indicators\n"
            +   std::to_string(m_token_splitters.size()) + " splitters\n"
            +   std::to_string(m_token_groups.size()) + " groups\n"
            +   std::to_string(m_rest_of_line_comments.size()) + " rol comments\n"
            +   std::to_string(m_ranged_comments.size()) + " ranged comments\n"
            +   "number seperator:" + (m_number_seperator.has_value()? "YES" : "NO");

    }
};

#endif //CODE_HIGHLIGHT_INFO_HPP
