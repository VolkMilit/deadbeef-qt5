#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

#include "config.h"

#include "SystemTrayIcon.h"
#include "VolumeSlider.h"
#include "SeekSlider.h"
#include "PreferencesDialog.h"

#ifdef ARTWORK_ENABLED
#include <plugins/CoverArt/CoverArtWidget.h>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);

    void loadIcons();

private:
    PreferencesDialog prefDialog{this};
    static MainWindow *instance;
    enum ActionOnClose {
        Exit = 0,
        Hide = 1,
        Minimize = 2,
    };
    
    void loadConfig();
    void saveConfig();
    
    ActionOnClose actionOnClose;
    void configureActionOnClose(bool minimizeOnClose, bool hideTrayIcon);

    Ui::MainWindow *ui;

    SystemTrayIcon *trayIcon;
    VolumeSlider volumeSlider;
    SeekSlider progressBar;
    QLabel status;

#ifdef ARTWORK_ENABLED
    CoverArtWidget coverArtWidget;
#endif

    QActionGroup orderGroup;
    QActionGroup loopingGroup;

    void createConnections();

    void createToolBars();
    void createTray();
    QMenu *createPopupMenu();

    void updateTitle(DB_playItem_t *it = NULL);
    
    void loadActions();

private slots:
    void on_actionAddURL_triggered();
    void on_actionAddAudioCD_triggered();
    void on_actionAddFiles_triggered();
    void on_actionPreferences_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAbout_triggered();
    void on_actionLoopNothing_triggered();
    void on_actionLoopTrack_triggered();
    void on_actionLoopAll_triggered();
    void on_actionShuffleOrder_triggered();
    void on_actionRandomOrder_triggered();
    void on_actionLinearOrder_triggered();
    void on_actionExit_triggered();
    void on_actionPause_triggered();
    void on_actionPrev_triggered();
    void on_actionNext_triggered();
    void on_actionStop_triggered();
    void on_actionPlay_triggered();
    void on_actionAddFolder_triggered();
    void on_actionClearAll_triggered();
    void on_actionSelectAll_triggered();
    void on_actionDeselectAll_triggered();
    void on_actionRemove_triggered();

    void on_actionSaveAsPlaylist_triggered();
    void on_actionLoadPlaylist_triggered();

    void on_actionHideMenuBar_triggered();
    void on_actionBlockToolbarChanges_triggered();

#ifdef ARTWORK_ENABLED
    void on_actionHideCoverArt_triggered(bool checked);
    void onCoverartClose();
#endif

    void trackChanged(DB_playItem_t *, DB_playItem_t *);

    void setCloseOnMinimized(bool);
    void setTrayIconHidden(bool);
    //void setTrayIconTheme(const QString &variant);
    void titleSettingChanged();
    void on_actionPlayListHeader_triggered();
    void on_actionHideTabBar_triggered();
    
    void on_deadbeefActivated();

    void updateStatusBar(DB_playItem_t *it);
};

#endif // MAINWINDOW_H
