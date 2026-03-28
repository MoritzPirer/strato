#include <ncurses.h>

#include "../../inc/View/NcursesSession.hpp"

NcursesSession::NcursesSession() {
    initscr();
    if (!stdscr) {
        return;
    } 

    initialized = true;

    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    set_escdelay(25);
    curs_set(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
    
    start_color();
    use_default_colors(); 

    /// colors for rendering
    init_pair(COLOR_NORMAL, -1,   -1); 
    init_pair(COLOR_WEAK_HIGHLIGHT, COLOR_MAGENTA, -1);
    init_pair(COLOR_MEDIUM_HIGHLIGHT, COLOR_BLUE,    -1);
    init_pair(COLOR_STRONG_HIGHLIGHT, -1, COLOR_BLUE);

    init_pair(COLOR_FILE_CHANGED, COLOR_YELLOW,  -1);
    init_pair(COLOR_FILE_SAVED, COLOR_GREEN,   -1);
    init_pair(COLOR_FILE_NEW, COLOR_RED,     -1); 

    init_pair(COLOR_CODE_COMMENT, COLOR_GREEN, -1); 
    init_pair(COLOR_CODE_KEYWORD, COLOR_BLUE,  -1);
    init_pair(COLOR_CODE_BUILTIN, COLOR_MAGENTA,  -1);
    init_pair(COLOR_CODE_STRING, COLOR_YELLOW,  -1);
    init_pair(COLOR_CODE_NUMBER, COLOR_CYAN,  -1);
}

NcursesSession::~NcursesSession() {
    if (!initialized) { 
        return;
    }
    curs_set(1);
    endwin();
}