#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QStandardItemModel>
#include <QDropEvent>
#include <QUrl>
#include <QMenu>

#include "PlayListModel.h"

#include <QTreeView>

class PlayList : public QTreeView {
    Q_OBJECT

public:
    PlayList(QWidget *parent = 0);
    
    void saveConfig();
    void loadConfig();
    
    void goToLastSelection();

    void restoreCursor();
    static void storeCursor();

    void clearPlayList();
    void insertByURLAtPosition(const QUrl &url, int position = -1);

    void toggleHeaderHidden();

signals:
    void enterRelease(QModelIndex);

private:
    QPoint startPos;
    QMenu headerContextMenu;
    QByteArray headerState;
    QAction *lockColumnsAction;

    void createConnections();
    void createContextMenu();
    void createHeaderContextMenu();

    bool eventFilter(QObject *obj, QEvent *event);


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    PlayListModel playListModel;

public slots:
    void reloadMetadata();
    void delSelectedTracks();
    void delSelectedFiles();
    void openFilesInFolder();
    void viewTrackProps();
    //void cutSelectedItems();
    //void copySelectedItems();
    //void pasteClipboardItems();
    void refresh();

private slots:
    void trackDoubleClicked(QModelIndex index);
    void headerContextMenuRequested(QPoint);
    void lockColumns(bool);
    void onTrackChanged(DB_playItem_t *, DB_playItem_t *);
    void showContextMenu(QPoint);
    void setColumnHidden(bool);
    void saveHeaderState();
    void showInGraphicalShell(const QString &pathIn);
};

#endif // PLAYLIST_H
