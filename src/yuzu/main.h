// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <unordered_map>
#include <QMainWindow>
#include <QTimer>
#include "core/core.h"
#include "ui_main.h"
#include "yuzu/hotkeys.h"

class Config;
class EmuThread;
class GameList;
class GImageInfo;
class GraphicsBreakPointsWidget;
class GraphicsSurfaceWidget;
class GRenderWindow;
class MicroProfileDialog;
class ProfilerWidget;
class WaitTreeWidget;
enum class GameListOpenTarget;

namespace FileSys {
class VfsFilesystem;
}

namespace Tegra {
class DebugContext;
}

class GMainWindow : public QMainWindow {
    Q_OBJECT

    /// Max number of recently loaded items to keep track of
    static const int max_recent_files_item = 10;

    // TODO: Make use of this!
    enum {
        UI_IDLE,
        UI_EMU_BOOTING,
        UI_EMU_RUNNING,
        UI_EMU_STOPPING,
    };

public:
    void filterBarSetChecked(bool state);
    void UpdateUITheme();
    GMainWindow();
    ~GMainWindow() override;

signals:

    /**
     * Signal that is emitted when a new EmuThread has been created and an emulation session is
     * about to start. At this time, the core system emulation has been initialized, and all
     * emulation handles and memory should be valid.
     *
     * @param emu_thread Pointer to the newly created EmuThread (to be used by widgets that need to
     *      access/change emulation state).
     */
    void EmulationStarting(EmuThread* emu_thread);

    /**
     * Signal that is emitted when emulation is about to stop. At this time, the EmuThread and core
     * system emulation handles and memory are still valid, but are about become invalid.
     */
    void EmulationStopping();

    // Signal that tells widgets to update icons to use the current theme
    void UpdateThemedIcons();

private:
    void InitializeWidgets();
    void InitializeDebugWidgets();
    void InitializeRecentFileMenuActions();
    void InitializeHotkeys();

    void SetDefaultUIGeometry();
    void RestoreUIState();

    void ConnectWidgetEvents();
    void ConnectMenuEvents();

    bool SupportsRequiredGLExtensions();
    bool LoadROM(const QString& filename);
    void BootGame(const QString& filename);
    void ShutdownGame();

    void ShowCallouts();

    /**
     * Stores the filename in the recently loaded files list.
     * The new filename is stored at the beginning of the recently loaded files list.
     * After inserting the new entry, duplicates are removed meaning that if
     * this was inserted from \a OnMenuRecentFile(), the entry will be put on top
     * and remove from its previous position.
     *
     * Finally, this function calls \a UpdateRecentFiles() to update the UI.
     *
     * @param filename the filename to store
     */
    void StoreRecentFile(const QString& filename);

    /**
     * Updates the recent files menu.
     * Menu entries are rebuilt from the configuration file.
     * If there is no entry in the menu, the menu is greyed out.
     */
    void UpdateRecentFiles();

    /**
     * If the emulation is running,
     * asks the user if he really want to close the emulator
     *
     * @return true if the user confirmed
     */
    bool ConfirmClose();
    bool ConfirmChangeGame();
    void closeEvent(QCloseEvent* event) override;

private slots:
    void OnStartGame();
    void OnPauseGame();
    void OnStopGame();
    /// Called whenever a user selects a game in the game list widget.
    void OnGameListLoadFile(QString game_path);
    void OnGameListOpenFolder(u64 program_id, GameListOpenTarget target);
    void OnGameListNavigateToGamedbEntry(
        u64 program_id,
        std::unordered_map<std::string, std::pair<QString, QString>>& compatibility_list);
    void OnMenuLoadFile();
    void OnMenuLoadFolder();
    void OnMenuInstallToNAND();
    /// Called whenever a user selects the "File->Select Game List Root" menu item
    void OnMenuSelectGameListRoot();
    void OnMenuRecentFile();
    void OnConfigure();
    void OnAbout();
    void OnToggleFilterBar();
    void OnDisplayTitleBars(bool);
    void ToggleFullscreen();
    void ShowFullscreen();
    void HideFullscreen();
    void ToggleWindowMode();
    void OnCoreError(Core::System::ResultStatus, std::string);

private:
    void UpdateStatusBar();

    Ui::MainWindow ui;

    std::shared_ptr<Tegra::DebugContext> debug_context;

    GRenderWindow* render_window;
    GameList* game_list;

    // Status bar elements
    QLabel* message_label = nullptr;
    QLabel* emu_speed_label = nullptr;
    QLabel* game_fps_label = nullptr;
    QLabel* emu_frametime_label = nullptr;
    QTimer status_bar_update_timer;

    std::unique_ptr<Config> config;

    // Whether emulation is currently running in yuzu.
    bool emulation_running = false;
    std::unique_ptr<EmuThread> emu_thread;
    // The path to the game currently running
    QString game_path;

    // FS
    std::shared_ptr<FileSys::VfsFilesystem> vfs;

    // Debugger panes
    ProfilerWidget* profilerWidget;
    MicroProfileDialog* microProfileDialog;
    GraphicsBreakPointsWidget* graphicsBreakpointsWidget;
    GraphicsSurfaceWidget* graphicsSurfaceWidget;
    WaitTreeWidget* waitTreeWidget;

    QAction* actions_recent_files[max_recent_files_item];

    // stores default icon theme search paths for the platform
    QStringList default_theme_paths;

    HotkeyRegistry hotkey_registry;

protected:
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
};
