
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
    
    QAction *infoAction   = contextMenu->addAction("Info");
    QAction *removeAction = contextMenu->addAction("Remove");
    QAction *stopAction   = contextMenu->addAction("Stop After");
    
    button->setFixedWidth(25);
    button->setMenu(contextMenu);
    
    layout->addWidget(label);
    layout->addWidget(button);
    
    setLayout(layout);
    
    PlaylistButtonValue playlistValue = m_data.value<PlaylistButtonValue>();
    
    connect(button, &QPushButton::clicked, this, &PlaylistWidgetItem::contextMenuShow);
//     connect(infoAction,   &QAction::triggered, m_data.value<PlaylistButtonValue>().mainwindow, &MainWindowUI::);
    connect(removeAction, &QAction::triggered, playlistValue.mainwindow, [=]() {
        playlistValue.mainwindow->removeFromPlaylist(playlistValue.item);
    });
    connect(stopAction,   &QAction::triggered, playlistValue.mainwindow, [=]() {
        playlistValue.mainwindow->setStopAudio(playlistValue.item);
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

void PlaylistWidgetItem::contextMenuShow () {
    contextMenu->exec(button->mapToGlobal(QPoint(0, button->height())));
}


