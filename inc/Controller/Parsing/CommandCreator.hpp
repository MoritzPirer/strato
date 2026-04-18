///
/// @file: CommandCreator.hpp
/// @description: Creates a ParseResult based on a CommandDetails struct populated by user input
///
/// @date: 2026-02-18
/// @author: Moritz Pirer
///

#ifndef COMMAND_CREATOR_HPP
#define COMMAND_CREATOR_HPP

#include "../../Shared/DataFlow/ParseResult.hpp"
#include "ParsingContext.hpp"
#include "../Actions/Options/Case.hpp"
#include "../Actions/Options/EndBehavior.hpp"
#include "CommandDetails.hpp"

class CommandCreator {
private:

    std::string m_expression_delimiters = " \t";
    std::string m_word_delimiters = " \t!\"$%&/()=?[]|{}`+*#'-_.:,;<>";

    std::optional<CommandDetails> m_previous_details;

    ParseResult generateCharacterwiseMove(CommandDetails details, ScreenSize text_area_size);
    ParseResult generateSpanMove(CommandDetails details, ParsingContext context, EndBehavior end_behavior);

    ParseResult generateCaseSetCommand(CommandDetails details, ParsingContext context, Case target_case);
    ParseResult generateFileCommand(CommandDetails details, const Settings& settings);

    ParseResult generateParagraphCreationCommand(CommandDetails details, ParsingContext context);
    ParseResult generateParagraphJoinCommand(ParsingContext context);
    ParseResult generateParagraphSplitCommand(ParsingContext context);
    
    ParseResult generateDeleteSingleCommand(CommandDetails details, ParsingContext context);
    ParseResult generateDeleteWithinCommand(CommandDetails details, ParsingContext context);
    ParseResult generateDeleteUntilCommand(CommandDetails details, ParsingContext context);
    ParseResult generateReplaceCommand(CommandDetails details, ParsingContext context);

    ParseResult generateCopyWithinCommand(CommandDetails details, ParsingContext context);
    ParseResult generateCopyUntilCommand(CommandDetails details, ParsingContext context);
    ParseResult gerneratePasteCommand(CommandDetails details, ParsingContext context);

    ParseResult generateIndentCommand(ParsingContext context);
    ParseResult generateUnindentCommand(ParsingContext context);

    ParseResult generateHint(CommandDetails details);
    std::string getAntiDelimiter(char delimiter);
    ParseResult generateHelp(CommandDetails details);
    void generateOperatorHelp(CommandDetails details, std::vector<std::string>& message);
    void generateScopeHelp(CommandDetails details, std::vector<std::string>& message);
    void generateRangeHelp(CommandDetails details, std::vector<std::string>& message);
    std::shared_ptr<CompoundAction> generateHelpAction(const std::vector<std::string>& message);

public:
    CommandCreator() = default;
    CommandCreator(const CommandCreator&) = default;
    ~CommandCreator() = default;

    ParseResult generateActions(std::optional<CommandDetails> details, ParsingContext context);
};

#endif //COMMAND_CREATOR_HPP
