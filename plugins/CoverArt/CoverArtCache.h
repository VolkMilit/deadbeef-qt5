#ifndef COVERARTCACHE_H
#define COVERARTCACHE_H

#include <QImage>
#include <QHash>

#define CACHE_SIZE 30

#define CACHE CoverArtCache::Instance()

class CoverArtCache : public QObject
{
    Q_OBJECT

public:
    static CoverArtCache *Instance(QObject *parent = nullptr);
    static void Destroy();

    void getCoverArt(const char *fname, const char *artist, const char *album);
    void getDefaultCoverArt();

    void removeCoverArt(const char *artist, const char *album);

    void clear();

private:
    CoverArtCache(QObject *parent = nullptr);
    static CoverArtCache *instance;

    QHash<QString, const QImage *> cache;
    QString currentName;

public slots:
    void putCover(const QImage *);

signals:
    void coverIsReady(const QImage *);
};

#endif // COVERARTCACHE_H
