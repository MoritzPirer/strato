#include <algorithm>

#include "../../../inc/Controller/Rendering/Renderer.hpp"
#include "../../../inc/Controller/Rendering/TextRenderer.hpp"
#include "../../../inc/Controller/Rendering/LineNumberRenderer.hpp"
#include "../../../inc/Controller/Rendering/MetadataRenderer.hpp"

#include "../../../inc/Shared/Utils/StringHelpers.hpp"

using std::vector, std::string;

Renderer::Renderer(const EditorState& state, const Settings& settings, const ModeManager& mode_manager, std::string executable_path):
    c_state{state},
    c_settings{settings},
    c_mode_manager{mode_manager},
    c_executable_path{executable_path}
    {}

vector<vector<VisualSegment>> Renderer::renderVisibleText(ScreenSize text_area_size) {
    return TextRenderer(c_state).renderVisibleText(text_area_size, c_executable_path);
}

vector<VisualSegment> Renderer::calculateLineNumbers(ScreenSize text_area_size) {
    return LineNumberRenderer(c_state, c_settings).calculateLineNumbers(text_area_size);
}

int Renderer::calculateLineNumberWidth() {
    return LineNumberRenderer(c_state, c_settings).calculateLineNumberWidth();
}

vector<vector<VisualSegment>> Renderer::calculateMetadataRows(ScreenSize actual_size) {
    return MetadataRenderer(c_state, c_settings, c_mode_manager).calculateMetadataRows(actual_size);
}

vector<VisualSegment> Renderer::calculateTemporaryRows(ScreenSize actual_size) {
    return MetadataRenderer(c_state, c_settings, c_mode_manager).calculateTemporaryRows(actual_size);
}

Position Renderer::calculateScreenPositionOfCursor(ScreenSize text_area_size) {
    int screen_row = std::min(
        text_area_size.height / 2, // keep cursor in top half of screen
        c_state.calculateVisualLineOfCursor(text_area_size.width)
    ); 

    int screen_column = c_state.getCursor().getColumn() % text_area_size.width;

    return {screen_row, screen_column};
}
