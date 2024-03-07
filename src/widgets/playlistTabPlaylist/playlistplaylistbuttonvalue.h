
#ifndef _PLAYLISTPLAYLISTBUTTONVALUE_H_
#define _PLAYLISTPLAYLISTBUTTONVALUE_H_


#include <QFileInfo>


class QListWidget;
class QListWidgetItem;

struct PlaylistPlaylistButtonValue {
    QFileInfo        fileInfo;
    QListWidget     *parent;
    QListWidgetItem *item;
};

Q_DECLARE_METATYPE(PlaylistPlaylistButtonValue)

#endif // _PLAYLISTPLAYLISTBUTTONVALUE_H_
