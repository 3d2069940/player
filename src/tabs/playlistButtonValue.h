
#ifndef _PLAYLISTBUTTONVALUE_H_
#define _PLAYLISTBUTTONVALUE_H_


#include <QListWidget>
#include <QFileInfo>


struct PlaylistButtonValue {
    QListWidget     *parent;
    QListWidgetItem *widget;
    QFileInfo        musicFile;
};

Q_DECLARE_METATYPE(PlaylistButtonValue)

#endif
