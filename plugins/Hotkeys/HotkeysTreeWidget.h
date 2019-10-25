#ifndef HOTKEYSTREEWIDGET_H
#define HOTKEYSTREEWIDGET_H

#include "QtGui.h"

#include <QTreeWidget>
#include <QLineEdit>

class HotkeysTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    HotkeysTreeWidget(QWidget *parent = nullptr);
    ~HotkeysTreeWidget();

    void loadHotkeys(DB_plugin_s **plugins);
    QHash<QString, QString> hotkeys;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void hotkeyDoubleClicked();
};

class HotkeyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    HotkeyLineEdit(QWidget* parent = nullptr);
    ~HotkeyLineEdit();

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();
};

#endif // HOTKEYSTREEWIDGET_H
