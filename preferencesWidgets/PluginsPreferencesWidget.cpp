#include "PluginsPreferencesWidget.h"
#include "ui_PluginsPreferencesWidget.h"

#include <QUrl>
#include <QDesktopServices>
#include <QTextEdit>
#include <QInputDialog>
#include <include/callbacks.h>

PluginsPreferencesWidget::PluginsPreferencesWidget(QWidget *parent, Qt::WindowFlags f):
        QWidget(parent, f),
        ui(new Ui::PluginsPreferencesWidget),
        copyrightDialog(this)
{
    ui->setupUi(this);
    ui->copyrightButton->setIcon(getStockIcon(ui->copyrightButton, "user-identity", QStyle::SP_MessageBoxInformation));
    ui->linkButton->setIcon(getStockIcon(ui->linkButton, "applications-internet", QStyle::SP_DirLinkIcon));
    settingsWidget = nullptr;
    spacer = nullptr;
    createPluginsSettings();
}

PluginsPreferencesWidget::~PluginsPreferencesWidget()
{
    delete ui;
}

void PluginsPreferencesWidget::saveSettings()
{
    settingsWidget->saveProperty();
}

void PluginsPreferencesWidget::createPluginsSettings()
{
    QList<int> sizes;
    sizes.append(120);
    sizes.append(200);
    ui->splitter->setSizes(sizes);
    connect(ui->pluginsList, SIGNAL(currentRowChanged(int)), SLOT(loadPluginInfo(int)));
    connect(ui->linkButton, SIGNAL(pressed()), SLOT(openUrl()));
    connect(ui->copyrightButton, SIGNAL(pressed()), &copyrightDialog, SLOT(exec()));
    connect(ui->btnBlacklist, SIGNAL(pressed()), SLOT(manageBlacklist()));
    
    plugins = DBAPI->plug_get_list();

    for (int i = 0; plugins[i]; i++)
    {
        QString name = QString::fromUtf8(plugins[i]->name);
        ui->pluginsList->addItem(name);
    }
    
    ui->pluginsList->setCurrentRow(0);
}

void PluginsPreferencesWidget::manageBlacklist()
{
    DBAPI->conf_lock();
    bool ok;

    QString blackList = QString::fromUtf8(DBAPI->conf_get_str_fast("blacklist_plugins", ""));

    DBAPI->conf_unlock();

    blackList = QInputDialog::getText(this, tr("Plugin blacklist"),
            tr("Type in the plugin file names without extension, split multiple names by space:"), QLineEdit::Normal, blackList, &ok);

    if (ok)
        DBAPI->conf_set_str("blacklist_plugins", blackList.toUtf8().constData());
}

void PluginsPreferencesWidget::loadPluginInfo(int item)
{
    DB_plugin_s *plugin = plugins[item];

    if (!plugin->descr && !plugin->website && !plugin->copyright)
    {
        ui->pluginDescriptionPanel->setVisible(false);
    }
    else
    {
        ui->pluginDescription->setText(plugin->descr ? QString::fromUtf8(plugin->descr) : tr("No description"));
        ui->linkButton->setVisible(plugin->website);
        ui->copyrightButton->setVisible(plugin->copyright);

        if (plugin->website)
            ui->linkButton->setToolTip(plugin->website);

        if (plugin->copyright)
            copyrightDialog.setText(plugin->copyright);
    }
    
    if (spacer)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(spacer);
        delete spacer;
    }
    
    if (plugin->configdialog)
    {
        ddb_dialog_t conf;
        conf.title = plugin->name;
        conf.layout = plugin->configdialog;
        conf.get_param = conf_get_str;
        configurePluginSettingsPanel(&conf);
    }
    /*else
    {
        if (settingsWidget)
            delete settingsWidget;
    }*/

    spacer = new QSpacerItem(10, 5, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(spacer);
}

void PluginsPreferencesWidget::configurePluginSettingsPanel(ddb_dialog_t *conf)
{
    if (settingsWidget)
    {
        ui->scrollAreaWidgetContents->layout()->removeWidget(settingsWidget);
        delete settingsWidget;
    }

    settingsWidget = new PluginSettingsWidget(conf, this);
    ui->scrollAreaWidgetContents->layout()->addWidget(settingsWidget);
}

void PluginsPreferencesWidget::openUrl()
{
    QDesktopServices::openUrl(QUrl(ui->linkButton->toolTip()));
}

void PluginsPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);

    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

CopyrightDialog::CopyrightDialog(QWidget *parent, Qt::WindowFlags f):
        QDialog(parent, f | Qt::WindowTitleHint),
        vbox(this),
        textEdit(this)
{
    setWindowIcon(getStockIcon(this, "help-about", 0));
    setWindowTitle("Copyright");
    resize(550, 350);
    vbox.setContentsMargins(0, 0, 0, 0);
    vbox.addWidget(&textEdit);
}

void CopyrightDialog::setText(const QString &text)
{
    textEdit.setText(text);
}
