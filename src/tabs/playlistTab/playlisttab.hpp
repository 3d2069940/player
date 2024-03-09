
#ifndef _PLAYLISTTAB_HPP_
#define _PLAYLISTTAB_HPP_


#include "src/db/db.h"
#include "playlisttab.h"

// #include "src/widgets/playlistTabPlaylist/playlisttabplaylistitem.h"
#include "src/effects/effects.h"
#include "src/widgets/playlistTabPlaylist/playlistplaylistbuttonvalue.h"
#include "src/widgets/playlistTabPlaylist/playlisttabplaylistitem.h"
#include "src/widgets/playlistTabPlaylist/playlisttabplaylistitem.hpp"

#include <QFileInfo>
#include <QPushButton>
#include <QDialogButtonBox>

#include <QDebug>
#include <qdialogbuttonbox.h>
#include <qpushbutton.h>
#include <qvariant.h>



PlaylistTab::PlaylistTab (DataBase *_db, QWidget *parent) 
    : QWidget (parent), db(_db) {
    ui.setupUi(this);
    createWidgets();
    setupWidgets();
    connectWidgets();
}

PlaylistTab::~PlaylistTab () {

}

void PlaylistTab::setEffects (Effects *_effects) {
    effects = _effects;
}

void PlaylistTab::createWidgets () {
    foreach (const QString &playlistName, db->getTableNames()) {
        auto button = new QPushButton ();
        ui.playlistScrollAreaLayout->addWidget(button);
        button->setText(playlistName);
        button->setFocusPolicy(Qt::StrongFocus);
        connect(button, &QPushButton::clicked, this, &PlaylistTab::onPlaylistButtonClicked);
    }
}

void PlaylistTab::setupWidgets () {
    ui.playlistCreationMenuFrame->hide();
    ui.playlistInfoFrame->hide();
    ui.playlistConfirmRemovingFrame->hide();

    ui.playlistValuesListWidget->setItemDelegate(&delegate);
}

void PlaylistTab::connectWidgets () {
    connect(ui.playlistCreateButton, &QPushButton::clicked, this, &PlaylistTab::onCreateButtonClicked);

    connect(ui.playlistControlButtonBox, &QDialogButtonBox::accepted, this, &PlaylistTab::onAcceptCreationButtonBoxClicked);
    connect(ui.playlistControlButtonBox, &QDialogButtonBox::rejected, this, &PlaylistTab::onRejectCreationButtonBoxClicked);

    connect(ui.playlistDeleteButton, &QPushButton::clicked, ui.playlistConfirmRemovingFrame, &QFrame::show);
    connect(ui.playlistConfirmRemovingButtonBox, &QDialogButtonBox::accepted, this, &PlaylistTab::onAcceptDeleteButtonBoxClicked);
    connect(ui.playlistConfirmRemovingButtonBox, &QDialogButtonBox::rejected, this, &PlaylistTab::onRejectDeleteButtonBoxClicked);

    connect(ui.playlistValuesListWidget, &QListWidget::itemClicked, this, &PlaylistTab::onPlaylistItemClicked);
}

void PlaylistTab::showEvent (QShowEvent *event) {
    if (currentItem != nullptr)
        currentItem->click();
    QWidget::showEvent(event);
}

void PlaylistTab::onCreateButtonClicked () {
    bool isVisible = ui.playlistCreationMenuFrame->isVisible();
    ui.playlistCreationMenuFrame->setVisible(!isVisible);

    ui.playlistInfoFrame->hide();
    ui.playlistConfirmRemovingButtonBox->hide();
    ui.playlistValuesListWidget->clear();
}

void PlaylistTab::onAcceptCreationButtonBoxClicked () {
    QString playlistName = ui.playlistNameLineEdit->text();
    if (playlistName.isEmpty() || db->tableExists(playlistName.toStdString()))
        return;
    if (!db->createTable(playlistName.toStdString()))
        return;
    auto button = new QPushButton();
    ui.playlistScrollAreaLayout->addWidget(button);
    button->setText(playlistName);

    connect(button, &QPushButton::clicked, this, &PlaylistTab::onPlaylistButtonClicked);
}

void PlaylistTab::onRejectCreationButtonBoxClicked () {
    ui.playlistNameLineEdit->clear();
    ui.playlistCreationMenuFrame->setVisible(!ui.playlistCreationMenuFrame->isVisible());
}

void PlaylistTab::onPlaylistButtonClicked () {
    auto obj = sender();
    if (obj == nullptr)
        return;

    ui.playlistValuesListWidget->clear();
    ui.playlistCreationMenuFrame->hide();
    ui.playlistInfoFrame->show();

    auto widget = qobject_cast<QPushButton*>(obj);
    currentItem = widget;
    std::string playlistName = widget->text().toStdString();
    
    QStringList playlistTime = db->getPlaylistTime(playlistName);

    ui.playlistNameLabel->setText(widget->text());

    ui.playlistCreationTimeLabel->setText(playlistTime.at(0));
    ui.playlistUpdateTimeLabel->setText(playlistTime.at(1));

    QSize       size (ui.playlistValuesListWidget->width(), 40);
    QVariant    data;
    PlaylistPlaylistButtonValue value;
    QStringList playlistValues = db->getTableValues(playlistName);

    for (int i = 2; i < playlistValues.size(); ++i) {
        QFileInfo fileInfo (playlistValues.at(i));
        if (!fileInfo.exists()) {
            db->removeValue(playlistName,
                            fileInfo.absoluteFilePath().toStdString());
            continue;
        }

        auto item   = new QListWidgetItem (ui.playlistValuesListWidget);

        value.fileInfo = fileInfo;
        value.parent   = ui.playlistValuesListWidget;
        value.item     = item;
        data = QVariant::fromValue(value);

        auto widget = new PlaylistTabPlaylistItem (data);
        
        item->setSizeHint(size);
        ui.playlistValuesListWidget->setItemWidget(item, widget);

        connect(widget, &PlaylistTabPlaylistItem::removeClicked, this, &PlaylistTab::onRemoveItemClicked);
    }
}

void PlaylistTab::onPlaylistItemClicked (QListWidgetItem *item) {
    effects->setAudioList(ui.playlistValuesListWidget);
    effects->setCurrentAudio<PlaylistTabPlaylistItem>(item);
    std::string filePath = effects->getAudioFilePath<PlaylistTabPlaylistItem>();
    // albumCover = parser->getAudioCover(filePath);
    // updateAlbumCover();
    // ui.playerPauseButton->setState(1);
    // audioTimer.start();
}

void PlaylistTab::onAcceptDeleteButtonBoxClicked () {
    if (currentItem != nullptr) {
        db->removeTable(currentItem->text().toStdString());
        currentItem->deleteLater();
    }
    ui.playlistInfoFrame->hide();
    ui.playlistConfirmRemovingFrame->hide();
    ui.playlistValuesListWidget->clear();
}

void PlaylistTab::onRejectDeleteButtonBoxClicked () {
    ui.playlistConfirmRemovingFrame->hide();
}

void PlaylistTab::onPlaylistRenameClicked () {

}

void PlaylistTab::onRemoveItemClicked (const QVariant &data) {
    auto value = data.value<PlaylistPlaylistButtonValue>();
    auto parent = value.parent;
    auto item = value.item;
    auto filePath = value.fileInfo.absoluteFilePath();

    db->removeValue(currentItem->text().toStdString(), filePath.toStdString());

    parent->removeItemWidget(item);
    delete item;
}
#endif // _PLAYLISTTAB_HPP_
