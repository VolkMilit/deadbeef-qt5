#include "SystemTrayIcon.h"

#include "QtGuiSettings.h"
#include <QDebug>

SystemTrayIcon::SystemTrayIcon(QObject *parent) :
    QSystemTrayIcon(parent),
    trayMenu(new QMenu)
{
    connect(this, &SystemTrayIcon::activated, this, &SystemTrayIcon::handleActivation);

    defaultTrayIcon = QIcon(":/root/images/bitmap.png");
    trayIconTheme["Default"] = defaultTrayIcon;
    trayIconTheme["Dark"] = QIcon(":/root/images/tray_dark.png");
    trayIconTheme["Light"] = QIcon(":/root/images/tray_light.png");

    QString variant(SETTINGS->getTrayIconTheme());
    if (trayIconTheme.contains(variant))
        setIcon(trayIconTheme[variant]);
    else
        setIcon(defaultTrayIcon);

    setContextMenu(trayMenu);
    show();
}

SystemTrayIcon::~SystemTrayIcon()
{
    delete trayMenu;
}

void SystemTrayIcon::addEntry(QAction *action)
{
    trayMenu->addAction(action);
}

void SystemTrayIcon::addSeparator()
{
    trayMenu->addSeparator();
}

void SystemTrayIcon::setTrayTheme(const QString &icon)
{
    if (trayIconTheme.contains(icon))
        setIcon(trayIconTheme[icon]);
    else
        setIcon(defaultTrayIcon);
}

template <typename T>
int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

bool SystemTrayIcon::event(QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelevent = static_cast<QWheelEvent *>(event);
        emit wheeled(signum(wheelevent->delta()));
        event->accept();
        return true;
    }
    else
    {
        return QSystemTrayIcon::event(event);
    }
}

void SystemTrayIcon::handleActivation(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        emit onTrigger();

    if (reason == QSystemTrayIcon::MiddleClick)
        emit onMiddleClick();
}
