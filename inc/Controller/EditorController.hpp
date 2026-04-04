///
/// @file: EditorController.hpp
/// @description: Organizes Data flow between Model and View
///
/// @date: 2026-01-14
/// @author: Moritz Pirer
///

#ifndef EDITOR_CONTROLLER_HPP
#define EDITOR_CONTROLLER_HPP

#include <optional>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "../View/UiHandler.hpp"
#include "../Model/EditorState.hpp"
#include "Modes/ModeManager.hpp"
#include "../Shared/Types/Scope.hpp"
#include "Services/UndoRedoManager.hpp"

#include "Services/Settings/Settings.hpp"

class EditorController {
private:
    EditorState m_state;
    ModeManager m_mode_manager;
    UiHandler m_ui_handler;
    Settings m_settings;
    UndoRedoManager m_undoRedoManager;

    std::thread m_autosaver_thread;
    static const std::chrono::seconds c_autosave_frequency;
    std::condition_variable m_is_terminating;
    std::mutex m_autosaver_lock;

    void startAutoSaveLoop(std::filesystem::path executable_path);

    /// @brief calculates what should be rendered to the screen
    RenderInfo calculateRenderInfo(ScreenSize actual_size, std::string executable_path);

    ScreenSize calculateTextAreaSize(const RenderInfo& render_info, const ScreenSize& total_size);
    
public:
    EditorController(std::optional<std::string> file_path = std::nullopt);
    virtual ~EditorController();

    /// @brief the main entry point for the editor
    void mainLoop(std::string executable_path);
};

#endif //EDITOR_CONTROLLER_HPP
