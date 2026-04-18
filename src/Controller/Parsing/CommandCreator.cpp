#include <unordered_map>
#include <format>
#include <functional>

#include "../../../inc/Controller/Parsing/CommandCreator.hpp"
#include "../../../inc/Controller/Parsing/CommandDetails.hpp"
#include "../../../inc/Controller/Parsing/ParsingContext.hpp"
#include "../../../inc/Controller/Services/Settings/Settings.hpp"

#include "../../../inc/Controller/Actions/Movement/CharwiseMoveAction.hpp"
#include "../../../inc/Controller/Actions/Movement/FixedPositionMoveAction.hpp"
#include "../../../inc/Controller/Actions/Movement/SpanMoveAction.hpp"

#include "../../../inc/Controller/Actions/System/CompoundAction.hpp"
#include "../../../inc/Controller/Actions/System/SaveAction.hpp"
#include "../../../inc/Controller/Actions/System/QuitAction.hpp"
#include "../../../inc/Controller/Actions/System/NotifyAction.hpp"
#include "../../../inc/Controller/Actions/System/UndoAction.hpp"
#include "../../../inc/Controller/Actions/System/RedoAction.hpp"
#include "../../../inc/Controller/Actions/System/CopyAction.hpp"

#include "../../../inc/Controller/Actions/Editing/InsertAction.hpp"
#include "../../../inc/Controller/Actions/Editing/DeleteAction.hpp"
#include "../../../inc/Controller/Actions/Editing/IndentAction.hpp"
#include "../../../inc/Controller/Actions/Editing/UnindentAction.hpp"
#include "../../../inc/Controller/Actions/Editing/CaseSetAction.hpp"

#include "../../../inc/Controller/Actions/Structure/ParagraphSplittingAction.hpp"
#include "../../../inc/Controller/Actions/Structure/ParagraphJoiningAction.hpp"

#include "../../../inc/Controller/Parsing/SpanResolver.hpp"

using std::make_shared;
typedef std::vector<std::shared_ptr<Action>> ActionList;

ParseResult CommandCreator::generateActions(std::optional<CommandDetails> details, ParsingContext context) {
    if (!details.has_value() ) {
        return ParseResult::nullObject();
    }

    if (!details->is_complete) {
        return generateHint(*details);
    }

    std::unordered_map<Operator, std::function<ParseResult(CommandDetails command_details)>> generators = {
        {Operator::SWITCH_MODE, [&](CommandDetails) { return ParseResult{ModeType::TYPING_MODE, std::nullopt}; }},
        {Operator::MOVE_BY_CHARACTER, [&](CommandDetails command_details) { return generateCharacterwiseMove(command_details, context.text_area_size); }},
        {Operator::MOVE_TO_FIND, [&](CommandDetails command_details) { return generateSpanMove(command_details, context, EndBehavior::STOP_ON_END); }},
        {Operator::MOVE_TO_END, [&](CommandDetails command_details) { return generateSpanMove(command_details, context, EndBehavior::STOP_BEFORE_END); }},
        {Operator::MOVE_TO_NEXT, [&](CommandDetails command_details) { return generateSpanMove(command_details, context, EndBehavior::STOP_AFTER_END); }},

        {Operator::CASE_SET_LOWER, [&](CommandDetails command_details) { return generateCaseSetCommand(command_details, context, Case::LOWER_CASE); }},
        {Operator::CASE_SET_UPPER, [&](CommandDetails command_details) { return generateCaseSetCommand(command_details, context, Case::UPPER_CASE); }},

        {Operator::FILE_ACTION, [&](CommandDetails command_details) { return generateFileCommand(command_details, context.settings); }},

        {Operator::PARAGRAPH_CREATE, [&](CommandDetails command_details) { return generateParagraphCreationCommand(command_details, context); }},
        {Operator::PARAGRAPH_JOIN, [&](CommandDetails) { return generateParagraphJoinCommand(context); }},
        {Operator::PARAGRAPH_SPLIT, [&](CommandDetails) { return generateParagraphSplitCommand(context); }},

        {Operator::DELETE_SINGLE, [&](CommandDetails command_details) { return generateDeleteSingleCommand(command_details, context); }},
        {Operator::DELETE_WITHIN, [&](CommandDetails command_details) { return generateDeleteWithinCommand(command_details, context); }},
        {Operator::DELETE_UNTIL, [&](CommandDetails command_details) { return generateDeleteUntilCommand(command_details, context); }},
        {Operator::REPLACE, [&](CommandDetails command_details) { return generateReplaceCommand(command_details, context); }},
        
        {Operator::COPY_WITHIN, [&](CommandDetails command_details) { return generateCopyWithinCommand(command_details, context); }},
        {Operator::COPY_UNTIL, [&](CommandDetails command_details) { return generateCopyUntilCommand(command_details, context); }},
        {Operator::PASTE, [&](CommandDetails command_details) { return gerneratePasteCommand(command_details, context); }},

        {Operator::INDENT, [&](CommandDetails) { return generateIndentCommand(context); }},
        {Operator::UNINDENT, [&](CommandDetails) { return generateUnindentCommand(context); }},

        {Operator::UNDO, [&](CommandDetails) { return ParseResult{ModeType::TOOL_MODE, make_shared<UndoAction>()}; }},
        {Operator::REDO, [&](CommandDetails) { return ParseResult{ModeType::TOOL_MODE, make_shared<RedoAction>()}; }},
        {Operator::HELP, [&](CommandDetails command_details) { return generateHelp(command_details); }}
    };

    if (details->operator_type == Operator::REPEAT) {
        if (m_previous_details.has_value()) {
            return generators.at(m_previous_details->operator_type)(*m_previous_details);
        }
        else {
            return {std::nullopt, make_shared<NotifyAction>("Nothing to repeat!")};
        }
    }


    if (generators.contains(details->operator_type)) {
        ParseResult result = generators.at(details->operator_type)(*details);
        if (result.action.has_value() && (*result.action)->canBeUndone()) {
            m_previous_details = *details; 
        }

        return result;
    }

    return ParseResult::nullObject();
}

ParseResult CommandCreator::generateHint(CommandDetails details) {
    std::string scope_range_hint = "Enter a scope or range!";
    
    std::unordered_map<Operator, std::string> hints = {
        {Operator::COPY_UNTIL, "Enter the end of the section to copy!"},
        {Operator::COPY_WITHIN, "Enter a scope or range to copy!"},
        {Operator::DELETE_UNTIL, "Enter the end of the section to delete!"},
        {Operator::DELETE_WITHIN, "Enter a scope or range to delete!"},
        {Operator::REPLACE, "Enter a character to replace the selected character!"},
        {Operator::MOVE_TO_END, scope_range_hint},
        {Operator::MOVE_TO_NEXT, scope_range_hint},
        {Operator::MOVE_TO_FIND, "Enter a character to find!"},
        {Operator::CASE_SET_UPPER, "Enter a scope or range to set to uppercase!"},
        {Operator::CASE_SET_LOWER, "Enter a scope or range to set to lowercase!"},
        {Operator::FILE_ACTION, "x to force quit, q to quit safely, Q to save and quit, s to save"},
        {Operator::HELP, "Enter a character to get information about!"}
    }; 
    
    if (hints.contains(details.operator_type)) {
        return {std::nullopt, make_shared<NotifyAction>(hints.at(details.operator_type))};
    }
    
    return ParseResult::nullObject();
}

ParseResult CommandCreator::generateCharacterwiseMove(CommandDetails details, ScreenSize text_area_size) {
    return {ModeType::TOOL_MODE, make_shared<CharwiseMoveAction>(text_area_size, *details.direction)};
}

ParseResult CommandCreator::generateSpanMove(CommandDetails details, ParsingContext context, EndBehavior end_behavior) {
    // range or custom delimiter
    if (!details.scope.has_value()) {
        auto [start, end] = SpanResolver::fromDelimiter(context.state, {
            .delimiters = std::string(1, *(details.argument)),
            .anti_delimiters =  getAntiDelimiter(*(details.argument)),
            .end_behavior = end_behavior,
            .paragraph_is_delimiter = false
        });

        return {details.next_mode, make_shared<SpanMoveAction>(start, end, *details.direction)};
    }

    // scope given
    ScopeSettings settings = {
        .scope = *(details.scope),
        .size = context.text_area_size,
        .end_behavior = end_behavior
    };

    if (settings.scope == Scope::EXPRESSION) {
        settings.delimiters = m_expression_delimiters;
    }
    else if (settings.scope == Scope::WORD) {
        settings.delimiters = m_word_delimiters;
    }

    auto [start, end] = SpanResolver::fromScope(context.state, settings);

    return {details.next_mode, make_shared<SpanMoveAction>(start, end, *details.direction)};
}

ParseResult CommandCreator::generateCaseSetCommand(CommandDetails details, ParsingContext context, Case target_case) {
    // range or delimiter given
    if (!details.scope.has_value()) {
        auto [start, end] = SpanResolver::fromDelimiter(context.state, {
            .delimiters = std::string(1, *(details.argument)),
            .anti_delimiters = getAntiDelimiter(*(details.argument)),
            .end_behavior = EndBehavior::STOP_BEFORE_END,
            .paragraph_is_delimiter = false
        });

        return {details.next_mode, make_shared<CaseSetAction>(start, end, target_case)};
    }

    // scope given
    ScopeSettings settings = {
        .scope = *(details.scope),
        .size = context.text_area_size,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    };

    if (settings.scope == Scope::EXPRESSION) {
        settings.delimiters = m_expression_delimiters;
    }
    else if (settings.scope == Scope::WORD) {
        settings.delimiters = m_word_delimiters;
    }

    auto [start, end] = SpanResolver::fromScope(context.state, settings);

    return {details.next_mode, make_shared<CaseSetAction>(start, end, target_case)};
}

ParseResult CommandCreator::generateFileCommand(CommandDetails details, const Settings& settings) {
    SaveConfirmation confirmation = settings.isEnabled("confirm_save")?
        SaveConfirmation::YES : SaveConfirmation::NO;
        
    std::unordered_map<char, ParseResult> results = {
        {'q', {std::nullopt, make_shared<QuitAction>(QuitMode::ONLY_IF_SAVED)}},
        {'x', {std::nullopt, make_shared<QuitAction>(QuitMode::FORCE_QUIT)}},
        {'s', {std::nullopt, make_shared<SaveAction>(confirmation)}},
        {'Q', {std::nullopt, make_shared<CompoundAction>(ActionList{
            make_shared<SaveAction>(confirmation),
            make_shared<QuitAction>(QuitMode::ONLY_IF_SAVED)
        })}}
    };

    if (results.contains(*(details.argument))) {
        return results.at(*(details.argument));
    }

    std::string message = std::string(1, *(details.argument)) + " is not a known file command!";
    return {std::nullopt, make_shared<NotifyAction>(message)};
}

ParseResult CommandCreator::generateParagraphCreationCommand(CommandDetails details, ParsingContext context) {
    auto [start, end] = SpanResolver::fromScope(context.state, {
        .scope = Scope::PARAGRAPH,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    });

    if (*details.direction == Direction::LEFT) {
        return {details.next_mode, make_shared<CompoundAction>(ActionList{
            make_shared<SpanMoveAction>(start, end, *details.direction),
            make_shared<ParagraphSplittingAction>(start),
            make_shared<CharwiseMoveAction>(context.text_area_size, Direction::LEFT)
        })};
    }
    
    return {details.next_mode, make_shared<CompoundAction>(ActionList{
        make_shared<SpanMoveAction>(start, end, *details.direction),
        make_shared<ParagraphSplittingAction>(end),
    })};
}

ParseResult CommandCreator::generateParagraphJoinCommand(ParsingContext context) {
    auto [start, end] = SpanResolver::fromScope(context.state, {
        .scope = Scope::PARAGRAPH,
        .size = context.text_area_size,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    });

    return {ModeType::TOOL_MODE, make_shared<CompoundAction>(ActionList{
        make_shared<SpanMoveAction>(start, end, Direction::RIGHT),
        make_shared<ParagraphJoiningAction>(context.state.getCursor().getPosition())
    })};
}

ParseResult CommandCreator::generateParagraphSplitCommand(ParsingContext context) {
    return {ModeType::TOOL_MODE, std::make_shared<ParagraphSplittingAction>(context.state.getCursor().getPosition())};
}
ParseResult CommandCreator::generateDeleteSingleCommand(CommandDetails details, ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();
    return {details.next_mode, make_shared<DeleteAction>(cursor, cursor, cursor)};
}

ParseResult CommandCreator::generateDeleteWithinCommand(CommandDetails details, ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();

    // range or custom delimiter
    if (!details.scope.has_value()) {
        auto [start, end] = SpanResolver::fromDelimiter(context.state, {
            .delimiters = std::string(1, *(details.argument)),
            .anti_delimiters = getAntiDelimiter(*(details.argument)),
            .end_behavior = EndBehavior::STOP_BEFORE_END,
            .paragraph_is_delimiter = false
        });

        return {details.next_mode, make_shared<DeleteAction>(start, end, cursor)};
    }

    // Scope given
    ScopeSettings settings = {
        .scope = *(details.scope),
        .size = context.text_area_size,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    };

    if (settings.scope == Scope::EXPRESSION) {
        settings.delimiters = m_expression_delimiters;
    }
    else if (settings.scope == Scope::WORD) {
        settings.delimiters = m_word_delimiters;
    }

    auto [start, end] = SpanResolver::fromScope(context.state, settings);

    return {details.next_mode, make_shared<DeleteAction>(start, end, cursor)};

}

ParseResult CommandCreator::generateDeleteUntilCommand(CommandDetails details, ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();

    auto [dummy, end] = SpanResolver::fromDelimiter(context.state, {
        .delimiters = getAntiDelimiter(*(details.argument)),
        .anti_delimiters = std::string(1, *(details.argument)),
        .end_behavior = EndBehavior::STOP_BEFORE_END,
        .paragraph_is_delimiter = false
    });

    return {details.next_mode, make_shared<DeleteAction>(cursor, end, cursor)};
}

ParseResult CommandCreator::generateReplaceCommand(CommandDetails details, ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();
    return {ModeType::TOOL_MODE, make_shared<CompoundAction>(ActionList{
        make_shared<DeleteAction>(cursor, cursor, cursor),
        make_shared<InsertAction>(std::vector<std::string>{std::string(1, *details.argument)}, cursor),
        make_shared<CharwiseMoveAction>(context.text_area_size, Direction::LEFT)
    })};
}

ParseResult CommandCreator::generateCopyWithinCommand(CommandDetails details, ParsingContext context) {
    // range or custom delimiter
    if (!details.scope.has_value()) {
        auto [start, end] = SpanResolver::fromDelimiter(context.state, {
            .delimiters = std::string(1, *(details.argument)),
            .anti_delimiters = getAntiDelimiter(*(details.argument)),
            .end_behavior = EndBehavior::STOP_BEFORE_END,
            .paragraph_is_delimiter = false
        });

        return {ModeType::TOOL_MODE, make_shared<CompoundAction>(ActionList{
            make_shared<CopyAction>(start, end, CopyType::INLINE),
            make_shared<NotifyAction>("Copied content to clipboard!")
            })
        };
    }

    // Scope given
    ScopeSettings settings = {
        .scope = *(details.scope),
        .size = context.text_area_size,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    };

    CopyType type = CopyType::INLINE;
    switch (settings.scope) {
        case Scope::FILE:
        case Scope::PARAGRAPH: {
            type = CopyType::BLOCK;
            break;
        }
        case Scope::EXPRESSION: {
            settings.delimiters = m_expression_delimiters;
            break;
        }
        case Scope::WORD: {
            settings.delimiters = m_word_delimiters;
            break;
        }
        default: {
            break;
        }
    }

    auto [start, end] = SpanResolver::fromScope(context.state, settings);
    if (static_cast<size_t>(end.column) == context.state.getParagraph(end.row).length()
        && end.column != 0) {
        end.column--;
    }

    return {ModeType::TOOL_MODE, make_shared<CompoundAction>(ActionList{
        make_shared<CopyAction>(start, end, type),
        make_shared<NotifyAction>("Copied content to clipboard!")
    })};
}

ParseResult CommandCreator::generateCopyUntilCommand(CommandDetails details, ParsingContext context) {
    Position cursor = context.state.getCursor().getPosition();

    auto [dummy, end] = SpanResolver::fromDelimiter(context.state, {
        .delimiters = getAntiDelimiter(*(details.argument)),
        .anti_delimiters = std::string(1, *(details.argument)),
        .end_behavior = EndBehavior::STOP_BEFORE_END,
        .paragraph_is_delimiter = false
    });

    return {std::nullopt, make_shared<CopyAction>(cursor, end, CopyType::INLINE)};
}

ParseResult CommandCreator::gerneratePasteCommand(CommandDetails details, ParsingContext context) {
    std::optional<Clipboard> clipboard = context.state.getClipboard();
    Position cursor = context.state.getCursor().getPosition();

    if (!clipboard.has_value()) {
        return {std::nullopt, make_shared<NotifyAction>("Nothing to paste! Copy text using y or Y.")};
    }

    if (clipboard->type == CopyType::INLINE) {
        if (details.direction == Direction::LEFT) {
            return {std::nullopt, make_shared<InsertAction>(clipboard->content, cursor, true)};
        }
        
        //paste after cursor
        if (static_cast<size_t>(cursor.column) < context.state.getParagraph(cursor.row).length()) {
            cursor.column++;
        }

        return {std::nullopt, make_shared<InsertAction>(clipboard->content, cursor, true)};
    }
    
    //paste full line above / below cursor
    auto [start, end] = SpanResolver::fromScope(context.state, {
        .scope = Scope::PARAGRAPH,
        .end_behavior = EndBehavior::STOP_BEFORE_END
    });

    if (*details.direction == Direction::LEFT) {
        Position insert_position = {cursor.row, 0};

        return {details.next_mode, make_shared<CompoundAction>(ActionList{
            make_shared<SpanMoveAction>(start, end, *details.direction),
            make_shared<ParagraphSplittingAction>(start),
            make_shared<InsertAction>(clipboard->content, insert_position, true),
            make_shared<FixedPositionMoveAction>(insert_position)
        })};
    }
    
    Position insert_position = {cursor.row + 1, 0};
    
    return {details.next_mode, make_shared<CompoundAction>(ActionList{
        make_shared<SpanMoveAction>(start, end, *details.direction),
        make_shared<ParagraphSplittingAction>(end),
        make_shared<CharwiseMoveAction>(context.text_area_size, Direction::RIGHT),
        make_shared<InsertAction>(clipboard->content, insert_position, true),
        make_shared<FixedPositionMoveAction>(insert_position)
    })};
}


ParseResult CommandCreator::generateIndentCommand(ParsingContext context) {
    int cursor_row = context.state.getCursor().getRow();
    int tab_width = context.settings.getTabWidth();
    return {std::nullopt, make_shared<IndentAction>(cursor_row, tab_width)};
}

ParseResult CommandCreator::generateUnindentCommand(ParsingContext context) {
    int cursor_row = context.state.getCursor().getRow();
    int tab_width = context.settings.getTabWidth();
    return {std::nullopt, make_shared<UnindentAction>(cursor_row, tab_width)};
}

std::string CommandCreator::getAntiDelimiter(char delimiter) {
    std::unordered_map<char, std::string> indicators = {
        {'{', "}"},
        {'}', "{"},
        {'<', ">"},
        {'>', "<"},
        {'[', "]"},
        {'(', ")"},
        {')', "("},
        {']', "["},
        {'"', "\""},
        {'\'', "'"},
    };

    if (indicators.contains(delimiter)) {
        return indicators.at(delimiter);
    }

    return "";
}

ParseResult CommandCreator::generateHelp(CommandDetails details) {

    std::vector<std::string> message;
    if (message.empty()) {
        message.push_back(std::format("{} is not an operator.", *details.argument));
    }

    generateOperatorHelp(details, message);
    generateScopeHelp(details, message);
    generateRangeHelp(details, message);

    return {std::nullopt, generateHelpAction(message)};
}

void CommandCreator::generateOperatorHelp(CommandDetails details, std::vector<std::string>& message) {
    std::unordered_map<char, std::vector<std::string>> operator_helps = {
        {'m', {"MOVE - expects a scope or range, moves the cursor to the end of that scope or range."}},
        {'M', {"MOVE (REVERSE) - expects a scope or range,", "moves the cursor to the start of that scope or range."}},
        {'n', {"NEXT - expects a scope or range, moves the cursor to the start of the next occurrence of that scope or range."}}, /////////
        {'n', {"NEXT (REVERSE) - expects a scope or range,", "moves the cursor to the end of the previous occurrence of that scope or range."}}, /////////
        {'?', {"HELP - provides information about the next character pressed."}},
        {'e', {"ERASE - erases the character under the cursor."}},
        {'E', {"ERASE - erases the character under the cursor, and enters TYPING MODE."}},
        {'r', {"REPLACE - replaces the character under the cursor with the next character pressed."}},
        {'t', {"TO LOWER - epxects a scope or range, changes that scope or range to be all-lowercase."}},
        {'T', {"TO UPPER - epxects a scope or range, changes that scope or range to be all-uppercase."}},
        {'u', {"UNDO - undoes the last modifying action. Movement and file actions cannot be undone."}},
        {'U', {"REDO - re-does the most recently undone action.", "Making new changes after undoing clears the redoable actions."}},
        {'i', {"INSERT - enters TYPING MODE."}},
        {'o', {"OPEN PARAGRAPH - creates a new paragraph below the current one and moves the cursor to it.", "Switches to TYPING MODE."}},
        {'O', {"OPEN PARAGRAPH (REVERSE) - creates a new paragraph above the current one and moves the cursor to it.", "Switches to TYPING MODE."}},
        {'a', {"AT - expects a scope or range, moves the cursor to the end of that scope or range.", "Switches to TYPING MODE. Equivalent to MOVE + INSERT. (m<section>i)"}},
        {'A', {"AT (REVERSE) - expects a scope or range, moves the cursor to the start of that scope or range.", "Switches to TYPING MODE. Equivalent to MOVE (REVERSE) + INSERT. (M<section>i)"}},
        {'f', {"FIND - moves the cursor to the next occurence of the next character entered.", "If no such character exists, move to the end of the file."}},
        {'F', {"FIND - moves the cursor to the previous occurence of the next character entered.", "If no such character exists, move to the start of the file."}},
        {'h', {"moves the cursor one character left, or to the end of the previous line."}},
        {'j', {"moves the cursor one line down, or to the end of the current line if already in the last line."}},
        {'k', {"moves the cursor one line up, or to the start of the current line if already in the first line."}},
        {'l', {"moves the cursor one character right, or to the start of the next line."}},
        {'H', {"Equivalent to NEXT (REVERSE) with word scope (Nw)."}},
        {'J', {"Equivalent to NEXT with paragraph scope (np)."}},
        {'K', {"Equivalent to NEXT (REVERSE) with paragraph scope (Np)."}},
        {'L', {"Equivalent to NEXT with word scope (nw)."}},
        {'d', {"DELETE SECTION - expects a scope or range. That section of text is deleted."}},
        {'D', {"DELETE UNTIL - deletes from the current cursor position to the next occurence of the next character entered."}},
        {'c', {"CHANGE SECTION - expects a scope or range. That section of text is deleted.", "Switchers to TYPING MODE. Equivalent to DELETE SECTION + INSERT (d<section>i)"}},
        {'C', {"CHANGE UNTIL - deletes from the current cursor position to the next occurence of the next character entered.",  "Switchers to TYPING MODE. Equivalent to DELETE UNTIL + INSERT (D<section>i)"}},


        //dD, +- yY  !
    };

    if (operator_helps.contains(*details.argument)) {
        message.push_back(std::format(
            "{}: {}", 
            *details.argument, operator_helps.at(*details.argument)
        ));
    }
}

void CommandCreator::generateScopeHelp(CommandDetails details, std::vector<std::string>& message) {
    std::unordered_map<char, std::string> scope_helps = {
        {'f', "the entire file"},
        {'p', "the current paragraph"},
        {'l', "the current line"},
        {'e', "the current expression (ended by space)"},
        {'w', "the current word (ended by space or special characters)"},
    };

    if (scope_helps.contains(*details.argument)) {
        message.push_back(std::format(
            "When used as a scope, {} the preceeding operator to {}.", 
            *details.argument, scope_helps.at(*details.argument)
        ));
    }
}

std::shared_ptr<CompoundAction> CommandCreator::generateHelpAction(const std::vector<std::string>& message) {
    ActionList list;
    list.reserve(message.size());
    for (const std::string& line : message) {
        list.push_back(std::make_shared<NotifyAction>(line));
    }

    return std::make_shared<CompoundAction>(list);
}

void CommandCreator::generateRangeHelp(CommandDetails details, std::vector<std::string>& message) {

}
