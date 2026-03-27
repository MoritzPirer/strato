#include <ncurses.h>

#include "../../inc/View/UiHandler.hpp"
#include "../../inc/Shared/Types/SpecialKey.hpp"

void UiHandler::renderLine(int start_visual_row, const std::string& line) {
    mvprintw(start_visual_row, 0, "%s", line.c_str());
}

void UiHandler::writeString(const std::string& content) {
    addstr(content.c_str());
}

void UiHandler::setStyle(TextStyle style) {
    // don't reset to keep textrole
    attroff(A_BOLD);
    attroff(A_ITALIC);
    
    if (style.is_bold) {
        attron(A_BOLD);
    }
    
    if (style.is_italic) {
        attron(A_ITALIC);
    }
}

void UiHandler::setRole(TextRole role) {
    attrset(A_NORMAL);
    
    switch (role) {
    case TextRole::NORMAL_TEXT: {
        attron(COLOR_PAIR(COLOR_NORMAL));
        break;
    }

    case TextRole::WEAK_HIGHLIGHT: {
        attron(COLOR_PAIR(COLOR_WEAK_HIGHLIGHT));
        break;
    }
    
    case TextRole::MEDIUM_HIGHLIGHT: {
        attron(COLOR_PAIR(COLOR_MEDIUM_HIGHLIGHT));
        break;
    }

    case TextRole::STRONG_HIGHLIGHT: {
        attron(COLOR_PAIR(COLOR_STRONG_HIGHLIGHT));
        break;
    }
    
    case TextRole::FILE_CHANGED: {
        attron(COLOR_PAIR(COLOR_FILE_CHANGED));
        break;
    }
    
    case TextRole::FILE_NEW: {
        attron(COLOR_PAIR(COLOR_FILE_NEW));
        break;
    }
    
    case TextRole::FILE_SAVED: {
        attron(COLOR_PAIR(COLOR_FILE_SAVED));
        break;
    }

    case TextRole::CODE_KEYWORD: {
        attron(COLOR_PAIR(COLOR_CODE_KEYWORD));
        break;
    }

    case TextRole::CODE_NUMBER_LITERAL: {
        attron(COLOR_PAIR(COLOR_CODE_NUMBER));
        break;
    }

    case TextRole::CODE_COMMENT: {
        attron(COLOR_PAIR(COLOR_CODE_COMMENT));
        break;
    }

    case TextRole::CODE_STRING_LITERAL: {
        attron(COLOR_PAIR(COLOR_CODE_STRING));
        break;
    }

    case TextRole::CODE_BUILTIN: {
        attron(COLOR_PAIR(COLOR_CODE_BUILTIN));
        break;
    }



    }
}

void UiHandler::renderTextArea(const RenderInfo& render_info) {
    for (int i = 0; i < render_info.getTextAreaRowCount(); i++) {
        move(i, render_info.getAsideWidth());

        for (auto& [content, role, style] : render_info.getTextAreaRow(i)) {
            if (render_info.shouldRenderColors()) {
                setRole(role);
            }
            else {
                setRole(TextRole::NORMAL_TEXT);
            }
            setStyle(style);
            
            writeString(content);
        }  
    }
}

void UiHandler::renderOverlay(const RenderInfo& render_info) {
    int overlay_offset = render_info.getTextAreaRowCount()
        - render_info.getOverlayRowCount();

    for (int i = 0; i < render_info.getOverlayRowCount(); i++) {
        move(overlay_offset + i, 0);
        
        const auto& [content, role, style] = render_info.getOverlayRow(i);

        if (render_info.shouldRenderColors()) {
            setRole(role);
        }
        else {
            setRole(TextRole::NORMAL_TEXT);
        }

        setStyle(style);

        writeString(content);
    }
}

void UiHandler::renderCursor(const RenderInfo& render_info) {
    move(
        render_info.getCursorPosition().row,
        render_info.getCursorPosition().column + render_info.getAsideWidth()
    );

    curs_set(1);
}

void UiHandler::renderPanel(const RenderInfo& render_info) {
    int panel_offset = render_info.getTextAreaRowCount();

    for (int i = 0; i < render_info.getPanelRowCount(); i++) {
        move(panel_offset, 0);
        for (auto& [content, role, style] : render_info.getPanelRow(i)) {
            if (render_info.shouldRenderColors()) {
                setRole(role);
            }
            else {
                setRole(TextRole::NORMAL_TEXT);
            }

            setStyle(style);
            
            writeString(content);
        }  
        panel_offset++;
    }
}

void UiHandler::renderAside(const RenderInfo& render_info) {
    for (int i = 0; i < render_info.getAsideRowCount(); i++) {
        move(i, 0);
        const auto& [content, role, style] = render_info.getAsideRow(i);

        if (render_info.shouldRenderColors()) {
            setRole(role);
        }
        else {
            setRole(TextRole::NORMAL_TEXT);
        }

        setStyle(style);

        writeString(content);
    }

}

Input UiHandler::translateMouseEvent() {
    MEVENT event;

    if (getmouse(&event) != OK) {
        return {};
    }
    
    /// left click
    if (event.bstate & BUTTON1_CLICKED) {
        return {Position{event.y, event.x}};
    }

    /// mousewheel up
    if (event.bstate & BUTTON4_PRESSED) {
        return {SpecialKey::ARROW_UP};
    }

    // mousewheel down -> probably not portable
    if (event.bstate & (1 << 21)) {
        return {SpecialKey::ARROW_DOWN};
    }

    return {};
}

Input UiHandler::translateInput(int original_input) {
    if (original_input == KEY_MOUSE) {
        return translateMouseEvent();
    }

    switch (original_input) {
    case '\n':
    case '\r':
    case KEY_ENTER: {
        return {SpecialKey::ENTER};
    }
    
    case 27: {
        return {SpecialKey::ESCAPE};
    }
    
    case KEY_BACKSPACE:
    case 127:
    case '\b': {
        return {SpecialKey::BACKSPACE};
    }

    case KEY_LEFT: {
        return {SpecialKey::ARROW_LEFT};
    }

    case KEY_RIGHT: {
        return {SpecialKey::ARROW_RIGHT};
    }

    case KEY_UP: {
        return {SpecialKey::ARROW_UP};
    }

    case KEY_DOWN: {
        return {SpecialKey::ARROW_DOWN};
    }

    case '\t': {
        return {SpecialKey::TAB};
    }

    case KEY_BTAB: {
        return {SpecialKey::SHIFT_TAB};
    }
    case KEY_RESIZE: {
        return {};
    }

    default: {
        return {static_cast<char>(original_input)};
    }

    }
}

ScreenSize UiHandler::screenSize() const {
    return {getmaxy(stdscr), getmaxx(stdscr)};
}

void UiHandler::render(const RenderInfo& render_info) {
    clear();

    renderTextArea(render_info);
    renderPanel(render_info);
    renderAside(render_info);
    renderOverlay(render_info);
    renderCursor(render_info);

    refresh();
}

Input UiHandler::getInput() {
    return translateInput(getch());
}