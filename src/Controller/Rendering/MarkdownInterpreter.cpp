#include <algorithm>

#include "../../../inc/Controller/Rendering/MarkdownInterpreter.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

MarkdownInterpreter::MarkdownInterpreter():
    Interpreter("`*\\", std::vector<std::string>{"\\`, \\*, **"}),
    m_current_style{TextStyle::makeNormal()},
    m_style_is_locked{false}
    {}

VisualSegment MarkdownInterpreter::parseToken(std::string token, TextRole paragraph_role) {
    return {
        .content = token,
        .role = paragraph_role,
        .style = pickStyle(token)
    };
}

TextStyle MarkdownInterpreter::pickStyle(const std::string& token) {
    if (token == "`") {
        m_style_is_locked = !m_style_is_locked;
        return m_current_style;
    }

    if (!m_style_is_locked) {
        if (token == "*") {
            m_current_style.toggleItalic();
            return {m_current_style.is_bold, true};
        }
        if (token == "**") {
            m_current_style.toggleBold();
            return {true, m_current_style.is_italic};
        }
    }

    return m_current_style;
}

TextRole MarkdownInterpreter::findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) {
    if (isHeading(current_paragraph, next_paragraph)) {
        return TextRole::WEAK_HIGHLIGHT;
    }

    if (isQuote(current_paragraph)) {
        return TextRole::STRONG_HIGHLIGHT;
    }

    return TextRole::NORMAL_TEXT;
}

bool MarkdownInterpreter::isHeading(const std::string& current_paragraph, const std::string& next_paragraph) {
    return isUnterline(next_paragraph)
        || StringHelpers::startsWith(current_paragraph, c_heading_indicator);
}

bool MarkdownInterpreter::isUnterline(const std::string& paragraph) {
    return (StringHelpers::containsOnlyIgnoringWhitespace(paragraph, c_underline_indicator)
        && std::ranges::count(paragraph, c_underline_indicator) >= c_min_underline_count);
}


bool MarkdownInterpreter::isQuote(const std::string& paragraph) {
    return StringHelpers::startsWithIgnoringWhitespace(paragraph, c_quote_indicator);
}

void MarkdownInterpreter::reset() {
    m_current_style = TextStyle::makeNormal();
    m_style_is_locked = false;
}