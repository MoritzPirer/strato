#include "../../inc/Controller/EditorController.hpp"
#include "../../inc/Controller/IO/FileHandler.hpp"
#include "../../inc/Shared/Utils/StringHelpers.hpp"
#include "../../inc/Controller/Rendering/Renderer.hpp"
#include "../../inc/Controller/Actions/ExecutionContext.hpp"

using std::vector, std::string;

const std::chrono::seconds EditorController::c_autosave_frequency = std::chrono::minutes(5);

EditorController::EditorController(std::optional<string> file_path):
    m_state(file_path.has_value() && !file_path->empty()?
    FileHandler::openFile(file_path.value()) :
    FileHandler::createFile(FileHandler::getDefaultName())),
    m_mode_manager{ModeType::TOOL_MODE},
    m_settings{}
    {}

EditorController::~EditorController() {
    m_is_terminating.notify_all();

    if (m_autosaver_thread.joinable()) {
        m_autosaver_thread.join();
    }
}

void EditorController::startAutoSaveLoop(std::filesystem::path executable_path) {
    if (m_state.getSaveState() == SaveState::SAVED) {
        m_state.registerBackup();
    } 
    else {
        m_state.requestBackup();
    }


    std::filesystem::path backup_directory = FileHandler::createBackupLocation(executable_path);
    std::filesystem::path backup_path = FileHandler::getBackupPath(m_state.getFile().getFilepath(), backup_directory);

    m_autosaver_thread = std::thread([this, backup_path]() {
        while (!m_state.getIsQuit()) {
            std::unique_lock<std::mutex> lock(m_autosaver_lock);
            m_is_terminating.wait_for(lock, c_autosave_frequency, [this] { return m_state.getIsQuit(); });

            if (m_state.getIsQuit()) {
                return;
            }

            if (m_state.needsBackup()) {
                continue;
            }

            TextFile file_copy = m_state.getFile();
            file_copy.setFilepath(backup_path);
            FileHandler::saveFile(file_copy);
            m_state.registerBackup();
        }
    });
}

RenderInfo EditorController::calculateRenderInfo(ScreenSize actual_size, std::string executable_path) {
    Renderer renderer(m_state, m_settings, m_mode_manager, executable_path);

    vector<vector<VisualSegment>> metadata_rows = renderer.calculateMetadataRows(actual_size);

    ScreenSize text_area_size = {
        actual_size.height - static_cast<int>(metadata_rows.size()),
        actual_size.width - renderer.calculateLineNumberWidth()
    };

    return {
        renderer.renderVisibleText(text_area_size),
        metadata_rows,
        renderer.calculateLineNumbers(text_area_size),
        renderer.calculateLineNumberWidth(),
        renderer.calculateTemporaryRows(actual_size),
        renderer.calculateScreenPositionOfCursor(text_area_size),
        m_settings.isEnabled("render_color")
    };
}

ScreenSize EditorController::calculateTextAreaSize(const RenderInfo& render_info, const ScreenSize& total_size) {
    return {
        total_size.height - render_info.getPanelRowCount(),
        total_size.width - render_info.getAsideWidth()
    };
}

void EditorController::mainLoop(std::string executable_path) {
    startAutoSaveLoop(executable_path);
    
    while (m_state.getIsQuit() == false) {
        ScreenSize total_size = m_ui_handler.screenSize();
        RenderInfo render_info = calculateRenderInfo(total_size, executable_path);
        m_ui_handler.render(render_info);
        Input input = m_ui_handler.getInput();

        m_state.clearTemporaryMessages();
        std::optional<std::shared_ptr<Action>> action = m_mode_manager.convertToAction(
            input, {
                m_state,
                total_size,
                calculateTextAreaSize(render_info, total_size),
                m_settings,
            }
        );

        ExecutionContext context = {m_state, m_undoRedoManager};

        if (action.has_value()) {
            (*action)->apply(context);
            m_undoRedoManager.add(*action);
        } 
    }
}