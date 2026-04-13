#include <algorithm>

#include "../../../inc/Controller/Rendering/CodeInterpreter.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

CodeInterpreter::CodeInterpreter(CodeHighlightInfo highlight_info):
    Interpreter(highlight_info.m_token_splitters, highlight_info.m_token_groups),
    m_highlight_info{highlight_info},
    m_is_end_of_paragraph_comment{false},
    m_is_ranged_comment{false}
    {}


bool CodeInterpreter::contains(const std::vector<std::string>& vec, const std::string& target) {
    if (m_highlight_info.m_ignore_case) {
        return std::binary_search(vec.begin(), vec.end(), target, StringHelpers::caselessCompare);
    }

    return (std::binary_search(vec.begin(), vec.end(), target));
}

VisualSegment CodeInterpreter::parseToken(std::string token, TextRole paragraph_role) {
    if (token.empty()) {
        goto makenormal;
    }
    // IS END OF RANGED COMMENT
    for (auto pair : m_highlight_info.m_ranged_comments) {
        if (!m_ranged_comment_opener.has_value() || pair.first != *m_ranged_comment_opener) {
            continue;
        }

        if (pair.second.ends_with(token)) {
            m_is_ranged_comment = false;
            m_ranged_comment_opener = std::nullopt;

            return makeComment(token);
        }
    }
    
    // IS WITHIN COMMENT
    if (m_is_end_of_paragraph_comment || m_is_ranged_comment) {
        return makeComment(token);
    }

    //  IS START OF REST OF LINE COMMENT
    if (contains(m_highlight_info.m_rest_of_line_comments, token)) {
        m_is_end_of_paragraph_comment = true;

        return makeComment(token);
    }

    // IS START OF RANGED COMMENT
    for (auto [start, end] : m_highlight_info.m_ranged_comments) {
        if (token.starts_with(start)) {
            m_ranged_comment_opener = start;
            m_is_ranged_comment = true;

            return makeComment(token);
        }
    }

    // IS END OF STRING
    if (m_is_within_string && m_string_opener.has_value()) {
        for (auto [start, end] : m_highlight_info.m_string_indicators) {
            if (!token.ends_with(end)) {
                continue;
            }

            m_is_within_string = false;
            m_string_opener = std::nullopt;

            return makeStringLiteral(token);
        }
    }

    // IS START OF STRING
    for (auto [start, end] : m_highlight_info.m_string_indicators) {
        if (token == start + end) { // empty string
            return makeStringLiteral(token);
        }

        if (token.starts_with(start)) {
            m_is_within_string = true;
            m_string_opener = start;

            return makeStringLiteral(token);
        }
    }

    if (m_is_within_string) {
        return makeStringLiteral(token);
    }

    if (StringHelpers::isNumberLiteral(token, m_highlight_info.m_number_seperator)) {
        return makeNumberLiteral(token);
    }

    if (contains(m_highlight_info.m_language_keywords, token)) {
        return makeKeyword(token);
    }

    if (contains(m_highlight_info.m_language_builtins, token)) {
        return makeBuiltin(token);
    }

    makenormal:
    return VisualSegment{
        .content = token,
        .role = paragraph_role,
        .style = TextStyle::makeNormal()
    };
}

VisualSegment CodeInterpreter::makeComment(const std::string& token) {
    return VisualSegment {
        .content = token,
        .role = TextRole::CODE_COMMENT,
        .style = TextStyle::makeItalic()
    };
}

VisualSegment CodeInterpreter::makeNumberLiteral(const std::string& token) {
    return VisualSegment {
        .content = token,
        .role = TextRole::CODE_NUMBER_LITERAL,
        .style = TextStyle::makeNormal()
    };
}

VisualSegment CodeInterpreter::makeStringLiteral(const std::string& token) {
    return VisualSegment {
        .content = token,
        .role = TextRole::CODE_STRING_LITERAL,
        .style = TextStyle::makeNormal()
    };
}

VisualSegment CodeInterpreter::makeKeyword(const std::string& token) {
    return VisualSegment {
        .content = token,
        .role = TextRole::CODE_KEYWORD,
        .style = TextStyle::makeNormal()
    };
}

VisualSegment CodeInterpreter::makeBuiltin(const std::string& token) {
    return VisualSegment {
        .content = token,
        .role = TextRole::CODE_BUILTIN,
        .style = TextStyle::makeNormal()
    };
}

TextRole CodeInterpreter::findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) {
    // return placeholder, unused
    (void) current_paragraph;
    (void) next_paragraph;

    return TextRole::NORMAL_TEXT;
}

void CodeInterpreter::reset() {
    //reset end of line comment, keep ranged comment
    m_is_end_of_paragraph_comment = false;
    m_is_within_string = false;
}