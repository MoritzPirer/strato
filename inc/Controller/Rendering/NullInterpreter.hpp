///
/// @file: NullInterpreter.hpp
/// @description: description
///
/// @date: 2026-03-21
/// @author: Moritz Pirer
///

#ifndef NULL_INTERPRETER_HPP
#define NULL_INTERPRETER_HPP

#include <vector>
#include "Interpreter.hpp"
#include "../../Shared/DataFlow/TextStyle.hpp"

class NullInterpreter: public Interpreter {
public:
    NullInterpreter();
    NullInterpreter(const NullInterpreter&) = default;
    ~NullInterpreter() = default;

    VisualSegment parseToken(std::string token, TextRole paragraph_role) override;

    TextRole findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) override;

    void reset() override;
};

#endif //NULL_INTERPRETER_HPP
