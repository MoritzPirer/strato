///
/// @file: InterpreterDispatcher.hpp
/// @description: description
///
/// @date: 2026-03-31
/// @author: Moritz Pirer
///

#ifndef INTERPRETER_DISPATCHER_HPP
#define INTERPRETER_DISPATCHER_HPP

#include <memory>
#include <optional>
#include <vector>

#include "Interpreter.hpp"
#include "CodeInterpreter.hpp"

class InterpreterDispatcher {
private:
    const std::string c_code_block_indicator = "```";
    bool m_last_was_code_interpreter = false;

    std::vector<std::shared_ptr<CodeInterpreter>> m_interpreters;
    
    std::optional<std::shared_ptr<CodeInterpreter>> getInterpreterByKeyword(const std::string& keyword);

public:
    InterpreterDispatcher(std::string executable_path);
    InterpreterDispatcher(const InterpreterDispatcher&) = default;
    ~InterpreterDispatcher() = default;

    static InterpreterDispatcher& instance(std::string executable_path = "");

    /// @brief returns an interpreter subclass based on the CURRENT LINE. Calling this function in the biddle of a code
    ///     block might have unintended effects
    /// @return 
    std::shared_ptr<Interpreter> getInterpreter(const std::string& paragraph, const std::string& file_extension);

    std::optional<std::shared_ptr<CodeInterpreter>> getInterpreterByExtension(const std::string& extension);

    bool isMarkdownCodeBlockFence(const std::string& paragraph);

};

#endif //INTERPRETER_DISPATCHER_HPP
