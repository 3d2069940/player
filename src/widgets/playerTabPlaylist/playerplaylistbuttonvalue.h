
#ifndef _PLAYERPLAYLISTBUTTONVALUE_H_
#define _PLAYERPLAYLISTBUTTONVALUE_H_ 


#include <QString>
#include <QListWidget>


struct PlayerPlaylistButtonValue {
    QString          filePath;
    QListWidget     *listWidget;
    QListWidgetItem *item;
};

Q_DECLARE_METATYPE(PlayerPlaylistButtonValue)

#endif // _PLAYERPLAYLISTBUTTONVALUE_H_
