///
/// @file: TextRole.hpp
/// @description: is used to assign semantic meaning to a piece of displayed text. used to infer color
///
/// @date: 2026-02-02
/// @author: Moritz Pirer
///

#ifndef TEXT_ROLE_HPP
#define TEXT_ROLE_HPP

enum class TextRole {
    NORMAL_TEXT,
    MEDIUM_HIGHLIGHT,
    WEAK_HIGHLIGHT,
    STRONG_HIGHLIGHT,
    FILE_NEW,
    FILE_CHANGED,
    FILE_SAVED,
    CODE_BUILTIN,
    CODE_KEYWORD,
    CODE_STRING_LITERAL,
    CODE_NUMBER_LITERAL,
    CODE_COMMENT
};

#endif // TEXT_ROLE_HPP

