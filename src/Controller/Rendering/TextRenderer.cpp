#include <algorithm>

#include "../../../inc/Controller/Rendering/TextRenderer.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"
#include "../../../inc/Controller/Rendering/RenderPipeline.hpp"
#include "../../../inc/Controller/Rendering/MarkdownInterpreter.hpp"
#include "../../../inc/Controller/Rendering/CodeInterpreter.hpp"
#include "../../../inc/Controller/Rendering/NullInterpreter.hpp"
#include "../../../inc/Controller/Rendering/InterpreterDispatcher.hpp"

using std::vector, std::string;

TextRenderer::TextRenderer(const EditorState& state): m_state{state} {}

vector<vector<VisualSegment>> TextRenderer::renderVisibleText(ScreenSize text_area_size, std::string executable_path) {

    InterpreterDispatcher::instance(executable_path);
    
    RenderPipeline pipeline = createPipeline(text_area_size);

    Position first_visible = m_state.getFirstVisibleChar(text_area_size);

    for (int i = 0; i < first_visible.row; i++) {
        const std::string& offscreen_paragraph = m_state.getParagraph(i);
        if (offscreen_paragraph.empty()) {
            continue;
        }

        if (InterpreterDispatcher::instance().isMarkdownCodeBlockFence(offscreen_paragraph)) {
            pipeline.setInterpreter(InterpreterDispatcher::instance().getInterpreter(offscreen_paragraph, m_state.getFileExtension()));
        }

        vector<vector<VisualSegment>> temp = pipeline.renderParagraph(
            m_state.getParagraph(i), "", - 1);
    }
    
    int paragraph_index = first_visible.row;
    vector<vector<VisualSegment>> visible_rows;    
    visible_rows.reserve(text_area_size.height);


    bool is_first_paragraph = true;
    for (int visual_row = 0; visual_row < text_area_size.height;) {

        if (static_cast<size_t>(paragraph_index) >= m_state.getNumberOfParagrahps()) { 
            visible_rows.push_back({VisualSegment{
                "~",
                TextRole::WEAK_HIGHLIGHT,
                TextStyle::makeNormal()
            }}); // FUTURE: load placeholder line from settings

            visual_row++;
            continue;
        }

        const std::string& paragraph = m_state.getParagraph(paragraph_index);
        //update renderer here
        if (InterpreterDispatcher::instance().isMarkdownCodeBlockFence(paragraph)) {
            pipeline.setInterpreter(InterpreterDispatcher::instance().getInterpreter(paragraph, m_state.getFileExtension()));
        }

        int start_column = (is_first_paragraph? first_visible.column : 0);
        is_first_paragraph = false;

        std::string next = m_state.isLastParagraph(paragraph_index)?
            "" : m_state.getParagraph(paragraph_index + 1);
            
        vector<vector<VisualSegment>> temp = pipeline.renderParagraph(
            m_state.getParagraph(paragraph_index), next, start_column);

        //TODO: only add rows until visual_row = text_area_size.height
        visible_rows.insert(visible_rows.end(), temp.begin(), temp.end());
        visual_row += temp.size();
        paragraph_index++;
    }
    
    return visible_rows;
}

RenderPipeline TextRenderer::createPipeline(ScreenSize text_area_size) {
    const std::string& file_name = m_state.getFileName();
    
    size_t start_of_extension = file_name.find_last_of('.');
    if (start_of_extension == std::string::npos) {
        return RenderPipeline(std::make_shared<NullInterpreter>(), text_area_size.width);
    }

    std::string extension = file_name.substr(start_of_extension);
    if (extension == ".md") {
        return RenderPipeline(std::make_shared<MarkdownInterpreter>(), text_area_size.width);
    }

    auto interpreter = InterpreterDispatcher::instance().getInterpreterByExtension(file_name.substr(start_of_extension));
    if (interpreter.has_value()) {
        return RenderPipeline(*interpreter, text_area_size.width);
    }

    return RenderPipeline(std::make_shared<NullInterpreter>(), text_area_size.width);
}