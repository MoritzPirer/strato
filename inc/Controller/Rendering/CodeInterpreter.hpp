///
/// @file: Codeinterpreter.hpp
/// @description: description
///
/// @date: 2026-03-26
/// @author: Moritz Pirer
///

#ifndef CODE_INTERPRETER_HPP
#define CODE_INTERPRETER_HPP

#include <vector>
#include "Interpreter.hpp"
#include "../../Shared/DataFlow/TextStyle.hpp"
#include "CodeHighlightInfo.hpp"

class CodeInterpreter: public Interpreter {
private:
    CodeHighlightInfo m_highlight_info;

    bool m_is_end_of_paragraph_comment;
    bool m_is_ranged_comment;
    bool m_is_within_string;
    std::optional<std::string> m_string_opener;
    std::optional<std::string> m_ranged_comment_opener;
    
    bool contains(const std::vector<std::string>& vec, const std::string& target);
    VisualSegment makeComment(const std::string& token);
    VisualSegment makeNumberLiteral(const std::string& token);
    VisualSegment makeStringLiteral(const std::string& token);
    VisualSegment makeKeyword(const std::string& token);
    VisualSegment makeBuiltin(const std::string& token);

public:
    CodeInterpreter(CodeHighlightInfo highlight_info);
    CodeInterpreter(const CodeInterpreter&) = default;
    ~CodeInterpreter() = default;

    VisualSegment parseToken(std::string token, TextRole paragraph_role) override;

    TextRole findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) override;

    void reset() override;

    CodeHighlightInfo getHighlightInfo() const { return m_highlight_info; }
};

#endif //CODE_INTERPRETER_HPP
