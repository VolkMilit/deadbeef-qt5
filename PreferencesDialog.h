#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>

#include <QStyle>

#include "QtGui.h"

#include "preferencesWidgets/InterfacePreferencesWidget.h"
#include "preferencesWidgets/SoundPreferencesWidget.h"
#include "preferencesWidgets/DspPreferencesWidget.h"
#include "preferencesWidgets/NetworkPreferencesWidget.h"
#include "preferencesWidgets/PluginsPreferencesWidget.h"

#ifdef HOTKEYS_ENABLED
#include "plugins/Hotkeys/HotkeysWidget.h"
#endif

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog(){}

private:
    QVBoxLayout vbox;
    QTabWidget tabWidget{this};
    QHBoxLayout hBoxButtons;
    QPushButton buttonOk{this};
    QPushButton buttonApply{this};

    InterfacePreferencesWidget interfaceWidget{this};
    SoundPreferencesWidget soundWidget{this};
    DspPreferencesWidget dspWidget{this};
    NetworkPreferencesWidget networkWidget{this};
#ifdef HOTKEYS_ENABLED
    HotkeysWidget hotkeysWidget{this};
#endif
    PluginsPreferencesWidget pluginsWidget{this};

    void configureTabs();
    void configureLayout();
    void configureConnections();
    
private slots:
    void on_buttonOk_clicked();
    void on_buttonApply_clicked();

signals:
    void setTrayIconHidden(bool);
    //void setTrayIconTheme(const QString &);
    void setCloseOnMinimize(bool);
    void titlePlayingChanged();
    void titleStoppedChanged();
    void refreshRateChanged(const QString &);
};

#endif // PREFERENCESDIALOG_H
