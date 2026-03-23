///
/// @file: MarkdownInterpreter.hpp
/// @description: description
///
/// @date: 2026-03-21
/// @author: Moritz Pirer
///

#ifndef MARKDOWN_INTERPRETER_HPP
#define MARKDOWN_INTERPRETER_HPP

#include <vector>
#include "Interpreter.hpp"
#include "../../Shared/DataFlow/TextStyle.hpp"

class MarkdownInterpreter: public Interpreter {
private:
    TextStyle m_current_style;
    bool m_style_is_locked;

    const char c_heading_indicator = '#';
    const char c_underline_indicator = '-';
    const int c_min_underline_count = 2;
    const char c_quote_indicator = '>';

    TextStyle pickStyle(const std::string& token);

    bool isHeading(const std::string& current_paragraph, const std::string& next_paragraph);

    bool isUnterline(const std::string& paragraph);
    bool isQuote(const std::string& paragraph);
public:
    MarkdownInterpreter();
    MarkdownInterpreter(const MarkdownInterpreter&) = default;
    ~MarkdownInterpreter() = default;

    VisualSegment parseToken(std::string token, TextRole paragraph_role) override;

    TextRole findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) override;

    void reset() override;
};

#endif //MARKDOWN_INTERPRETER_HPP
