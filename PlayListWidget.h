#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QUrl>
#include <QBoxLayout>

#include "PlayList.h"
#include "TabBar.h"

class QMenu;

class PlayListWidget : public QWidget {
    Q_OBJECT

public:
    PlayListWidget(QWidget *parent = 0);

    void loadConfig();
    void saveConfig();
    
    void clearPlayList();
    void selectAll();
    void deselectAll();

    void insertByURLAtPosition(const QUrl &url, int position = -1);
    void deleteSelectedTracks();

    void header();
    void hideTab();

private:
    void configureLayout();
    void createConnections();

    QBoxLayout layout;
    TabBar tabBar;
    PlayList playList;
    TabBar::TabBarPosition tabBarPosition;
    
public slots:
    void refresh();
    
private slots:
    void selectPlaylist(int);
    void closePlylist(int);
    void renamePlaylist(int, const QString &);
    void setTabBarPosition(TabBar::TabBarPosition);
    
signals:
    void newPlaylist();
};

#endif // PLAYLISTWIDGET_H
