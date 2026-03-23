///
/// @file: Interpreter.hpp
/// @description: description
///
/// @date: 2026-03-21
/// @author: Moritz Pirer
///

#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <string>

#include "../../Shared/DataFlow/VisualSegment.hpp"

class Interpreter {
private:
    std::string m_delimiters;
    std::vector<std::string> m_grouped_delimiters;

public:
    Interpreter(std::string delimiters, std::vector<std::string> grouped_delimiters):
        m_delimiters{delimiters},
        m_grouped_delimiters{grouped_delimiters}
        {}
        
    Interpreter(const Interpreter&) = default;
    virtual ~Interpreter() = default;

    std::string getDelimiters() const { return m_delimiters; }
    std::vector<std::string> getGroupedDelimiters() const { return m_grouped_delimiters; }

    virtual VisualSegment parseToken(std::string token, TextRole paragraph_role) = 0;
    virtual TextRole findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) = 0;
    virtual void reset() = 0;
};

#endif //INTERPRETER_HPP
