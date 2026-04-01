#include <algorithm>

#include "../../../inc/Controller/Rendering/NullInterpreter.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

NullInterpreter::NullInterpreter():
    Interpreter("", std::vector<std::string>{})
    {}

VisualSegment NullInterpreter::parseToken(std::string token, TextRole paragraph_role) {
    return {
        .content = token,
        .role = paragraph_role,
        .style = TextStyle::makeNormal()
    };
}

TextRole NullInterpreter::findParagraphLevelRole(const std::string& current_paragraph, const std::string& next_paragraph) {
    (void) current_paragraph;
    (void) next_paragraph;

    return TextRole::NORMAL_TEXT;
}

void NullInterpreter::reset() {
    return;
}