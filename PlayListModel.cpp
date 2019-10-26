#include "PlayListModel.h"

#include "QtGui.h"

#include <QDebug>

PlayListModel::PlayListModel(QObject *parent) : QAbstractItemModel(parent),
    playIcon(":/root/images/play_64.png"),
    pauseIcon(":/root/images/pause_64.png") {
    connect(DBApiWrapper::Instance(), &DBApiWrapper::trackChanged, this, &PlayListModel::trackChanged);
    connect(DBApiWrapper::Instance(), &DBApiWrapper::playbackPaused, this, &PlayListModel::playerPaused);
    columnNames.insert("%s", tr("Status"));
    columnNames.insert("%n", tr("№"));
    columnNames.insert("%t", tr("Title"));
    columnNames.insert("%a", tr("Artist"));
    columnNames.insert("%b", tr("Album"));
    columnNames.insert("%y", tr("Year"));
    columnNames.insert("%l", tr("Duration"));
    
    loadConfig();
}

void PlayListModel::loadConfig()
{
    QString config = "%s|%n|%t|%a|%b|%y|%l";
    columns = config.split('|');
    status_column = columns.indexOf(QRegExp("^(.*(%s).*)+$"), 0);
}

void PlayListModel::saveConfig()
{
}

int PlayListModel::columnCount(const QModelIndex &) const
{
    return columns.count();
}

Qt::ItemFlags PlayListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QVariant PlayListModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            char title[1024];
            DBItemRef track(DBPltRef().at(index.row()));

            if (index.column() == status_column)
                DBAPI->pl_format_title(track, 0, title, sizeof(title), DB_COLUMN_PLAYING, NULL);
            else
                DBAPI->pl_format_title(track, 0, title, sizeof(title), -1, columns[index.column()].toUtf8().data());

            return QString::fromUtf8(title);
        }
        case Qt::DecorationRole:
        {
            if (index.column() == status_column)
            {
                if (playingItemIndex() == index.row())
                {
                    if (DBApiWrapper::Instance()->isPaused)
                        return pauseIcon;
                    else
                        return playIcon;
                }
            }
            break;
        }
        case Qt::SizeHintRole:
        {
            QSize defSize;
            //TODO: get value from settings
            defSize.setHeight(25);
            return defSize;
        }
    }

    return QVariant();
}

QModelIndex PlayListModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, nullptr);
}

QModelIndex PlayListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int PlayListModel::rowCount(const QModelIndex &parent) const
{
    return pltItemCount();
}

QVariant PlayListModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section < columns.count())
            {
                if (columnNames[columns[section]] == tr("Status"))
                    return QVariant("");
                else
                    return QVariant(columnNames[columns[section]]);
            }
            else
            {
                return QVariant(columns[section]);
            }
        }
    }

    return QVariant();
}

void PlayListModel::trackChanged(DB_playItem_t *from, DB_playItem_t *to)
{
    if (status_column != -1)
    {
        emit dataChanged(createIndex(0, status_column, nullptr),
                         createIndex(pltItemCount() - 1, status_column, nullptr));
    }
}

void PlayListModel::playerPaused()
{
    QModelIndex index = createIndex(playingItemIndex(), status_column, nullptr);
    emit dataChanged(index, index);
}

void PlayListModel::trackProps(const QModelIndexList &tracks)
{
    DBPltRef plt;
    DB_playItem_t *it = plt.at(tracks[0].row());

    if (DBAPI->is_local_file(DBAPI->pl_find_meta(it, ":URI")))
    {
        MetadataDialog *metaDlg = new MetadataDialog(it, nullptr);
        metaDlg->exec();
        //delete metaDlg;
    }
}

void PlayListModel::reloadMetadata(const QModelIndexList &tracks)
{
    if (tracks.length() == 0)
        return;

    DB_playItem_t *it;
    DBPltRef plt;
    QModelIndex index;

    foreach(index, tracks)
    {
        it = plt.at(index.row());
        DBAPI->pl_lock();
        QString decoder_id(DBAPI->pl_find_meta(it, ":DECODER"));
        //qDebug() << DBAPI->pl_is_selected(it);
        int match = DBAPI->is_local_file(DBAPI->pl_find_meta(it, ":URI")) && !decoder_id.isEmpty();
        DBAPI->pl_unlock();
        DBAPI->pl_item_ref(it);

        if (match)
        {
            uint32_t f = DBAPI->pl_get_item_flags(it);

            if (!(f & DDB_IS_SUBTRACK))
            {
                f &= ~DDB_TAG_MASK;
                DBAPI->pl_set_item_flags(it, f);
                DB_decoder_t **decoders = DBAPI->plug_get_decoder_list();
                for (int i = 0; decoders[i]; i++)
                {
                    if (decoder_id.compare(QString(decoders[i]->plugin.id)) != 0)
                    {
                        if (decoders[i]->read_metadata)
                        {
                            decoders[i]->read_metadata(it);
                            break;
                        }
                    }
                }
            }
        }
        DBAPI->pl_item_unref(it);
    }

    DBAPI->pl_save_current();
    DBAPI->sendmessage(DB_EV_PLAYLISTCHANGED, 0, DDB_PLAYLIST_CHANGE_CONTENT, 0);
}

void PlayListModel::deleteTracks(const QModelIndexList &tracks, bool delFile)
{
    if (tracks.length() == 0)
        return;

    if (delFile && QMessageBox::question(nullptr, "DeaDBeeF",
        tr("Selected tracks will be deleted from disk, proceed?"),
        QMessageBox::Ok|QMessageBox::Cancel,
        QMessageBox::Cancel) == QMessageBox::Cancel)
            return;

    beginRemoveRows(QModelIndex(), tracks.first().row(), tracks.last().row());

    QModelIndex index;
    DBPltRef plt;
    char fPath[PATH_MAX];
    QStringList failedList;

    foreach(index, tracks)
    {
        DB_playItem_t *it = plt.at(index.row());

        if (delFile)
        {
            if (DBAPI->streamer_get_playing_track() == it)
                DBAPI->sendmessage(DB_EV_STOP, 0, 0, 0);

            DBAPI->pl_format_title(it, -1, fPath, sizeof (fPath), -1, "%F");

            if (!QFile::remove(fPath))
                failedList << fPath;
            else
                DBAPI->pl_set_selected(it, 1);
        }
        else
            DBAPI->pl_set_selected(it, 1);
    }
    
    DBAPI->plt_delete_selected(plt);
    
    if (failedList.count() != 0)
    {
        //QMessageBox::warning(nullptr, "DeaDBeeF",
        //tr("Some files cannot be deleted:\n") + failedList.join("\n"),
        //QMessageBox::Ok);
        
        QMessageBox errorDeleteMsgBox(QMessageBox::Warning, "DeaDBeeF",
                                        tr("Some files cannot be deleted!\n"), QMessageBox::Ok);
        errorDeleteMsgBox.setDetailedText(failedList.join("\n"));
        errorDeleteMsgBox.exec();
    }
    //free(fPath);
    endRemoveRows();
}

void PlayListModel::sort(int column, Qt::SortOrder order)
{
    if (sortCount<2)
    {
        sortCount++;
        return;
    }

    if (column == status_column)
        return;

    DBPltRef plt;
    DBAPI->plt_sort(plt, PL_MAIN, -1, columns[column].toUtf8().data(), order);
    emit dataChanged(createIndex(0, 0, nullptr),
                     createIndex(plt.itemCount(), columns.count(), nullptr));
}

void PlayListModel::clearPlayList()
{
    DBPltRef plt;
    beginRemoveRows(QModelIndex(), 0, plt.itemCount() - 1);
    DBAPI->plt_clear(plt);
    endRemoveRows();
}

void PlayListModel::insertByURLAtPosition(const QUrl &url, int position)
{
    DBPltRef plt;
    int prev_track_count = plt.itemCount();
    DBApiWrapper::Instance()->addTracksByUrl(url, position);
    int count = plt.itemCount() - prev_track_count;
    beginInsertRows(QModelIndex(), position, position + count - 1);
    endInsertRows();
}

void PlayListModel::moveItems(QList<int> indices, int before)
{
    uint32_t inds[indices.length()];
    for (int i = 0; i < indices.length(); i++)
        inds[i] = indices[i];

    DBAPI->pl_lock();
    DBPltRef plt;
    int lastItem = plt.itemCount() - 1;
    DBItemRef bef(before > lastItem
                  ? plt.at(lastItem).next()
                  : plt.at(before));
    
    DBAPI->plt_move_items(plt, PL_MAIN, plt, bef, inds, indices.length());
    //DBAPI->plt_save_config(plt);
    DBAPI->pl_unlock();
    
    //DBAPI->sendmessage(DB_EV_PLAYLISTCHANGED, 0, DDB_PLAYLIST_CHANGE_CONTENT, 0);
    
    beginRemoveRows(QModelIndex(), 0, lastItem);
    endRemoveRows();
    beginInsertRows(QModelIndex(), 0, lastItem);
    endInsertRows();
}

QStringList PlayListModel::mimeTypes () const
{
    QStringList qstrList;
    qstrList.append("playlist/track");
    qstrList.append("text/uri-list");
    return qstrList;
}

Qt::DropActions PlayListModel::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QMimeData *PlayListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << index.row() << text;
        }
    }

    mimeData->setData("playlist/track", encodedData);
    return mimeData;
}
