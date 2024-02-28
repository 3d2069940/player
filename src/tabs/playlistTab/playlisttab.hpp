
#ifndef _PLAYLISTTAB_HPP_
#define _PLAYLISTTAB_HPP_

#include "playlisttab.h"

#include <QDialogButtonBox>
#include <qdialogbuttonbox.h>

PlaylistTab::PlaylistTab (QWidget *parent) 
    : QWidget (parent) {
    ui.setupUi(this);
    createWidgets();
    setupWidgets();
    connectWidgets();
}

PlaylistTab::~PlaylistTab () {

}

void PlaylistTab::createWidgets () {

}

void PlaylistTab::setupWidgets () {
    ui.playlistCreationMenuFrame->hide();
}

void PlaylistTab::connectWidgets () {
    connect(ui.playlistCreateButton, &QPushButton::clicked, this, &PlaylistTab::onCreateButtonClicked);

    connect(ui.playlistControlButtonBox, &QDialogButtonBox::accepted, this, &PlaylistTab::onAcceptButtonBoxClicked);
    connect(ui.playlistControlButtonBox, &QDialogButtonBox::rejected, this, &PlaylistTab::onRejectButtonBoxClicked);
}

void PlaylistTab::onCreateButtonClicked () {
    bool isVisible = ui.playlistCreationMenuFrame->isVisible();
    ui.playlistCreationMenuFrame->setVisible(!isVisible);
}

void PlaylistTab::onAcceptButtonBoxClicked () {

}

void PlaylistTab::onRejectButtonBoxClicked () {
    ui.playlistNameLineEdit->clear();
    ui.playlistCreationMenuFrame->setVisible(!ui.playlistCreationMenuFrame->isVisible());
}
#endif // _PLAYLISTTAB_HPP_
