
#pragma once

#include <string>

#include <QPoint>
#include <QDebug>

//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "metaTypes.h"
#include "playlistWidgetItem.h"


PlaylistWidgetItem::PlaylistWidgetItem (const QString &labelText, const QVariant &data)  
  : m_data(data) {
    layout      = new QHBoxLayout ();
    button      = new QPushButton ();
    label       = new QLabel(labelText);
    contextMenu = new QMenu ();
    
    infoAction    = contextMenu->addAction("Info");
    removeAction  = contextMenu->addAction("Remove");    
    addToPlaylist = contextMenu->addAction("Add To Playlist");
    stopAction    = contextMenu->addAction("Stop After");
    
    updateIcons ();
    
    button->setFixedWidth(25);
    button->setMenu(contextMenu);
    
    layout->addWidget(label);
    layout->addWidget(button);
    
    setLayout(layout);
    
    PlaylistButtonValue playlistValue = m_data.value<PlaylistButtonValue>();
    
    connect(button,       &QPushButton::clicked, this, &PlaylistWidgetItem::contextMenuShow);
//     connect(infoAction,   &QAction::triggered, m_data.value<PlaylistButtonValue>().mainwindow, &MainWindowUI::infoActionClicked);
    connect(infoAction,   &QAction::triggered, this, &PlaylistWidgetItem::infoActionClicked);
    connect(removeAction, &QAction::triggered, playlistValue.mainwindow, [=]() {
        playlistValue.mainwindow->removeFromPlaylist(playlistValue.item);
    });
    connect(stopAction,   &QAction::triggered, playlistValue.mainwindow, [=]() {
        playlistValue.mainwindow->setPlaylistStopAudio(playlistValue.item);
    });
}

PlaylistWidgetItem::~PlaylistWidgetItem () {
    delete contextMenu;
    delete label;
    delete button;
    delete layout;
}

std::string PlaylistWidgetItem::filePath () const {
    return m_data.value<PlaylistButtonValue>().filePath.toStdString();
}

void PlaylistWidgetItem::updateIcons () {
    QPalette palette = qApp->palette();
    QColor   backgroundColor = palette.color(QPalette::Window);
    QString  iconPath = backgroundColor.value() < 128 ? ":icons/white/" : ":icons/black/";
    
    infoAction  ->setIcon(QIcon(iconPath+"info.svg"));
    removeAction->setIcon(QIcon(iconPath+"remove.svg"));
    stopAction  ->setIcon(QIcon(iconPath+"stop_after.svg"));
}

void PlaylistWidgetItem::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange)
        updateIcons();
    QWidget::changeEvent(event);
}

void PlaylistWidgetItem::contextMenuShow () {
    contextMenu->exec(button->mapToGlobal(QPoint(0, button->height())));
}

void PlaylistWidgetItem::infoActionClicked () {
    emit infoActionShow(m_data.value<PlaylistButtonValue>().filePath);
}
