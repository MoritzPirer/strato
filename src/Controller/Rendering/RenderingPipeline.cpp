#include "../../../inc/Controller/Rendering/RenderPipeline.hpp"

using std::vector, std::string;

RenderPipeline::RenderPipeline(std::shared_ptr<Interpreter> interpreter, size_t m_max_width): 
    m_interpreter{interpreter},
    m_max_width{m_max_width}
    {}

vector<string> RenderPipeline::tokenize(const string& paragraph) {
    vector<string> tokens;
    string current_token;
    bool previous_was_delimiter = false;

    for (char current : paragraph) {
        if (isDelimiter(current)) {
            previous_was_delimiter = true;
            mergeIfPossible(current_token, current, tokens);
            continue;
        }
        if (previous_was_delimiter) {
            tokens.push_back(current_token);
            current_token.clear();
        }
        
        current_token += current;
        previous_was_delimiter = false;
    }

    tokens.push_back(current_token);

    return tokens;
}

bool RenderPipeline::isDelimiter(char c) {
    return m_interpreter->getDelimiters().find_first_of(c) != string::npos;
}

void RenderPipeline::mergeIfPossible(string& current_token, char current, vector<string>& tokens) {
    for (const string& group : m_interpreter->getGroupedDelimiters()) {
        if (group.starts_with(current_token + current)) {
            current_token += current;
        }
    }

    tokens.push_back(current_token);
    current_token = current;
}

vector<VisualSegment> RenderPipeline::interpret(
    const vector<string>& tokenized_paragraph,
    int disregard_until,
    TextRole paragraph_role
) {
    m_interpreter->reset();

    vector<VisualSegment> interpreted_tokens;
    interpreted_tokens.reserve(tokenized_paragraph.size());

    int ignored = 0;
    for (const string& token : tokenized_paragraph) {
        VisualSegment parsed = m_interpreter->parseToken(token, paragraph_role);
        // fully ignored
        if (ignored + static_cast<int>(parsed.content.length()) < disregard_until) {
            ignored += parsed.content.length();
            continue;
        }

        // fully used
        if (ignored >= disregard_until) {
            interpreted_tokens.push_back(parsed);
            continue;
        }

        // partially used
        VisualSegment rest = splitSegmentAt(parsed, disregard_until - ignored + 1);
        interpreted_tokens.push_back(rest);
        ignored += disregard_until - ignored + 1;
    }

    return interpreted_tokens;
}

vector<vector<VisualSegment>> RenderPipeline::restructure(const vector<VisualSegment>& interpreted_paragraph) {
vector<vector<VisualSegment>> restructured;
    vector<VisualSegment> current_line;
    int current_line_width = 0;

    for (VisualSegment token : interpreted_paragraph) {
        while (!token.content.empty()) {
            int remaining_space = m_max_width - current_line_width;

            if (token.content.length() <= static_cast<size_t>(remaining_space)) {
                current_line_width += token.content.length();
                current_line.push_back(token);
                token.content.clear(); 
            } 
            else if (remaining_space > 0) {
                VisualSegment tail = splitSegmentAt(token, remaining_space);
                
                current_line.push_back(token);
                
                restructured.push_back(current_line);
                current_line.clear();
                current_line_width = 0;
                
                token = tail;
            } 
            else {
                restructured.push_back(current_line);
                current_line.clear();
                current_line_width = 0;
            }
        }
    }

    if (!current_line.empty()) {
        VisualSegment filler = {
            .content = std::string(m_max_width - current_line_width, ' '),
            .role = current_line.back().role,
            .style = current_line.back().style
        };

        current_line.push_back(filler);
        restructured.push_back(current_line);
    }

    //add a token that mirrors the last token's style and has the width to fill the line

    return restructured;
}

VisualSegment RenderPipeline::splitSegmentAt(VisualSegment& segment, int index) {
    VisualSegment rest = segment; 
    rest.content = segment.content.substr(index);
    segment.content = segment.content.substr(0, index);

    return rest;
}

vector<vector<VisualSegment>> RenderPipeline::renderParagraph(
    const string& paragraph,
    const string& next_paragraph,
    int disregard_until
) {
    if (paragraph.empty()) {
        return {{}};
    }
    TextRole paragraph_role = m_interpreter->findParagraphLevelRole(paragraph, next_paragraph);

    vector<string> tokenized_paragraph = tokenize(paragraph);
    vector<VisualSegment> interpreted_paragraph = interpret(tokenized_paragraph, disregard_until, paragraph_role);

    vector<vector<VisualSegment>> restructured_paragraph = restructure(interpreted_paragraph);

    return restructured_paragraph;
}