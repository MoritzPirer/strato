///
/// @file: TextRenderer.hpp
/// @description: description
///
/// @date: 2026-3-15
/// @author: Moritz Pirer
///

#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#include <vector>

#include "../../Shared/DataFlow/VisualSegment.hpp"
#include "../../Model/EditorState.hpp"
#include "EscapeState.hpp"

class TextRenderer {
private:
    const EditorState& m_state; 

public:
    TextRenderer(const EditorState& state);
    TextRenderer(const TextRenderer&) = default;
    ~TextRenderer() = default;

    std::vector<std::vector<VisualSegment>> renderVisibleText(ScreenSize text_area_size);
};

#endif //TEXT_RENDERER_HPP
