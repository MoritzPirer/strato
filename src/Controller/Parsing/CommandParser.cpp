#include <unordered_map>
#include <functional>

#include "../../../inc/Controller/Parsing/CommandParser.hpp"
#include "../../../inc/Controller/Parsing/CommandDetails.hpp"
#include "../../../inc/Controller/Parsing/CommandCreator.hpp"


ParseResult CommandParser::parseInput(char input, ParsingContext context) {
    m_details.has_value()? parseAsParameter(input) : parseAsOperator(input);
    
    ParseResult result = m_creator.generateActions(m_details, context);
    if (m_details.has_value() && m_details->is_complete) {
        m_details = std::nullopt;
    }

    return result;
}

void CommandParser::parseAsOperator(char input) {
    // operators requiring only one input
    std::unordered_map<char, CommandDetails> simple_commands = {
        {'h', {
            .operator_type = Operator::MOVE_BY_CHARACTER,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'j', {
            .operator_type = Operator::MOVE_BY_CHARACTER,
            .direction = Direction::DOWN,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'k', {
            .operator_type = Operator::MOVE_BY_CHARACTER,
            .direction = Direction::UP,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'l', {
            .operator_type = Operator::MOVE_BY_CHARACTER,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'i', {
            .operator_type = Operator::SWITCH_MODE,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'e', {
            .operator_type = Operator::DELETE_SINGLE,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'E', {
            .operator_type = Operator::DELETE_SINGLE,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'o', {
            .operator_type = Operator::PARAGRAPH_CREATE,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'O', {
            .operator_type = Operator::PARAGRAPH_CREATE,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'+', {
            .operator_type = Operator::PARAGRAPH_JOIN
        }},
        {'-', {
            .operator_type = Operator::PARAGRAPH_SPLIT
        }},
        {'>', {
            .operator_type = Operator::INDENT,
        }},
        {'<', {
            .operator_type = Operator::UNINDENT,
        }},
        {'u', {
            .operator_type = Operator::UNDO,
        }},
        {'U', {
            .operator_type = Operator::REDO,
        }},
        {'p', {
            .operator_type = Operator::PASTE,
            .direction = Direction::RIGHT
        }},
        {'P', {
            .operator_type = Operator::PASTE,
            .direction = Direction::LEFT
        }},
        {'.', {
            .operator_type = Operator::REPEAT
        }},
        {'H', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::LEFT,
            .scope = Scope::WORD,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'J', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::RIGHT,
            .scope = Scope::PARAGRAPH,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'K', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::LEFT,
            .scope = Scope::PARAGRAPH,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'L', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::RIGHT,
            .scope = Scope::WORD,
            .next_mode = ModeType::TOOL_MODE
        }},
    };

    if (simple_commands.contains(input)) {
        m_details = simple_commands.at(input);
        m_details->is_complete = true;
        return;
    }

    //operators requiring a second input
    std::unordered_map<char, CommandDetails> compound_commands = {
        {'m', {
            .operator_type = Operator::MOVE_TO_END,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'M', {
            .operator_type = Operator::MOVE_TO_END,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'n', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'N', {
            .operator_type = Operator::MOVE_TO_NEXT,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'a', {
            .operator_type = Operator::MOVE_TO_END,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'A', {
            .operator_type = Operator::MOVE_TO_END,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TYPING_MODE
        }},
        {'!', {
            .operator_type = Operator::FILE_ACTION
        }},
        {'f', {
            .operator_type = Operator::MOVE_TO_FIND,
            .direction = Direction::RIGHT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'F', {
            .operator_type = Operator::MOVE_TO_FIND,
            .direction = Direction::LEFT,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'r', {
            .operator_type = Operator::REPLACE
        }},
        {'t', {
            .operator_type = Operator::CASE_SET_LOWER,
        }},
        {'T', {
            .operator_type = Operator::CASE_SET_UPPER,
        }},
        {'d', {
            .operator_type = Operator::DELETE_WITHIN,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'c', {
            .operator_type = Operator::DELETE_WITHIN,
            .next_mode = ModeType::TYPING_MODE

        }},
        {'D', {
            .operator_type = Operator::DELETE_UNTIL,
            .next_mode = ModeType::TOOL_MODE
        }},
        {'C', {
            .operator_type = Operator::DELETE_UNTIL,
            .next_mode = ModeType::TYPING_MODE

        }},
        {'y', {
            .operator_type = Operator::COPY_WITHIN
        }},
        {'Y', {
            .operator_type = Operator::COPY_UNTIL
        }},
        {'?', {
            .operator_type = Operator::HELP
        }}

    };

    if (compound_commands.contains(input)) {
        m_details = compound_commands.at(input);
        m_details->is_complete = false;
        return;
    }
    
    m_details->is_complete = false;
    m_details = std::nullopt;
}

void CommandParser::parseAsParameter(char input) {
    //only need to care about compund commands here

    auto parseScopeOrRange = [&] () -> void {
        std::optional<Scope> scope = charToScope(input);
        if (scope.has_value()) {
            m_details->scope = scope;
            m_details->is_complete = true;
        }
        else if (isRangeIndicator(input)) {
            m_details->argument = input;
            m_details->is_complete = true;
        }
        else {
            m_details = std::nullopt;
            m_details->is_complete = false;
        }
    };

    auto parseArgument = [&] () -> void {
        m_details->argument = input;
        m_details->is_complete = true;
    };

    std::unordered_map<Operator, std::function<void(void)>> parse_styles = {
        {Operator::DELETE_WITHIN, parseScopeOrRange},
        {Operator::DELETE_UNTIL, parseArgument},
        {Operator::REPLACE, parseArgument},
        {Operator::COPY_WITHIN, parseScopeOrRange},
        {Operator::COPY_UNTIL, parseArgument},
        {Operator::CASE_SET_LOWER, parseScopeOrRange},
        {Operator::CASE_SET_UPPER, parseScopeOrRange},
        {Operator::MOVE_TO_END, parseScopeOrRange},
        {Operator::MOVE_TO_NEXT, parseScopeOrRange},
        {Operator::MOVE_TO_FIND, parseArgument},
        {Operator::FILE_ACTION, parseArgument},
        {Operator::HELP, parseArgument},
    };

    if (parse_styles.contains(m_details->operator_type)) {
        parse_styles.at(m_details->operator_type)();
    }
    else {
        m_details = std::nullopt;
    }
}

bool CommandParser::isRangeIndicator(char c) {
    return (std::string("\"<[({'").find(c) != std::string::npos);
}

std::optional<Scope> CommandParser::charToScope(char c) {
    std::unordered_map<char, Scope> scopes = {
        {'w', Scope::WORD},
        {'e', Scope::EXPRESSION},
        {'l', Scope::LINE},
        {'p', Scope::PARAGRAPH},
        {'f', Scope::FILE},
    };
    
    if (scopes.contains(c)) {
        return scopes.at(c);
    }

    return std::nullopt;
} 