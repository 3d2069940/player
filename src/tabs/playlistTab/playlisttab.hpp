
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
    updateIcons();
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
    audioTimer.setInterval(500);

    ui.playlistCreationMenuFrame->hide();
    ui.playlistInfoFrame->hide();
    ui.playlistConfirmRemovingFrame->hide();
    ui.playlistAudioControlFrame->hide();
    ui.playlistRenameLayout->hide();

    ui.playlistValuesListWidget->setItemDelegate(&delegate);
}

void PlaylistTab::connectWidgets () {
    connect(ui.playlistCreateButton, &QPushButton::clicked, this, &PlaylistTab::onCreateButtonClicked);

    connect(ui.playlistControlButtonBox, &QDialogButtonBox::accepted, this, &PlaylistTab::onAcceptCreationButtonBoxClicked);
    connect(ui.playlistControlButtonBox, &QDialogButtonBox::rejected, this, &PlaylistTab::onRejectCreationButtonBoxClicked);

    connect(ui.playlistRenameButton, &QPushButton::clicked, this, &PlaylistTab::onRenameButtonClicked);
    connect(ui.playlistDeleteButton, &QPushButton::clicked, this, &PlaylistTab::onDeleteButtonClicked);
    connect(ui.playlistConfirmRemovingButtonBox, &QDialogButtonBox::accepted, this, &PlaylistTab::onAcceptDeleteButtonBoxClicked);
    connect(ui.playlistConfirmRemovingButtonBox, &QDialogButtonBox::rejected, this, &PlaylistTab::onRejectDeleteButtonBoxClicked);

    connect(ui.playlistValuesListWidget, &QListWidget::itemClicked, this, &PlaylistTab::onPlaylistItemClicked);
//  slider
    connect(ui.playlistAudioPositionSlider, &QSlider::valueChanged, this, &PlaylistTab::onSliderValueChanged);
    connect(ui.playlistAudioPositionSlider, &QSlider::sliderPressed, this, &PlaylistTab::onSliderPressed);
    connect(ui.playlistAudioPositionSlider, &QSlider::sliderReleased, this, &PlaylistTab::onSliderReleased);
//  timer
    connect(&audioTimer, &QTimer::timeout, this, &PlaylistTab::updateAudioInfo);
//  control buttons
    connect(ui.playlistPreviousButton, &QPushButton::clicked, this, &PlaylistTab::onPreviousButtonClicked);
    connect(ui.playlistPauseButton, &QPushButton::clicked, this, &PlaylistTab::onPauseButtonClicked);
    connect(ui.playlistNextButton, &QPushButton::clicked, this, &PlaylistTab::onNextButtonClicked);
}

void PlaylistTab::updateIcons () {
    QColor  backgroundColor = palette().color(QPalette::Window);
    QString iconPath = backgroundColor.value() < 128 ? QStringLiteral(":icons/white/") :
                                                       QStringLiteral(":icons/black/");

    ui.playlistPreviousButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playlistRepeatButton->setIcon(QIcon(iconPath+"repeat_all.svg"));
    ui.playlistPauseButton->setIcons({QIcon(iconPath+"play_music.svg"),
                                    QIcon(iconPath+"pause_music.svg")});
    ui.playlistShuffleButton->setIcon(QIcon(iconPath+"shuffle_playlist.svg"));
    ui.playlistNextButton->setIcon(QIcon(iconPath+"next_track.svg"));
}

void PlaylistTab::showEvent (QShowEvent *event) {
    if (currentItem != nullptr)
        currentItem->click();
    QWidget::showEvent(event);
}

void PlaylistTab::onCreateButtonClicked () {
    bool isVisible = ui.playlistCreationMenuFrame->isVisible();
    ui.playlistCreationMenuFrame->setVisible(!isVisible);

    ui.playlistConfirmRemovingFrame->hide();
    ui.playlistRenameLayout->hide();
    ui.playlistAudioControlFrame->hide();
    ui.playlistInfoFrame->hide();

    ui.playlistValuesListWidget->clear();
}

void PlaylistTab::onPlaylistButtonClicked () {
    auto obj = sender();
    if (obj == nullptr)
        return;

    int currentId = ui.playlistValuesListWidget->currentRow();

    ui.playlistValuesListWidget->clear();
    ui.playlistCreationMenuFrame->hide();
    ui.playlistInfoFrame->show();
    ui.playlistAudioControlFrame->show();

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
    ui.playlistValuesListWidget->setCurrentRow(currentId);
}

void PlaylistTab::onPlaylistItemClicked (QListWidgetItem *item) {
    effects->setAudioList(ui.playlistValuesListWidget);
    effects->setCurrentAudio<PlaylistTabPlaylistItem>(item);
    std::string filePath = effects->getAudioFilePath<PlaylistTabPlaylistItem>();
    ui.playlistPauseButton->setState(1);
    audioTimer.start();
    emit audioStateChanged();
    // albumCover = parser->getAudioCover(filePath);
    // updateAlbumCover();
    // ui.playerPauseButton->setState(1);
    // audioTimer.start();
}

void PlaylistTab::onRenameButtonClicked () {
    ui.playlistConfirmRemovingFrame->hide();
    ui.playlistRenameLayout->show();
}

void PlaylistTab::onDeleteButtonClicked () {
    ui.playlistConfirmRemovingFrame->show();
    ui.playlistRenameLayout->hide();
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

void PlaylistTab::onRemoveItemClicked (const QVariant &data) {
    auto value = data.value<PlaylistPlaylistButtonValue>();
    auto parent = value.parent;
    auto item = value.item;
    auto filePath = value.fileInfo.absoluteFilePath();

    db->removeValue(currentItem->text().toStdString(), filePath.toStdString());

    parent->removeItemWidget(item);
    delete item;
}

void PlaylistTab::onSliderValueChanged (int value) {
    Q_UNUSED(value);
}

void PlaylistTab::onSliderReleased () {
    effects->updateAudioDuration();
    int max   = ui.playlistAudioPositionSlider->maximum(),
        value = ui.playlistAudioPositionSlider->value();
    effects->seekPlayingPosition(effects->audioDuration*value/max);
    audioTimer.start();
}

void PlaylistTab::onSliderPressed () {
    audioTimer.stop();
}

void PlaylistTab::onPreviousButtonClicked () {
    if (ui.playlistValuesListWidget->currentItem() == nullptr)
        return;
    audioTimer.start();
    effects->setAudioList(ui.playlistValuesListWidget);
    effects->setCurrentAudio<PlaylistTabPlaylistItem>(ui.playlistValuesListWidget->currentItem());
    effects->playPreviousAudio();
    ui.playlistPauseButton->setState(1);
    emit audioStateChanged();
}

void PlaylistTab::onPauseButtonClicked () {
    if (ui.playlistValuesListWidget->currentItem() == nullptr)
        return;
    effects->togglePipelineState();
    emit audioStateChanged();
}

void PlaylistTab::onNextButtonClicked () {
    if (ui.playlistValuesListWidget->currentItem() == nullptr)
        return;
    audioTimer.start();
    effects->setAudioList(ui.playlistValuesListWidget);
    effects->setCurrentAudio<PlaylistTabPlaylistItem>(ui.playlistValuesListWidget->currentItem());
    effects->playNextAudio();
    ui.playlistPauseButton->setState(1);
    emit audioStateChanged();
}

void PlaylistTab::updateAudioInfo () {
    QSignalBlocker blocker (ui.playlistAudioPositionSlider);
    effects->updateAudioDuration();
    effects->updateAudioPosition();

    int sliderMax = ui.playlistAudioPositionSlider->maximum();
    ui.playlistAudioPositionSlider->setValue(
        sliderMax * effects->audioPosition / effects->audioDuration);

    QString labelText = "%1:%2:%3/%4:%5:%6";

    int currentSeconds = effects->audioPosition/GST_SECOND%60,
        currentMinutes = effects->audioPosition/GST_SECOND/60%60,
        currentHours   = effects->audioPosition/GST_SECOND/3600;

    labelText = labelText.arg(QString::number(currentHours));
    labelText = labelText.arg(QString::number(currentMinutes),2,QChar('0'));
    labelText = labelText.arg(QString::number(currentSeconds),2,QChar('0'));

    int durationSeconds = effects->audioDuration/GST_SECOND%60,
        durationMinutes = effects->audioDuration/GST_SECOND/60%60,
        durationHours   = effects->audioDuration/GST_SECOND/3600;

    labelText = labelText.arg(QString::number(durationHours));
    labelText = labelText.arg(QString::number(durationMinutes),2,QChar('0'));
    labelText = labelText.arg(QString::number(durationSeconds),2,QChar('0'));

    ui.playlistAudioPositionLabel->setText(labelText);
}

#endif // _PLAYLISTTAB_HPP_
