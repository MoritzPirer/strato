#include "../../../../inc/Controller/Actions/Structure/ParagraphSplittingAction.hpp"
#include "../../../../inc/Controller/Actions/ExecutionContext.hpp"

ParagraphSplittingAction::ParagraphSplittingAction(Position first_after_split, const std::string& new_line_prefix):
    c_first_after_split{first_after_split},
    c_new_line_prefix{new_line_prefix}
    {}

void ParagraphSplittingAction::apply(ExecutionContext& context) {
    context.state.splitAt(c_first_after_split);

    int row_offset = 1;
    Position move_to = {c_first_after_split.row + row_offset, static_cast<int>(c_new_line_prefix.length())};

    if (!c_new_line_prefix.empty()) {
        context.state.insertLines({c_new_line_prefix}, {c_first_after_split.row + 1, 0});
    }

    context.state.moveCursorTo(move_to);

    context.state.requestBackup();
}

void ParagraphSplittingAction::undo(EditorState& state) {
    if (!c_new_line_prefix.empty()) {
        Position start = {c_first_after_split.row + 1, 0};
        Position end = {c_first_after_split.row + 1, static_cast<int>(c_new_line_prefix.length()) - 1};
        state.deleteRange(start, end);
    }

    state.joinNextParagraphTo(c_first_after_split.row);
    state.moveCursorTo(c_first_after_split);

    state.requestBackup();
}

bool ParagraphSplittingAction::canBeUndone() const {
    return true;
}