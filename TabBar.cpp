#include "TabBar.h"

#include <QMouseEvent>
#include <QInputDialog>

#include "QtGui.h"

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent),
    tabContextMenu(this)
{
    configure();
    fillTabs();
    buildTabContextMenu();
    selectLastTab();
    createConnections();
}

TabBar::~TabBar()
{
    delete delPlaylist;
    delete renPlaylist;
    delete addPlaylist;
}

void TabBar::createConnections()
{
    //connect(this, QOverload<void>::of(&TabBar::tabCloseRequested), this, &TabBar::closeTab);
    connect(this, &TabBar::tabContextMenuRequested, this, &TabBar::showTabContextMenu);
    connect(this, &TabBar::tabMoved, this, &TabBar::moveTab);
}

void TabBar::fillTabs()
{
    int cnt = DBAPI->plt_get_count();
    char title[100];

    for (int i = 0; i < cnt; i++)
    {
        DBAPI->pl_lock();
        DBAPI->plt_get_title(DBAPI->plt_get_for_idx(i), title, sizeof(title));
        DBAPI->pl_unlock();
        addTab(QString::fromUtf8(title));
        strcpy(title, "");
    }
}

void TabBar::configure()
{
    setAcceptDrops(true);
    setMouseTracking(true);
    setMovable(true);
    setTabsClosable(true);
    setSelectionBehaviorOnRemove(SelectLeftTab);
}

void TabBar::selectLastTab()
{
    setCurrentIndex(DBAPI->plt_get_curr_idx());

    if (count() == 1)
    {
        setTabsClosable(false);
        delPlaylist->setEnabled(false);
    }
}

void TabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    int tab = selectTab(event->pos());

    if (tab == -1)
        newPlaylist();
    else
        emit tabDoubleClicked(tab);

    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int tab = selectTab(event->pos());
        if (tab != -1)
            emit tabSelected(tab);
    }
    else if (event->button() == Qt::RightButton)
    {
        int tab = selectTab(event->pos());

        if (tab != -1)
            emit tabContextMenuRequested(tab, mapToGlobal(event->pos()));
        else
            emit emptyAreaContextMenuRequested(mapToGlobal(event->pos()));

        return;
    }

    QTabBar::mousePressEvent(event);
}

QSize TabBar::tabSizeHint(int index) const
{
    return QTabBar::tabSizeHint(index);
}

void TabBar::wheelEvent(QWheelEvent *event)
{
    if (!(event->orientation() == Qt::Horizontal))
    {
        if (receivers(SIGNAL(wheelDelta(int))))
        {
            emit(wheelDelta(event->delta()));
            return;
        }

        int lastIndex = count() - 1;
        int targetIndex = -1;
        bool forward = event->delta() < 0;

        if (forward && lastIndex == currentIndex())
        {
            targetIndex = 0;
        }
        else
        {
            if (!forward && 0 == currentIndex())
                targetIndex = lastIndex;
        }

        setCurrentIndex(targetIndex);

        if (targetIndex != currentIndex() || !isTabEnabled(targetIndex))
            QTabBar::wheelEvent(event);

        event->accept();
        emit tabSelected(currentIndex());
    }
    else
    {
        event->ignore();
    }
}

int TabBar::selectTab(const QPoint &pos) const
{
    const int tabCount = count();

    for (int i = 0; i < tabCount; ++i)
    {
        if (tabRect(i).contains(pos))
            return i;
    }

    return -1;
}

void TabBar::buildTabContextMenu()
{
    renPlaylist = new QAction(tr("Rename playlist"), &tabContextMenu);
    connect(renPlaylist, &QAction::triggered, this, &TabBar::renamePlaylist);
    addPlaylist = new QAction(tr("Add new playlist"), &tabContextMenu);
    connect(addPlaylist, &QAction::triggered, this, &TabBar::newPlaylist);
    delPlaylist = new QAction(tr("Remove playlist"), &tabContextMenu);
    //connect(delPlaylist, &QAction::triggered, this, &TabBar::closeTab);
    
    tabContextMenu.addAction(renPlaylist);
    tabContextMenu.addAction(addPlaylist);
    tabContextMenu.addAction(delPlaylist);

    tabContextMenu.addSeparator();

    QMenu *positionMenu = tabContextMenu.addMenu(tr("Tabbar position"));

    top = positionMenu->addAction(tr("Top"));
    top->setCheckable(true);
    connect(top, &QAction::toggled, this, &TabBar::setTopPosition);

    bottom = positionMenu->addAction(tr("Bottom"));
    bottom->setCheckable(true);
    connect(bottom, &QAction::toggled, this, &TabBar::setBottomPosition);

    left = positionMenu->addAction(tr("Left"));
    left->setCheckable(true);
    connect(left, &QAction::toggled, this, &TabBar::setLeftPosition);

    right = positionMenu->addAction(tr("Right"));
    right->setCheckable(true);
    connect(right, &QAction::toggled, this, &TabBar::setRightPosition);

    QActionGroup *positionGroup = new QActionGroup(positionMenu);
    positionGroup->addAction(top);
    positionGroup->addAction(bottom);
    positionGroup->addAction(left);
    positionGroup->addAction(right);
    positionGroup->setExclusive(true);

    top->setChecked(true);
}

void TabBar::showTabContextMenu(int index, QPoint globalPos)
{
    indexForAction = index;
    tabContextMenu.move(globalPos);
    tabContextMenu.show();
}

void TabBar::moveTab(int to, int from)
{
    DBAPI->plt_move(from, to);
}

void TabBar::newPlaylist()
{
    int cnt = DBAPI->plt_get_count();
    int i;
    int idx = 0;

    for (;;)
    {
        QString name = "";

        if (!idx)
            name = tr("New Playlist");
        else
            name = tr("New Playlist (%1)").arg(idx);

        DBAPI->pl_lock();

        for (i = 0; i < cnt; i++)
        {
            char t[100];
            DBAPI->plt_get_title(DBAPI->plt_get_for_idx(i), t, sizeof(t));

            if (!strcasecmp (t, name.toUtf8().constData()))
                break;
        }

        DBAPI->pl_unlock();

        if (i == cnt)
        {
            DBAPI->plt_add(cnt, name.toUtf8().constData());

            if (count() == 1)
            {
                setTabsClosable(true);
                delPlaylist->setEnabled(true);
            }

            insertTab(cnt, name);
            setCurrentIndex(cnt);
            emit tabSelected(cnt);
            return;
        }

        idx++;
    }
}

void TabBar::closeTab(int index)
{
    if (count() == 2)
    {
        setTabsClosable(false);
        delPlaylist->setEnabled(false);
    }

    removeTab(index);
    emit tabClosed(index);
}

void TabBar::closeTab()
{
    closeTab(indexForAction);
}

void TabBar::renamePlaylist()
{
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Choose new name"), tr("Enter new playlist name: "), QLineEdit::Normal, tabText(indexForAction), &ok);

    if (ok && !newName.isEmpty())
    {
        setTabText(indexForAction, newName);
        emit tabRenamed(indexForAction, newName);
    }
}

void TabBar::setBottomPosition()
{
    emit changeTabPosition(TabBar::Bottom);
}

void TabBar::setLeftPosition()
{
    emit changeTabPosition(TabBar::Left);
}

void TabBar::setRightPosition()
{
    emit changeTabPosition(TabBar::Right);
}

void TabBar::setTopPosition()
{
    emit changeTabPosition(TabBar::Top);
}

void TabBar::setShape(QTabBar::Shape shape)
{
    switch (shape)
    {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            top->setChecked(true);
            break;

        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            bottom->setChecked(true);
            break;

        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            left->setChecked(true);
            break;

        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            right->setChecked(true);
            break;
    }

    QTabBar::setShape(shape);
}

