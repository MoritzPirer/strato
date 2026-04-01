///
/// @file: EditorState.hpp
/// @description: Manages the internal state of the Editor, including the currently opened file and the
///     position of the cursor
///
/// @date: 2026-01-14
/// @author: Moritz Pirer
///

#ifndef EDITOR_STATE_HPP
#define EDITOR_STATE_HPP

#include <optional>
#include <atomic>

#include "TextFile.hpp"
#include "Cursor.hpp"
#include "../Shared/Types/ScreenSize.hpp"
#include "../Shared/Types/Direction.hpp"
#include "Clipboard/Clipboard.hpp"

class EditorState {
private:
    TextFile m_file;
    Cursor m_cursor;
    std::optional<Clipboard> m_clipboard = std::nullopt;

    std::atomic<bool> m_is_quit;
    std::atomic<bool> m_is_backed_up = false;

    std::vector<std::string> m_temporary_messages;

    bool isCursorInLastRowOfParagraph(int screen_width);
    
    /// @brief calculates the first visible char after skipping some number of visual rows
    /// @param offscreen_visual_lines how many lines to skip
    /// @param screen_width how wide the text area is
    Position skipOffscreenLines(int offscreen_visual_lines, int screen_width) const;

public:
    EditorState():
        m_file{},
        m_cursor{},  
        m_is_quit{false} {}

    EditorState(TextFile file): m_file{file} {}  
    
    EditorState(const EditorState&) = delete;
    ~EditorState() = default;
    
    /// @return the number of paragraphs in the file
    size_t getNumberOfParagrahps() const { return m_file.getNumberOfParagrahps(); }
    size_t getNumberOfWords() const { return m_file.getNumberOfWords(); }
    size_t getNumberOfCharacters() const { return m_file.getNumberofCharacters(); }

    SaveState getSaveState() const { return m_file.getSaveState(); }
    bool needsBackup() const { return m_is_backed_up; }
    void requestBackup() { m_is_backed_up = false; }
    void registerBackup() { m_is_backed_up = true; }
    std::string getFileName() const { return m_file.getFilepath().filename(); }

    const std::string& getParagraph(size_t row) const { return m_file.getParagraph(row); }

    const Cursor& getCursor() const { return m_cursor; }
    TextFile& getFile() { return m_file; }
    std::string getFileExtension() const { return m_file.getFilepath().extension(); }
    bool getIsQuit() const { return m_is_quit; }
    void setIsQuit(bool is_quit) { m_is_quit = is_quit; } 
    
    void moveCursorUp(int screen_width);
    void moveCursorLeft();
    void moveCursorDown(int screen_width);
    void moveCursorRight();

    /// @param direction should be LEFT or RIGHT, otherwise the function has no effect
    void moveCursorSideways(Direction direction);
    void moveCursor(Direction direction, int screen_width);
    void moveCursorTo(Position position);
    bool canCursorMove(Direction direction) const;

    void insertCharacterAt(char character_to_add, Position position);
    void insertLines(std::vector<std::string> content, Position start);

    std::optional<char> readCharacterAtCursor();
    void setCharacterAt(char character_to_set, Position position);
    
    /// @brief deletes from start to end, including both end points. if the range is 
    ///     an empty line, it is removed, not just cleared.
    /// @param start the inclusive start of deletion 
    /// @param end the inclusive end of deletion 
    void deleteRange(Position start, Position end);

    std::vector<std::string> copyRange(Position start, Position end);

    void splitAt(Position position);

    void joinNextParagraphTo(int paragraph_index);

    /// @brief calculates the logical position of the first character to be shown on screen
    /// @param size the size of the text area
    Position getFirstVisibleChar(ScreenSize size) const;
    
    int calculateVisualLineOfCursor(int screen_width) const;

    void addTemporaryMessage(std::string message);
    const std::vector<std::string>& getTemporaryMessages() const;
    void clearTemporaryMessages();

    void setClipboard(Clipboard clipboard) { m_clipboard = clipboard; }
    std::optional<Clipboard> getClipboard() const { return m_clipboard; }


    bool isLastParagraph(int paragraph_index) const;
};

#endif //EDITOR_STATE_HPP
