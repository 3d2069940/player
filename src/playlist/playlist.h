
#ifndef PLAYLIST_H
#define PLAYLIST_H


#include <QStringList>
#include <qlistwidget.h>


class Playlist {

public: 
    Playlist  ();
    ~Playlist ();

    inline void setPlaylistFiles (const QStringList &_playlistFiles);
    inline void show (QListWidget *widget);

private:
    std::string name;
    QStringList playlistFiles;
};

Q_DECLARE_METATYPE(Playlist);
#endif // PLAYLIST_H
