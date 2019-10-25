#ifndef HOTKEYSWIDGET_H
#define HOTKEYSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>

#include "HotkeysTreeWidget.h"

class HotkeysWidget : public QWidget
{
    Q_OBJECT
public:
    HotkeysWidget(QWidget* parent = nullptr, Qt::WindowFlags f = nullptr);

private:
    void createConnections();
    
    QVBoxLayout vbox;
    HotkeysTreeWidget hotkeysTreeWidget;
    QHBoxLayout hbox;
    HotkeyLineEdit hotkeyLineEdit;
    QPushButton clearHotkeyButton;

private slots:
    void catchHotkey();
    void setHotkeyLineEdit();
    void clearHotkey();
    void applyHotkey(const QString &hotkey);
};

#endif // HOTKEYSWIDGET_H
