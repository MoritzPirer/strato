///
/// @file: Renderer.hpp
/// @description: handles everything related to rendering the things to be shown on the screen
///
/// @date: 2026-02-06
/// @author: Moritz Pirer
///

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../../Shared/DataFlow/VisualSegment.hpp"
#include "../../Model/EditorState.hpp"
#include "../Modes/ModeManager.hpp"
#include "../Services/Settings/Settings.hpp"

class Renderer {
private:
    const EditorState& c_state;
    const Settings& c_settings;
    const ModeManager& c_mode_manager;
    const std::string c_executable_path;

public:
    Renderer(const EditorState& state, const Settings& settings, const ModeManager& mode_manager, std::string executable_path);
    Renderer(const Renderer&) = default;
    ~Renderer() = default;

    /// @brief calculates what lines are visible based on the position of the cursor and the size
    ///     of the text area
    /// @param text_area_size the size of the text area (i.e. the screen size without the metadata
    ///     panel and the line numbers)
    /// @return a vector where each line is a set of VisualSegments that should
    ///     be rendered in a line of the screen
    std::vector<std::vector<VisualSegment>> renderVisibleText(ScreenSize text_area_size);

    /// @brief calculate how much horizontal space the line numbers need
    int calculateLineNumberWidth();

    /// @brief calculates the line numbers to show on the left edge of the text area
    /// @return the line numbers to render (empty if line numbers are turned of)
    std::vector<VisualSegment> calculateLineNumbers(ScreenSize text_area_size);
    
    /// @brief calculates the rows that should be shown for the metadata panel
    /// @param actual_size the total size of the screen (not only the text area)
    /// @return a vector where each element is a set of VisualSegments
    /// that should be rendered in the same row of the screen 
    std::vector<std::vector<VisualSegment>> calculateMetadataRows(ScreenSize actual_size);

    std::vector<VisualSegment> calculateTemporaryRows(ScreenSize actual_size);
    
    /// @brief calculates where on the screen the cursor should be rendered
    /// @param text_area_size the size of the text area
    Position calculateScreenPositionOfCursor(ScreenSize text_area_size);
};

#endif //RENDERER_HPP
