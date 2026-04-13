#include "../../../inc/Controller/Rendering/MetadataRenderer.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"
#include "../../../inc/Shared/Utils/Version.hpp"

using std::vector, std::string;

MetadataRenderer::MetadataRenderer(const EditorState& state, const Settings& settings, const ModeManager& mode_manager):
    c_state{state},
    c_settings{settings},
    c_mode_manager{mode_manager}
    {}

vector<VisualSegment> MetadataRenderer::calculateTemporaryRows(ScreenSize actual_size) {
    vector<VisualSegment> chunks;

    if (!c_state.getTemporaryMessages().empty()) {
        vector<VisualSegment> seperator_chunks = getSeperatorChunks(actual_size);
        chunks.insert(chunks.end(), seperator_chunks.begin(), seperator_chunks.end());
    }

    for (const string& message : c_state.getTemporaryMessages()) {
        for (const string& message_row : StringHelpers::splitIntoRows(message, 0, actual_size.width - 1)) {
            chunks.push_back({
                StringHelpers::leftAlign(message_row, actual_size.width),
                TextRole::WEAK_HIGHLIGHT,
                TextStyle::makeNormal()
            });
        }
    }

    return chunks;
}

vector<VisualSegment> MetadataRenderer::getSeperatorChunks(ScreenSize actual_size) {
    return { VisualSegment{
        string(actual_size.width - 1, (c_settings.isEnabled("show_seperator") ? '-' : ' ')),
        TextRole::MEDIUM_HIGHLIGHT, TextStyle::makeNormal()
    }};
}

vector<vector<VisualSegment>> MetadataRenderer::calculateMetadataRows(ScreenSize actual_size) {
    vector<VisualSegment> ordered_chunks; 

    auto addContent = [&](const vector<VisualSegment>& chunks, bool supress_divider = false) {
        ordered_chunks.insert(ordered_chunks.end(), chunks.begin(), chunks.end());
        if (chunks.size() != 0 && !supress_divider) {
            ordered_chunks.push_back({" | ", TextRole::NORMAL_TEXT, TextStyle::makeNormal()});
        }
    };
    
    addContent(getSeperatorChunks(actual_size), true);
    addContent(getEditorModeChunks());
    addContent(getFileNameChunks());
    addContent(getSaveIconChunks());
    addContent(getCursorPositionChunks());
    addContent(getCharacterCountChunks());
    addContent(getWordCountChunks());
    addContent(getParagraphCountChunks());
    addContent(getVersionChunks());

    return reorganizeMetadataRows(ordered_chunks, actual_size);
}

vector<VisualSegment> MetadataRenderer::getEditorModeChunks() {
    if (!c_settings.isEnabled("show_editor_mode")) {
        return {};
    }

    return {{c_mode_manager.getModeLabel(), TextRole::WEAK_HIGHLIGHT, TextStyle::makeNormal()}};
}

vector<VisualSegment> MetadataRenderer::getFileNameChunks() {
    if (!c_settings.isEnabled("show_file_name")) {
        return {};
    }

    return {{"Currently editing " + c_state.getFileName(), TextRole::NORMAL_TEXT, TextStyle::makeNormal()}};
}

vector<VisualSegment> MetadataRenderer::getSaveIconChunks() {
    bool show_save_icon = c_settings.isEnabled("show_save_icon");
    bool show_save_text = c_settings.isEnabled("show_save_text");

    if (!show_save_icon && !show_save_text) {
        return {};
    }
    
    string icon = "";
    string text = "";
    TextRole role;    

    switch (c_state.getSaveState()) {
    case SaveState::NEW_FILE: {
        icon = "[!]";
        text = "New File";
        role = TextRole::FILE_NEW;
        break;
    }
    case SaveState::UNSAVED_CHANGES: {
        icon = "[*]";
        text = "Unsaved Changes";
        role = TextRole::FILE_CHANGED;
        break;
    }
    case SaveState::SAVED: {
        icon = "[=]";
        text = "Changes Saved";
        role = TextRole::FILE_SAVED;
    }
    }

    string message = icon + (show_save_icon && show_save_text? " " : "") + text;

    return {{message, role, TextStyle::makeNormal()}};
}

vector<VisualSegment> MetadataRenderer::getCursorPositionChunks() {
    if (!c_settings.isEnabled("show_cursor_position")) {
        return {};
    }

    Position position = c_state.getCursor().getPosition();
    position.row++; // display as 1-indexed

    return {
        {"Cursor position: ", TextRole::NORMAL_TEXT, TextStyle::makeNormal()},
        {position.format(), TextRole::WEAK_HIGHLIGHT, TextStyle::makeBoldItalic()}
    };
}

vector<VisualSegment> MetadataRenderer::getCharacterCountChunks() {
    if (!c_settings.isEnabled("show_character_count")) {
        return {};
    }

    int character_count = c_state.getNumberOfCharacters();
    std::string count_string = StringHelpers::addSeperators(character_count, 3);

    return {
        {"Character(s): ", TextRole::NORMAL_TEXT, TextStyle::makeNormal()},
        {count_string, TextRole::WEAK_HIGHLIGHT, TextStyle::makeItalic()}
    };
}

vector<VisualSegment> MetadataRenderer::getWordCountChunks() {
    if (!c_settings.isEnabled("show_word_count")) {
        return {};
    }

    int word_count = c_state.getNumberOfWords();
    std::string count_string = StringHelpers::addSeperators(word_count, 3);

    return {
        {"Word(s): ", TextRole::NORMAL_TEXT, TextStyle::makeNormal()},
        {count_string, TextRole::WEAK_HIGHLIGHT, TextStyle::makeItalic()}
    };
}

vector<VisualSegment> MetadataRenderer::getParagraphCountChunks() {
    if (!c_settings.isEnabled("show_paragraph_count")) {
        return {};
    }

    int paragraph_count = c_state.getNumberOfParagrahps();
    std::string count_string = StringHelpers::addSeperators(paragraph_count, 3);

    return {
        {"Paragraph(s): ", TextRole::NORMAL_TEXT, TextStyle::makeNormal()},
        {count_string, TextRole::WEAK_HIGHLIGHT, TextStyle::makeItalic()}
    };
}

vector<VisualSegment> MetadataRenderer::getVersionChunks() {
    if (!c_settings.isEnabled("show_version")) {
        return {};
    }

    return {{VERSION, TextRole::NORMAL_TEXT, TextStyle::makeNormal()}};
}

vector<vector<VisualSegment>> MetadataRenderer::reorganizeMetadataRows(
    vector<VisualSegment> ordered_chunks, ScreenSize actual_size) {
    
    int row_length = 0;
    vector<vector<VisualSegment>> rows;
    vector<VisualSegment> current_row;

    int max_used_width = actual_size.width - 1;

    for (const VisualSegment& chunk : ordered_chunks) {
        if (row_length + static_cast<int>(chunk.content.size()) > max_used_width) {
            rows.push_back(current_row);
            current_row.clear();
            row_length = 0;
        }

        current_row.push_back(chunk);
        row_length += chunk.content.size();
    }

    if (!current_row.empty()) {
        rows.push_back(current_row);
    }

    return rows; 
}