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
    std::unordered_set<std::string> m_language_names;
    std::vector<std::string> m_file_extensions;
    std::unordered_set<std::string> m_language_keywords;
    std::unordered_set<std::string> m_language_builtins;

    std::vector<std::pair<std::string, std::string>> m_string_indicators;

    std::string m_token_splitters;
    std::vector<std::string> m_token_groups;

    std::unordered_set<std::string> m_rest_of_line_comments;
    std::vector<std::pair<std::string, std::string>> m_ranged_comments;

    std::optional<char> m_number_seperator;

    CodeHighlightInfo() = default;
    CodeHighlightInfo(const CodeHighlightInfo&) = default;
    ~CodeHighlightInfo() = default;

    static CodeHighlightInfo testPython();
};

#endif //CODE_HIGHLIGHT_INFO_HPP
