#include "PreferencesDialog.h"

#include <include/callbacks.h>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
        QDialog(parent, Qt::WindowTitleHint)
{
    setModal(true);
    setWindowTitle(tr("Preferences"));
    setWindowIcon(getStockIcon(this, "preferences-system", QStyle::SP_CustomBase));

    buttonApply.setText("Apply");
    buttonOk.setText("Ok");

    hBoxButtons.addWidget(&buttonApply);
    hBoxButtons.addWidget(&buttonOk);

    configureTabs();
    configureLayout();
    configureConnections();

    setLayout(&vbox);

    resize(800, 600);
}

void PreferencesDialog::configureTabs()
{
    tabWidget.addTab(&interfaceWidget, tr("Interface"));
    tabWidget.addTab(&soundWidget, tr("Sound"));
    tabWidget.addTab(&dspWidget, tr("DSP"));
    tabWidget.addTab(&networkWidget, tr("Network"));
    tabWidget.addTab(&pluginsWidget, tr("Plugins"));
#ifdef HOTKEYS_ENABLED
    tabWidget.addTab(&hotkeysWidget, tr("Hotkeys"));
#endif
    this->adjustSize();
}

void PreferencesDialog::configureLayout()
{
    vbox.addWidget(&tabWidget);
    vbox.addLayout(&hBoxButtons);
}

void PreferencesDialog::configureConnections()
{
    connect(&interfaceWidget, &InterfacePreferencesWidget::setCloseOnMinimize, this, &PreferencesDialog::setCloseOnMinimize);
    connect(&interfaceWidget, &InterfacePreferencesWidget::setTrayIconHidden, this, &PreferencesDialog::setTrayIconHidden);
    //connect(&interfaceWidget, &InterfacePreferencesWidget::setTrayIconTheme, this, &PreferencesDialog::setTrayIconTheme);
    connect(&interfaceWidget, &InterfacePreferencesWidget::titlePlayingChanged, this, &PreferencesDialog::titlePlayingChanged);
    connect(&interfaceWidget, &InterfacePreferencesWidget::titleStoppedChanged, this, &PreferencesDialog::titleStoppedChanged);
    connect(&buttonOk, &QPushButton::clicked, this, &PreferencesDialog::on_buttonOk_clicked);
    connect(&buttonApply, &QPushButton::clicked, this, &PreferencesDialog::on_buttonApply_clicked);
}

void PreferencesDialog::on_buttonOk_clicked()
{
    pluginsWidget.saveSettings();
    DBAPI->sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
    close();
}

void PreferencesDialog::on_buttonApply_clicked()
{
    pluginsWidget.saveSettings();
    DBAPI->sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
}
