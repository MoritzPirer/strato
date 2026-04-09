#include <algorithm>

#include "../../../inc/Controller/Rendering/InterpreterDispatcher.hpp"
#include "../../../inc/Controller/Rendering/CodeInterpreter.hpp"
#include "../../../inc/Controller/Rendering/MarkdownInterpreter.hpp"
#include "../../../inc/Controller/Rendering/NullInterpreter.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"
#include "../../../inc/Controller/IO/FileHandler.hpp"

using std::shared_ptr;

InterpreterDispatcher::InterpreterDispatcher(std::string executable_path) 
{
    std::filesystem::path path = std::filesystem::absolute(executable_path).parent_path();
    
    std::filesystem::path data_folder = path / "data/codeHighlight";
    std::vector<CodeHighlightInfo> infos = FileHandler::parseAllCodeLanguageFiles(data_folder);
    for (const CodeHighlightInfo& info : infos) {
        m_interpreters.push_back(std::make_shared<CodeInterpreter>(info));
    }
}

InterpreterDispatcher& InterpreterDispatcher::instance(std::string executable_path) {
    static InterpreterDispatcher m_instance(executable_path);
    return m_instance;
}

shared_ptr<Interpreter> InterpreterDispatcher::getInterpreter(const std::string& paragraph, const std::string& file_extension) {

    if (!file_extension.ends_with("md")) {
        auto interpreter = getInterpreterByExtension(file_extension);
        if (interpreter.has_value()) {
            m_last_was_code_interpreter = true;
            return std::make_shared<CodeInterpreter>(**interpreter);
        }

        return std::make_shared<NullInterpreter>();
    }

    const std::string after_indicator = paragraph.substr(c_code_block_indicator.length());

    if (!paragraph.starts_with(c_code_block_indicator)) {
        m_last_was_code_interpreter = false;
        return std::make_shared<MarkdownInterpreter>();
    }

    if (StringHelpers::containsOnly(after_indicator, ' ')) {
        //only give out Markdown if withing code block
        if (m_last_was_code_interpreter) {
            m_last_was_code_interpreter = false;
            return std::make_shared<MarkdownInterpreter>();
        }
        else {
            m_last_was_code_interpreter = true;
            return std::make_shared<NullInterpreter>();
        }
    }

    std::optional<std::shared_ptr<CodeInterpreter>> interpreter = getInterpreterByKeyword(after_indicator);
    if (interpreter.has_value()) {
        m_last_was_code_interpreter = true;
        return std::make_shared<CodeInterpreter>(**interpreter);
    }

    m_last_was_code_interpreter = true;
    return std::make_shared<NullInterpreter>();
}

std::optional<shared_ptr<CodeInterpreter>> InterpreterDispatcher::getInterpreterByExtension(const std::string& extension) {
    for (std::shared_ptr<CodeInterpreter> current : m_interpreters) {
        for (const std::string& language_extension : current->getHighlightInfo().m_file_extensions) {
            if (StringHelpers::caselessEquals(extension, language_extension)) {
                return current;
            }
        }
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<CodeInterpreter>> InterpreterDispatcher::getInterpreterByKeyword(const std::string& keyword) {
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