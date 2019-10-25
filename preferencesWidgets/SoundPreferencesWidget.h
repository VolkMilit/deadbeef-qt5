#ifndef SOUNDPREFERENCESWIDGET_H
#define SOUNDPREFERENCESWIDGET_H

#include <QWidget>
#include <QHash>
#include <QIntValidator>

enum { DDB_RG_PROCESSING_NONE = 0, DDB_RG_PROCESSING_GAIN = 1, DDB_RG_PROCESSING_PREVENT_CLIPPING = 2, };

namespace Ui {
    class SoundPreferencesWidget;
}

class SoundPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    SoundPreferencesWidget(QWidget* parent = nullptr, Qt::WindowFlags f = nullptr);
    
    void addDevice(const char *name, const char *desc);
    
private:
    Ui::SoundPreferencesWidget *ui;
    
    QHash<QString, QString> alsaDevices;
    
    void loadSettings();
    void createConnections();
    
protected:
    void changeEvent(QEvent *e);

private slots:
    void changeOutputDevice(int);
    void changeOutputPlugin(int);
    void changeReplaygainMode(int);
    void changeReplaygainFlags(int);
    void saveReplaygainScale(bool);
    void saveReplaygainPreamp();
    void saveReplaygainGlobalPreamp();
    void saveAddToDefaultPlaylist(bool);
    void saveDefaultPlaylistName();
    void saveDontAddArchives(bool);
    void saveResumeOnStartup(bool);
    void save8to16(bool enabled);
    void save16to24(bool enabled);
    void saveOverrideSR(bool enabled);
    void saveTargetSR();
    void saveDependentSR(bool enabled);
    void saveSRMulti48();
    void saveSRMulti44();
};

#endif // SOUNDPREFERENCESWIDGET_H
