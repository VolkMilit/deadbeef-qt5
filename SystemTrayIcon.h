#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    virtual ~SystemTrayIcon();

    void addEntry(QAction *action);
    void addSeparator();

    void setTrayTheme(const QString &icon);

protected:
    virtual bool event(QEvent *event);

signals:
    void wheeled(int);
    void onMiddleClick();
    void onTrigger();

private:
    void handleActivation(QSystemTrayIcon::ActivationReason reason);

    QMenu *trayMenu;

    QIcon defaultTrayIcon;
    QMap<QString, QIcon> trayIconTheme;
};

#endif // SYSTEMTRAYICON_H
