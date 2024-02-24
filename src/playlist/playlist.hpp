
#pragma once

#include "playlist.h"
#include <QFileInfo>

Playlist::Playlist () {
    
}

Playlist::~Playlist () {

}

void Playlist::setPlaylistFiles (const QStringList &_playlistFiles) {
    playlistFiles = _playlistFiles;    
}

void Playlist::show (QListWidget *widget) {
    widget->clear();
    foreach (const QString &file, playlistFiles) {
        QFileInfo fileInfo (file);
        if (fileInfo.exists()) {
            widget->addItem(fileInfo.fileName());
        }
    }
}
