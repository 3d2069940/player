
#ifndef _PLAYERTABPLAYLIST_HPP_
#define _PLAYERTABPLAYLIST_HPP_

#include <QEvent>
#include <QApplication>

#include "playertabplaylistitem.h"
#include "playerplaylistbuttonvalue.h"


PlayerTabPlaylistItem::PlayerTabPlaylistItem (const QString &labelText, const QVariant &_data)  
  : data(_data) {
    layout      = new QHBoxLayout ();
    button      = new QPushButton ();
    label       = new QLabel(labelText);
    contextMenu = new QMenu ();
    
    infoAction    = contextMenu->addAction("Info");
    addToPlaylist = contextMenu->addAction("Add To Playlist");
    stopAction    = contextMenu->addAction("Stop After");
    
    updateIcons ();
    
    button->setFixedWidth(25);
    button->setMenu(contextMenu);
    
    layout->addWidget(label);
    layout->addWidget(button);
    
    setLayout(layout);
    
    auto playlistValue = data.value<PlayerPlaylistButtonValue>();
    
    connect(button,       &QPushButton::clicked, this, &PlayerTabPlaylistItem::contextMenuShow);
//     connect(infoAction,   &QAction::triggered, m_data.value<PlaylistButtonValue>().mainwindow, &MainWindowUI::infoActionClicked);
    // connect(infoAction,   &QAction::triggered, this, &PlayerTabPlaylistItem::infoActionClicked);
    connect(addToPlaylist, &QAction::triggered, this, &PlayerTabPlaylistItem::addToPlaylistClicked);
}

PlayerTabPlaylistItem::~PlayerTabPlaylistItem () {
    delete contextMenu;
    delete label;
    delete button;
    delete layout;
}

std::string PlayerTabPlaylistItem::filePath () const {
    return data.value<PlayerPlaylistButtonValue>().filePath.toStdString();
}

void PlayerTabPlaylistItem::updateIcons () {
    QPalette palette = qApp->palette();
    QColor   backgroundColor = palette.color(QPalette::Window);
    QString  iconPath = backgroundColor.value() < 128 ? ":icons/white/" : ":icons/black/";
    
    infoAction  ->setIcon(QIcon(iconPath+"info.svg"));
    stopAction  ->setIcon(QIcon(iconPath+"stop_after.svg"));
}

void PlayerTabPlaylistItem::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange)
        updateIcons();
    QWidget::changeEvent(event);
}

void PlayerTabPlaylistItem::contextMenuShow () {
    contextMenu->exec(button->mapToGlobal(QPoint(0, button->height())));
}

void PlayerTabPlaylistItem::addToPlaylistClicked () {
    emit onAddToPlaylistClicked (data.value<PlayerPlaylistButtonValue>().filePath);
}

#endif // _PLAYERTABPLAYLIST_HPP_
