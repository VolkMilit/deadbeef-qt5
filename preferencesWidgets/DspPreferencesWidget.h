#ifndef DSPPREFERENCESWIDGET_H
#define DSPPREFERENCESWIDGET_H

#include <QDir>
#include <QWidget>
#include <QStandardItemModel>

#include "PluginSettingsWidget.h"
#include "QtGui.h"

namespace Ui {
    class DspPreferencesWidget;
}

class DspPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    DspPreferencesWidget(QWidget* parent = nullptr, Qt::WindowFlags f = nullptr);
    ~DspPreferencesWidget();

private:
    Ui::DspPreferencesWidget *ui;
    
    QStandardItemModel *model;
    QDir dspPresetDir;
    ddb_dsp_context_t *dsp_clone(ddb_dsp_context_t *from);
    ddb_dsp_context_t *chain;

    int swap_items(int idx);
    void loadSettings();
    void createConnections();
    void fill_dsp_chain(QStandardItemModel *mdl);    
    void fillPresets();    
    void update_streamer();
    
private slots:
    void openDspConf();
    void addDsp();
    void rmDsp();
    void dspUp();
    void dspDown();
    void deletePreset();
    void savePreset();
    void loadPreset();
};

#endif // DSPPREFERENCESWIDGET_H
