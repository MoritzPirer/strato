///
/// @file: NcursesSession.hpp
/// @description: A helper class for initializing and cleaning up ncurses
///
/// @date: 2026-01-14
/// @author: MoritzPirer
///

#ifndef NCURSES_SESSION_HPP
#define NCURSES_SESSION_HPP

#define COLOR_NORMAL 1
#define COLOR_WEAK_HIGHLIGHT 2
#define COLOR_MEDIUM_HIGHLIGHT 3
#define COLOR_STRONG_HIGHLIGHT 4
#define COLOR_FILE_NEW 5
#define COLOR_FILE_CHANGED 6
#define COLOR_FILE_SAVED 7
#define COLOR_CODE_COMMENT 8
#define COLOR_CODE_KEYWORD 9 
#define COLOR_CODE_BUILTIN 10 
#define COLOR_CODE_STRING 11 
#define COLOR_CODE_NUMBER 12 

class NcursesSession {
private: 
    bool initialized;
public:
    NcursesSession();
    NcursesSession(const NcursesSession&) = delete;
    ~NcursesSession();
};

#endif //NCURSES_SESSION_HPP
