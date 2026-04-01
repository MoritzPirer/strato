#include <algorithm>

#include "../../../inc/Controller/Rendering/InterpreterDispatcher.hpp"
#include "../../../inc/Controller/Rendering/CodeInterpreter.hpp"
#include "../../../inc/Controller/Rendering/MarkdownInterpreter.hpp"
#include "../../../inc/Controller/Rendering/NullInterpreter.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

using std::shared_ptr;

InterpreterDispatcher::InterpreterDispatcher(): 
    m_interpreters{{std::make_shared<CodeInterpreter>(CodeHighlightInfo::testPython())}} {}

InterpreterDispatcher& InterpreterDispatcher::instance() {
    static InterpreterDispatcher m_instance;
    return m_instance;
}

shared_ptr<Interpreter> InterpreterDispatcher::getInterpreter(const std::string& paragraph, const std::string& file_extension) {
    if (!file_extension.ends_with("md")) {
        auto interpreter = getInterpreterByExtension(file_extension);
        if (interpreter.has_value()) {
            return *interpreter;
        }

        return std::make_shared<NullInterpreter>();
    }

    const std::string after_indicator = paragraph.substr(c_code_block_indicator.length());

    if (!paragraph.starts_with(c_code_block_indicator)) {
        return std::make_shared<MarkdownInterpreter>();
    }

    if (StringHelpers::consistsOnlyOf(after_indicator, ' ')) {
        return std::make_shared<MarkdownInterpreter>();
    }

    auto interpreter = getInterpreterByKeyword(after_indicator);
    if (interpreter.has_value()) {
        return *interpreter;
    }

    return std::make_shared<NullInterpreter>();
}

std::optional<shared_ptr<Interpreter>> InterpreterDispatcher::getInterpreterByExtension(const std::string& extension) {
    for (std::shared_ptr<CodeInterpreter> current : m_interpreters) {
        for (const std::string& language_extension : current->getHighlightInfo().m_file_extensions) {
            if (StringHelpers::caselessEquals(extension, language_extension)) {
                return current;
            }
        }
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<Interpreter>> InterpreterDispatcher::getInterpreterByKeyword(const std::string& keyword) {
    for (std::shared_ptr<CodeInterpreter> current : m_interpreters) {
        for (const std::string& language_name : current->getHighlightInfo().m_language_names) {
            if (StringHelpers::caselessEquals(keyword, language_name)) {
                return current;
            }
        }
    }

    return std::nullopt;
}

bool InterpreterDispatcher::isMarkdownCodeBlockFence(const std::string& paragraph) {
    return (paragraph.starts_with(c_code_block_indicator)
        && std::ranges::count(paragraph, c_code_block_indicator.at(0)) == static_cast<int>(c_code_block_indicator.length()));
}