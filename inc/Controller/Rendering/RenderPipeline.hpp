///
/// @file: RenderPipeline.hpp
/// @description: description
///
/// @date: 2026-03-21
/// @author: Moritz Pirer
///

#ifndef RENDER_PIPELINE_HPP
#define RENDER_PIPELINE_HPP

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "../../Shared/DataFlow/VisualSegment.hpp"
#include "Interpreter.hpp"

class RenderPipeline {
private:
    std::shared_ptr<Interpreter> m_interpreter;
    size_t m_max_width;

    std::vector<std::string> tokenize(const std::string& paragraph);
    bool isDelimiter(char c);
    void mergeIfPossible(std::string& current_token, char current, std::vector<std::string>& tokens);

    std::vector<VisualSegment> interpret(
        const std::vector<std::string>& tokenized_paragraph,
        int disregard_until,
        TextRole paragraph_role
    );

    std::vector<std::vector<VisualSegment>> restructure(const std::vector<VisualSegment>& interpreted_paragraph);

    VisualSegment splitSegmentAt(VisualSegment& segment, int index);
public:
    RenderPipeline(std::shared_ptr<Interpreter> interpreter, size_t max_width);
    RenderPipeline(const RenderPipeline&) = default;
    ~RenderPipeline() = default;

    void setInterpreter(std::shared_ptr<Interpreter> interpreter) { m_interpreter = interpreter; }

    std::vector<std::vector<VisualSegment>> renderParagraph(
        const std::string& paragraph,
        const std::string& next_paragraph,
        int disregard_until
    );
};

#endif //RENDER_PIPELINE_HPP
