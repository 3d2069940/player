
#ifndef _PLAYLISTTABPLAYLISTITEM_HPP_
#define _PLAYLISTTABPLAYLISTITEM_HPP_


#include "playlisttabplaylistitem.h"
#include "playlistplaylistbuttonvalue.h"

#include <QMenu>
#include <QLabel>
#include <QAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>


PlaylistTabPlaylistItem::PlaylistTabPlaylistItem (const QVariant &_data)
    : data(_data) {
    layout = new QHBoxLayout ();

    label  = new QLabel (data.value<PlaylistPlaylistButtonValue>().fileInfo.fileName());
    button = new QPushButton ();

    menu = new QMenu ();

    infoAction   = menu->addAction("Info");
    removeAction = menu->addAction("Remove");
    stopAction   = menu->addAction("Stop After");

    button->setMenu(menu);
    button->setFixedWidth(25);

    layout->addWidget(label);
    layout->addWidget(button);

    setLayout(layout);

    connect(removeAction, &QAction::triggered, this, &PlaylistTabPlaylistItem::removeClickedWrapper);
}

PlaylistTabPlaylistItem::~PlaylistTabPlaylistItem () {

}

std::string PlaylistTabPlaylistItem::filePath () const {
    QFileInfo fileInfo = data.value<PlaylistPlaylistButtonValue>().fileInfo;
    return fileInfo.absoluteFilePath().toStdString();
}

void PlaylistTabPlaylistItem::removeClickedWrapper () {
    emit removeClicked(data);
}

#endif // _PLAYLISTTABPLAYLISTITEM_HPP_
