#include <algorithm>

#include "../../../inc/Controller/Rendering/TextRenderer.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"
#include "../../../inc/Controller/Rendering/RenderPipeline.hpp"
#include "../../../inc/Controller/Rendering/MarkdownInterpreter.hpp"
#include "../../../inc/Controller/Rendering/CodeInterpreter.hpp"

using std::vector, std::string;

TextRenderer::TextRenderer(const EditorState& state): m_state{state} {}

vector<vector<VisualSegment>> TextRenderer::renderVisibleText(ScreenSize text_area_size) {
    // RenderPipeline pipeline = RenderPipeline(
    //     std::make_shared<MarkdownInterpreter>(),
    //     text_area_size.width
    // );

    RenderPipeline pipeline = RenderPipeline(
        std::make_shared<CodeInterpreter>(CodeHighlightInfo::testPython()),
        text_area_size.width
    );

    Position first_visible = m_state.getFirstVisibleChar(text_area_size);

    for (int i = 0; i < first_visible.row; i++) {
        const std::string& offscreen_paragraph = m_state.getParagraph(i);
        if (offscreen_paragraph.empty()) {
            continue;
        }

        vector<vector<VisualSegment>> temp = pipeline.renderParagraph(
            m_state.getParagraph(i), "", - 1);
    }
    
    int current_paragraph = first_visible.row;
    vector<vector<VisualSegment>> visible_rows;    
    visible_rows.reserve(text_area_size.height);


    bool is_first_paragraph = true;
    for (int visual_row = 0; visual_row < text_area_size.height;) {

        if (static_cast<size_t>(current_paragraph) >= m_state.getNumberOfParagrahps()) { 
            visible_rows.push_back({VisualSegment{
                "~",
                TextRole::WEAK_HIGHLIGHT,
                TextStyle::makeNormal()
            }}); // FUTURE: load placeholder line from settings

            visual_row++;
            continue;
        }

        int start_column = (is_first_paragraph? first_visible.column : 0);
        is_first_paragraph = false;

        std::string next = m_state.isLastParagraph(current_paragraph)?
            "" : m_state.getParagraph(current_paragraph + 1);
            
        vector<vector<VisualSegment>> temp = pipeline.renderParagraph(
            m_state.getParagraph(current_paragraph), next, start_column);

        //TODO: only add rows until visual_row = text_area_size.height
        visible_rows.insert(visible_rows.end(), temp.begin(), temp.end());
        visual_row += temp.size();
        current_paragraph++;
    }
    
    return visible_rows;
}