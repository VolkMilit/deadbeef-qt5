#ifndef COVERARTWRAPPER_H
#define COVERARTWRAPPER_H

#include <QImage>
#include <QFutureWatcher>
#include <QApplication>
#include <QDesktopWidget>

class CoverArtWrapper : public QObject
{
    Q_OBJECT
public:
    static CoverArtWrapper *Instance(QObject *parent = nullptr);
    static void Destroy();

    void getCoverArt(const char *fname, const char *artist, const char *album);
    void getDefaultCoverArt();
    void openAndScaleCover(const char *fname);

    int defaultWidth;
    int defaultWidthPixel;

private:
    CoverArtWrapper(QObject *parent = nullptr);
    static CoverArtWrapper *instance;

    QFutureWatcher<QImage *> coverLoadWatcher;

    void loadSettings();
    void saveSettings();

signals:
    void coverIsReady(const QImage *);

private slots:
    void onImageLoaded(int);
};

#endif // COVERARTWRAPPER_H
