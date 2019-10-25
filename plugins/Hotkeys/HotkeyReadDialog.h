#ifndef HOTKEYREADDIALOG_H
#define HOTKEYREADDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class HotkeyReadDialog : public QDialog
{
    Q_OBJECT

public:
    HotkeyReadDialog(const QString &actionTitle, QWidget *parent = nullptr);
    ~HotkeyReadDialog();

private:
    QVBoxLayout vbox;
    QLabel message;
    QLabel hotkey;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void hotkeyChanged(const QString &);
};

#endif // HOTKEYREADDIALOG_H
