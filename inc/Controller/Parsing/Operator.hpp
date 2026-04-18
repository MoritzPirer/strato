///
/// @file: Operator.hpp
/// @description: describes the different operators the editor knows
///
/// @date: 2026-02-07
/// @author: Moritz Pirer
///

#ifndef OPERATOR_TYPE_HPP
#define OPERATOR_TYPE_HPP

enum class Operator {
    MOVE_BY_CHARACTER,
    MOVE_TO_END,
    MOVE_TO_NEXT,
    MOVE_TO_FIND,

    PARAGRAPH_JOIN,
    PARAGRAPH_SPLIT,
    PARAGRAPH_CREATE,

    DELETE_SINGLE,
    DELETE_WITHIN,
    DELETE_UNTIL,
    REPLACE,

    INDENT,
    UNINDENT,
    CASE_SET_LOWER,
    CASE_SET_UPPER,

    FILE_ACTION,
    SWITCH_MODE,

    UNDO,
    REDO,
    COPY_WITHIN,
    COPY_UNTIL,
    PASTE,
    REPEAT,
    HELP
};

#endif //OPERATOR_TYPE_HPP
