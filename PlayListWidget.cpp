#include "PlayListWidget.h"

#include <QtGui.h>
#include <QtGuiSettings.h>

PlayListWidget::PlayListWidget(QWidget *parent) :
        QWidget(parent),
        layout(QBoxLayout::TopToBottom, this),
        tabBar(this),
        playList(this)
{
    loadConfig();
    configureLayout();
    playList.restoreCursor();
    createConnections();
}

void PlayListWidget::createConnections()
{
    connect(&tabBar, &TabBar::tabClosed, this, &PlayListWidget::closePlylist);
    connect(&tabBar, &TabBar::tabSelected, this, &PlayListWidget::selectPlaylist);
    connect(this, &PlayListWidget::newPlaylist, &tabBar, &TabBar::newPlaylist);
    connect(&tabBar, &TabBar::tabRenamed, this, &PlayListWidget::renamePlaylist);
    connect(&tabBar, &TabBar::changeTabPosition, this, &PlayListWidget::setTabBarPosition);
}

void PlayListWidget::configureLayout()
{
    layout.setSpacing(0);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.addWidget(&tabBar);
    layout.addWidget(&playList);
}

void PlayListWidget::loadConfig()
{
    bool isVisible = SETTINGS->getTabBarIsVisible();
    tabBar.setHidden(!isVisible);

    int pos = SETTINGS->getTabBarPosition();
    switch (pos)
    {
        case TabBar::Left:
            tabBar.setShape(QTabBar::RoundedWest);
            tabBar.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            layout.setDirection(QBoxLayout::LeftToRight);
            tabBarPosition = TabBar::Left;
            break;

        case TabBar::Right:
            tabBar.setShape(QTabBar::RoundedEast);
            tabBar.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            layout.setDirection(QBoxLayout::RightToLeft);
            tabBarPosition = TabBar::Right;
            break;

        case TabBar::Bottom:
            tabBar.setShape(QTabBar::RoundedSouth);
            tabBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout.setDirection(QBoxLayout::BottomToTop);
            tabBarPosition = TabBar::Bottom;
            break;

        case TabBar::Top:
        default:
            tabBar.setShape(QTabBar::RoundedNorth);
            tabBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout.setDirection(QBoxLayout::TopToBottom);
            tabBarPosition = TabBar::Top;
            break;
    }
}

void PlayListWidget::saveConfig()
{
    SETTINGS->setTabBarIsVisible(!tabBar.isHidden());
    SETTINGS->setTabBarPosition (tabBarPosition);
    playList.saveConfig();
}

void PlayListWidget::deleteSelectedTracks()
{
    playList.delSelectedTracks();
}

void PlayListWidget::insertByURLAtPosition(const QUrl &url, int position)
{
    playList.insertByURLAtPosition(url, position);
}

void PlayListWidget::refresh()
{
    playList.refresh();
}

void PlayListWidget::clearPlayList()
{
    playList.clearPlayList();
}

void PlayListWidget::selectAll()
{
    playList.selectAll();
}

void PlayListWidget::deselectAll()
{
    playList.clearSelection();
}

void PlayListWidget::setTabBarPosition(TabBar::TabBarPosition position)
{
    if (position == tabBarPosition)
        return;

    switch (position)
    {
        case TabBar::Left:
            tabBar.setShape(QTabBar::RoundedWest);
            tabBar.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            layout.setDirection(QBoxLayout::LeftToRight);
            break;

        case TabBar::Right:
            tabBar.setShape(QTabBar::RoundedEast);
            tabBar.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            layout.setDirection(QBoxLayout::RightToLeft);
            break;

        case TabBar::Top:
            tabBar.setShape(QTabBar::RoundedNorth);
            tabBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout.setDirection(QBoxLayout::TopToBottom);
            break;

        case TabBar::Bottom:
            tabBar.setShape(QTabBar::RoundedSouth);
            tabBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout.setDirection(QBoxLayout::BottomToTop);
            break;
    }

    tabBarPosition = position;
}

void PlayListWidget::closePlylist(int index)
{
    int playlist = DBAPI->plt_get_curr_idx();
    DBAPI->plt_remove(index);

    if (playlist == index)
        DBAPI->plt_set_curr_idx((index - 1 < 0) ? 0 : index - 1);

    playlist = DBAPI->plt_get_curr_idx();
    DBAPI->conf_set_int("playlist.current", playlist);

    playList.refresh();
}

void PlayListWidget::selectPlaylist(int index)
{
    if (DBAPI->plt_get_curr_idx() == index)
        return;

    DBAPI->plt_set_curr_idx(index);
    DBAPI->conf_set_int("playlist.current", index);

    playList.refresh();
}

void PlayListWidget::renamePlaylist(int index, const QString &newName)
{
    DBAPI->pl_lock();
    DBAPI->plt_set_title(DBAPI->plt_get_for_idx(index), newName.toUtf8().constData());
    DBAPI->pl_unlock();
}

void PlayListWidget::header()
{
    playList.toggleHeaderHidden();
}

void PlayListWidget::hideTab()
{
    tabBar.setHidden(!tabBar.isHidden());
}
