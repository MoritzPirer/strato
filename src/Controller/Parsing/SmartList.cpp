#include <regex>
#include <map>

#include "../../../inc/Controller/Parsing/SmartList.hpp"

#include "../../../inc/Controller/Actions/Structure/ParagraphSplittingAction.hpp"
#include "../../../inc/Controller/Actions/Editing/DeleteAction.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

namespace {
bool isWithinCodeBlock(ParsingContext context);
ParseResult basicParagraphSplit(Position cursor);
std::optional<ParseResult> getStaticSmartlistInsertion(ParsingContext context);
int getNumberListValue(const std::string& paragraph);
char getNumberListEnder(const std::string& paragraph);
ParseResult getLetterListInsertion(const std::string& paragraph, Position cursor);
bool canBeLetterList(const std::string& paragraph);
ParseResult getNumberListInsertion(const std::string& paragraph, Position cursor);
bool canBeNumberList(const std::string& paragraph);
std::optional<ParseResult> getDynamicSmartListInsertion(ParsingContext context);
std::optional<ParseResult> getStaticSmartlistInsertion(ParsingContext context);

bool shouldTrySmartList(ParsingContext context) {
    const std::string& file_name = context.state.getFileName();
    return ((file_name.ends_with(".md") || file_name.ends_with(".txt")) && !isWithinCodeBlock(context));
}

bool isWithinCodeBlock(ParsingContext context) {
    auto isStartOfCodeBlock = [](const std::string& paragraph) -> bool {
        return paragraph.starts_with("```");
    };

    auto isEndOfCodeBlock = [](const std::string& paragraph) -> bool {
        return paragraph.starts_with("```") && StringHelpers::containsOnlyIgnoringWhitespace(paragraph, '`');
    };

    bool is_within_code_block = false;
    for (int paragraph_index = 0; paragraph_index < context.state.getCursor().getRow(); paragraph_index++) {
        const std::string& paragraph = context.state.getParagraph(paragraph_index);
        if (is_within_code_block && isEndOfCodeBlock(paragraph)) {
            is_within_code_block = false;
        }
        else if (!is_within_code_block && isStartOfCodeBlock(paragraph)) {
            is_within_code_block = true;
        }
    }

    return is_within_code_block;
}

ParseResult basicParagraphSplit(Position cursor) {
    return {ModeType::TYPING_MODE, {std::make_shared<ParagraphSplittingAction>(cursor)}
    }; 
}

std::optional<ParseResult> getStaticSmartlistInsertion(ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();
    const std::string& current_paragraph = context.state.getParagraph(cursor.row);
    std::string until_cursor = current_paragraph.substr(0, cursor.column);

    std::map<std::string, std::string> list_types = {
        {"- [ ] ", "- [ ] "},
        {"- [x] ", "- [ ] "},
        {"- ", "- "},
        {"+ ", "+ "},
        {">", ">"}
    };

    for (const auto& [prefix, continuation] : list_types) {
        if (!StringHelpers::startsWithIgnoringWhitespace(until_cursor, prefix)) {
            continue;
        }
        
        // if the user presses enter at when the paragraph is only the prefix, the prefix is deleted
        // To escape from list when no more should be added
        if (prefix.length() == current_paragraph.length()) {
            Position delete_start = {cursor.row, 0};

            //never called for empty paragraph so underflow is not a problem
            Position delete_end = {cursor.row, static_cast<int>(current_paragraph.length()) - 1}; 
            return ParseResult{ModeType::TYPING_MODE, std::make_shared<DeleteAction>(delete_start, delete_end, cursor)};
        }
        
        size_t leading_spaces = StringHelpers::countLeadingSpaces(until_cursor);
        std::string new_paragraph_prefix = std::string(leading_spaces, ' ') + continuation;

        ParseResult result = {
            ModeType::TYPING_MODE,
            {std::make_shared<ParagraphSplittingAction>(context.state.getCursor().getPosition(), new_paragraph_prefix)}
        };

        return result; 
    }

    return std::nullopt;
}

std::optional<ParseResult> getDynamicSmartListInsertion(ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();
    const std::string& paragraph = context.state.getParagraph(cursor.row);

    if (canBeNumberList(paragraph)) {
        return getNumberListInsertion(paragraph, cursor);
    }
    if (canBeLetterList(paragraph)) {
        return getLetterListInsertion(paragraph, cursor);
    }

    return std::nullopt;
}

bool canBeNumberList(const std::string& paragraph) {
    std::regex number_list_regex("^ *[0-9]+[.)] .*");
    return std::regex_match(paragraph, number_list_regex);
}

ParseResult getNumberListInsertion(const std::string& paragraph, Position cursor) {
    int numbering_value = getNumberListValue(paragraph);
    char numbering_ender = getNumberListEnder(paragraph);

    // if the user presses enter at when the paragraph is only the prefix, the prefix is deleted
    // To escape from list when no more should be added
    if (StringHelpers::countLeadingSpaces(paragraph) + std::to_string(numbering_value).length() + 2 == paragraph.length()) {
        Position delete_start = {cursor.row, 0};

        //never called for empty paragraph so underflow is not a problem
        Position delete_end = {cursor.row, static_cast<int>(paragraph.length()) - 1}; 

        return ParseResult{ModeType::TYPING_MODE, std::make_shared<DeleteAction>(delete_start, delete_end, cursor)};
    }

    std::string continuation = std::string(StringHelpers::countLeadingSpaces(paragraph), ' ')
        + std::to_string(numbering_value + 1)
        + numbering_ender
        + " ";
    
    ParseResult result = {
        ModeType::TYPING_MODE,
        {std::make_shared<ParagraphSplittingAction>(cursor, continuation)}
    };

    return result;
}

bool canBeLetterList(const std::string& paragraph) {
    std::regex number_list_regex("^ *[a-zA-Z][.)] .*");
    return std::regex_match(paragraph, number_list_regex);
}

ParseResult getLetterListInsertion(const std::string& paragraph, Position cursor) {
    char numbering_value = *StringHelpers::firstNonSpace(paragraph); 
    char numbering_ender = getNumberListEnder(paragraph);

    // if the user presses enter at when the paragraph is only the prefix, the prefix is deleted
    // To escape from list when no more should be added
    const int c_letter_list_length = 3; // letter, ender, space
    if (StringHelpers::countLeadingSpaces(paragraph) + c_letter_list_length == paragraph.length()) {
        Position delete_start = {cursor.row, 0};

        //never called for empty paragraph so underflow is not a problem
        Position delete_end = {cursor.row, static_cast<int>(paragraph.length()) - 1}; 

        return ParseResult{ModeType::TYPING_MODE, std::make_shared<DeleteAction>(delete_start, delete_end, cursor)};
    }

    if (numbering_value == 'z' || numbering_value == 'Z') {
        // numbering_value -= ('Z' - 'A' - 1);
        return basicParagraphSplit(cursor);
    }

    std::string continuation = std::string(StringHelpers::countLeadingSpaces(paragraph), ' ')
        + std::string(1, numbering_value + 1)
        + numbering_ender
        + " ";
    
    ParseResult result = {
        ModeType::TYPING_MODE,
        {std::make_shared<ParagraphSplittingAction>(cursor, continuation)}
    };

    return result;
}

int getNumberListValue(const std::string& paragraph) {
    std::string after_whitespace = paragraph.substr(paragraph.find_first_not_of(' '));
    std::string number_only = after_whitespace.substr(0, StringHelpers::findFirstNonDigit(after_whitespace));

    return std::stoi(number_only);
}

char getNumberListEnder(const std::string& paragraph) {
    size_t dot = paragraph.find_first_of('.');
    size_t parentheses = paragraph.find_first_of(')');

    if (dot == std::string::npos && parentheses == std::string::npos) {
        throw std::invalid_argument("canBeNumberList() matched an illegal paragraph!");
    }

    if (dot == std::string::npos) {
        return ')';
    }

    if (parentheses == std::string::npos) {
        return '.';
    }

    return (dot < parentheses? '.' : ')');
}
} // anonymous namespace

ParseResult SmartList::trySmartListInsertion(ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();
    if (!shouldTrySmartList(context)) {
        return basicParagraphSplit(cursor);
    }

    std::optional<ParseResult> result = getStaticSmartlistInsertion(context);
    if (result.has_value()) {
        return *result;
    }
    
    result = getDynamicSmartListInsertion(context);
    if (result.has_value()) {
        return *result;
    }

    return basicParagraphSplit(cursor);
}