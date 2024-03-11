
#ifndef _PLAYERTAB_HPP_
#define _PLAYERTAB_HPP_


//*******************************************************//
// Qt5
//*******************************************************//
#include <QDir>
#include <QStandardPaths>

#include <QDebug>
#include <qdialogbuttonbox.h>
#include <qevent.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "playertab.h"
//*******************************************************//
// Classes definition
//*******************************************************//
#include "src/db/db.h"
#include "src/tabs/playlistitemdelegate.h"
#include "src/widgets/playerTabPlaylist/playertabplaylistitem.h"
#include "togglebutton.hpp"
#include "src/effects/effects.h"
#include "src/parser/parser.h"
#include "src/widgets/playerTabPlaylist/playertabplaylistitem.hpp"


PlayerTab::PlayerTab (DataBase *_db, QWidget *parent) 
    : QWidget (parent), db(_db) {
    ui.setupUi(this);

    createWidgets  ();
    setupWidgets   ();
    connectWidgets ();
    updateIcons    ();
}

PlayerTab::~PlayerTab () {

}

void PlayerTab::createWidgets () {
    dialog   = QSharedPointer<QDialog>::create();
}

void PlayerTab::setupWidgets () {
    ui.playerTagFrame->hide();
    ui.playerAllTracksListWidget->hide();
    ui.playerSearchLineEdit->hide();
    ui.playerSearchButton->hide();

    dialogUi.setupUi(dialog.data());

    ui.playerAllTracksListWidget->setItemDelegate(&delegate);
    dialogUi.listWidget->setItemDelegate(&delegate);

    audioTimer.setInterval(50);
}

void PlayerTab::connectWidgets () {
    connect(ui.playerTogglePlaylistButton, &QPushButton::clicked, this, &PlayerTab::togglePlaylistView);
//  search
    connect(ui.playerSearchButton, &QPushButton::clicked, this, &PlayerTab::toggleSearchLineView);
    connect(ui.playerSearchLineEdit, &QLineEdit::textChanged, this, &PlayerTab::onLineEditTextChanged);

    connect(ui.playerAllTracksListWidget, &QListWidget::itemClicked, this, &PlayerTab::onPlaylistItemClicked);
//  control buttons
    connect(ui.playerPrevButton, &QPushButton::clicked, this, &PlayerTab::onPreviousButtonClicked);
    connect(ui.playerPauseButton, &QPushButton::clicked, this, &PlayerTab::onPauseButtonClicked);
    connect(ui.playerNextButton, &QPushButton::clicked, this, &PlayerTab::onNextButtonClicked);
//  slider
    connect(ui.playerAudioSlider, &QSlider::valueChanged, this, &PlayerTab::onSliderValueChanged);
    connect(ui.playerAudioSlider, &QSlider::sliderReleased, this, &PlayerTab::onSliderReleased);
    connect(ui.playerAudioSlider, &QSlider::sliderPressed, this, &PlayerTab::onSliderPressed);
//  timer
    connect(&audioTimer, &QTimer::timeout, this, &PlayerTab::updateAudioInfo);
//  dialog window
    connect(dialogUi.buttonBox, &QDialogButtonBox::accepted, this, &PlayerTab::addToPlaylist);
}

void PlayerTab::updatePlayList () {
    if (musicFolders.isEmpty())
        musicFolders.append(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    QFileInfoList musicFiles;
    parser->getMusicFiles(musicFiles);

    QSize size (ui.playerAllTracksListWidget->width(), 40);
    PlayerPlaylistButtonValue buttonValue;
    foreach (const QFileInfo &musicFile, musicFiles) {
        QListWidgetItem *item = new QListWidgetItem (ui.playerAllTracksListWidget);
        playlistItems.append(item);
        item->setSizeHint(size);

        buttonValue.listWidget = ui.playerAllTracksListWidget;
        buttonValue.filePath   = musicFile.absoluteFilePath();
        buttonValue.item       = item;

        QVariant value = QVariant::fromValue(buttonValue);
        auto *playlistItem = new PlayerTabPlaylistItem (musicFile.fileName(), value);
        connect(playlistItem, &PlayerTabPlaylistItem::onAddToPlaylistClicked, this, &PlayerTab::addToPlaylistClicked);
        ui.playerAllTracksListWidget->setItemWidget(item, playlistItem);
    }
}

void PlayerTab::setEffects (Effects *_effects) {
    effects = _effects;
}

void PlayerTab::setParser (Parser *_parser) {
    parser = _parser;
    musicFolders = parser->getMusicFolders();
    extensions   = parser->getExtensions();
    updatePlayList();
}

void PlayerTab::updateIcons () {
    QColor   backgroundColor = palette().color(QPalette::Window);
    QString iconPath = backgroundColor.value() < 128 ? QStringLiteral(":icons/white/") :
                                                       QStringLiteral(":icons/black/");

    ui.playerTogglePlaylistButton->setIcons({QIcon(iconPath+"playlist_show.svg"),
                                                 QIcon(iconPath+"playlist_hide.svg")});
    ui.playerSearchButton->setIcon(QIcon(iconPath+"search.svg"));
//  control buttons
    ui.playerPrevButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playerRepeatButton->setIcon(QIcon(iconPath+"repeat_all.svg"));
    ui.playerPauseButton->setIcons({QIcon(iconPath+"play_music.svg"),
                                    QIcon(iconPath+"pause_music.svg")});
    ui.playerShuffleButton->setIcon(QIcon(iconPath+"shuffle_playlist.svg"));
    ui.playerNextButton->setIcon(QIcon(iconPath+"next_track.svg"));
}

void PlayerTab::updateAlbumCover () {
    if (albumCover.isNull())
        ui.playerAudioCoverLabel->setText("*LOGO HERE*");
    else {
        int albumCoverSize = qMin(ui.playerHLine_1->width(), height()-150);
        QImage image = albumCover.scaled(QSize(albumCoverSize, albumCoverSize),
                                     Qt::KeepAspectRatio);
        QPixmap pixmap    = QPixmap::fromImage(image);
        ui.playerAudioCoverLabel->setPixmap(pixmap);
    }
}

void PlayerTab::setMusicFolders (const QStringList &_musicFolders) {
    musicFolders = _musicFolders;
}

void PlayerTab::setExtensions (const QStringList &_extensions) {
    extensions = _extensions;
}

void PlayerTab::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange)
        updateIcons();
    QWidget::changeEvent(event);
}

void PlayerTab::resizeEvent (QResizeEvent *event) {
    if (!albumCover.isNull())
        updateAlbumCover();
    QWidget::resizeEvent(event);
}

void PlayerTab::showEvent (QShowEvent *event) {
    // if (parser->showAudioCover()) {
    //     auto itemWidget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(currentAudio));
    //     albumCover = parser->getAudioCover(itemWidget->filePath());
    //     updateAlbumCover();
    // }
    QWidget::showEvent(event);
}

void PlayerTab::toggleSearchLineView () {
    ui.playerSearchLineEdit->setVisible(!ui.playerSearchLineEdit->isVisible());
}

void PlayerTab::togglePlaylistView () {
    ui.playerAllTracksListWidget->setVisible(!ui.playerAllTracksListWidget->isVisible());
    ui.playerSearchButton->setVisible(!ui.playerSearchButton->isVisible());
    ui.playerSearchLineEdit->setVisible(false);
    updateAlbumCover();
}

/**
* This function is called when typing in QLineEdit
* hides elements that do not contain string occurrences
*/
void PlayerTab::onLineEditTextChanged (const QString &newText) {
    foreach (QListWidgetItem* item, playlistItems) {
        auto widget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(item));
        QString fileName = QString::fromStdString(widget->filePath());
        if (fileName.contains(newText, Qt::CaseInsensitive))
            item->setHidden(false);
        else
            item->setHidden(true);
    }
}

void PlayerTab::onPreviousButtonClicked () {
    if (ui.playerAllTracksListWidget->currentItem() == nullptr)
        return;
    playlistAudioActive = false;
    effects->setAudioList(ui.playerAllTracksListWidget);
    effects->setCurrentAudio<PlayerTabPlaylistItem>(ui.playerAllTracksListWidget->currentItem());
    effects->playPreviousAudio();
    std::string filePath = effects->getAudioFilePath<PlayerTabPlaylistItem>();
    albumCover = parser->getAudioCover(filePath);
    ui.playerPauseButton->setState(1);
    updateAlbumCover();
}

void PlayerTab::onPauseButtonClicked () {
    auto item = ui.playerAllTracksListWidget->currentItem();
    if (item == nullptr)
        return;
    effects->setAudioList(ui.playerAllTracksListWidget);
    effects->togglePipelineState();
    if (playlistAudioActive) {
        effects->setCurrentAudio<PlayerTabPlaylistItem>(item);
        audioTimer.start();
        playlistAudioActive = false;
    }
}

void PlayerTab::onNextButtonClicked () {
    auto item = ui.playerAllTracksListWidget->currentItem();
    if (item == nullptr)
        return;
    playlistAudioActive = false;
    effects->setAudioList(ui.playerAllTracksListWidget);
    effects->setCurrentAudio<PlayerTabPlaylistItem>(item);
    effects->playNextAudio();
    std::string filePath = effects->getAudioFilePath<PlayerTabPlaylistItem>();
    albumCover = parser->getAudioCover(filePath);
    ui.playerPauseButton->setState(1);
    updateAlbumCover();
}

void PlayerTab::onSliderValueChanged (int value) {
    Q_UNUSED(value);
}

void PlayerTab::onSliderReleased () {
    effects->updateAudioDuration();
    int max   = ui.playerAudioSlider->maximum(),
        value = ui.playerAudioSlider->value();
    effects->seekPlayingPosition(effects->audioDuration*value/max);
    audioTimer.start();
}

void PlayerTab::onSliderPressed () {
    audioTimer.stop();
}

void PlayerTab::onPlaylistItemClicked (QListWidgetItem *item) {
    effects->setAudioList(ui.playerAllTracksListWidget);
    effects->setCurrentAudio<PlayerTabPlaylistItem>(item);
    std::string filePath = effects->getAudioFilePath<PlayerTabPlaylistItem>();
    albumCover = parser->getAudioCover(filePath);
    updateAlbumCover();
    ui.playerPauseButton->setState(1);
    audioTimer.start();
}

void PlayerTab::addToPlaylistClicked (const QString &_musicFile) {
    dialogUi.listWidget->clear();
    dialogUi.listWidget->addItems(db->getTableNames());

    musicFile = _musicFile;

    dialog->show();
}

void PlayerTab::addToPlaylist () {
    auto item = dialogUi.listWidget->currentItem();
    if (item == nullptr)
        return;
    QString tableName = item->text();
    db->writeValue(tableName.toStdString(), musicFile.toStdString());
}

void PlayerTab::updateAudioInfo () {
    QSignalBlocker blocker (ui.playerAudioSlider);
    effects->updateAudioDuration();
    effects->updateAudioPosition();

    int sliderMax = ui.playerAudioSlider->maximum();
    ui.playerAudioSlider->setValue(sliderMax*effects->audioPosition/effects->audioDuration);

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

    ui.playerAudioLabel->setText(labelText);
}

void PlayerTab::onAudioStateChanged () {
    audioTimer.stop();
    playlistAudioActive = true;
    ui.playerPauseButton->setState(0);
}

#endif // _PLAYERTAB_HPP
