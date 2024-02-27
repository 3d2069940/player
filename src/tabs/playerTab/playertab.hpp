
#ifndef _PLAYERTAB_HPP_
#define _PLAYERTAB_HPP_


//*******************************************************//
// Qt5
//*******************************************************//
#include <QDir>
#include <QStandardPaths>

#include <QDebug>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "playertab.h"
//*******************************************************//
// Classes definition
//*******************************************************//
#include "src/tabs/playerTab/playertabitemdelegation.h"
#include "src/widgets/playerTabPlaylist/playertabplaylistitem.h"
#include "togglebutton.hpp"
#include "src/effects/effects.h"
#include <qfileinfo.h>
#include <qlistwidget.h>
#include <qpushbutton.h>
#include <qstandardpaths.h>
#include <qvariant.h>
#include "../../widgets/playerTabPlaylist/playertabplaylistitem.hpp"


PlayerTab::PlayerTab (QWidget *parent) 
    : QWidget (parent) {
    ui.setupUi(this);

    createWidgets ();
    setupTab      ();
    connectTab    ();
    updateIcons   ();
}

PlayerTab::~PlayerTab () {

}

void PlayerTab::createWidgets () {
    delegate = QSharedPointer<PlayerTabItemDelegate>::create();
    updatePlayList();
}

void PlayerTab::setupTab () {
    ui.playerTagFrame->hide();
    ui.playerAllTracksListWidget->hide();
    ui.playerSearchLineEdit->hide();
    ui.playerSearchButton->hide();

    ui.playerAllTracksListWidget->setItemDelegate(delegate.data());

    audioTimer.setInterval(50);
}

void PlayerTab::connectTab () {
    connect(ui.playerTogglePlaylistButton, &QPushButton::clicked, this, &PlayerTab::togglePlaylistView);
    connect(ui.playerSearchButton, &QPushButton::clicked, this, &PlayerTab::toggleSearchLineView);
    connect(ui.playerAllTracksListWidget, &QListWidget::itemClicked, this, &PlayerTab::onPlayerPlaylistItemClicked);
//  control buttons
    connect(ui.playerPauseButton, &QPushButton::clicked, this, &PlayerTab::onPauseButtonClicked);
//  timer
    connect(&audioTimer, &QTimer::timeout, this, &PlayerTab::updateAudioInfo);
}

void PlayerTab::updatePlayList () {
    if (musicFolders.isEmpty())
        musicFolders.append(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    QFileInfoList musicFiles;

    foreach (const QString &folder, musicFolders)
        parseMusic(folder, musicFiles);

    QSize size (ui.playerAllTracksListWidget->width(), 40);
    PlayerPlaylistButtonValue buttonValue;
    foreach (const QFileInfo &musicFile, musicFiles) {
        QListWidgetItem *item = new QListWidgetItem (ui.playerAllTracksListWidget);
        item->setSizeHint(size);

        buttonValue.listWidget = ui.playerAllTracksListWidget;
        buttonValue.filePath   = musicFile.absoluteFilePath();
        buttonValue.item       = item;

        QVariant value = QVariant::fromValue(buttonValue);
        auto *playlistItem = new PlayerTabPlaylistItem (musicFile.fileName(), value);
        ui.playerAllTracksListWidget->setItemWidget(item, playlistItem);
    }
}

void PlayerTab::parseMusic (const QString &path, QFileInfoList &musicFiles) {
    if (extensions.isEmpty())
        return;
    QDir musicDir (path);
    musicDir.setNameFilters(extensions);
    musicFiles += musicDir.entryInfoList(QDir::Files);

    QDir folderDir (path);
    folderDir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList folderList = folderDir.entryInfoList();
    foreach (const QFileInfo &folder, folderList)
        parseMusic(folder.absoluteFilePath(), musicFiles);
}

void PlayerTab::setEffects (Effects *_effects) {
    effects = _effects;
}

void PlayerTab::updateIcons () {
    QPalette palette = qApp->palette();
    QColor   backgroundColor = palette.color(QPalette::Window);
    
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

void PlayerTab::setMusicFolders (const QStringList &_musicFolders) {
    musicFolders = _musicFolders;
    updatePlayList();
}

void PlayerTab::setExtensions (const QStringList &_extensions) {
    extensions = _extensions;
    updatePlayList();
}

void PlayerTab::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange)
        updateIcons();
    QWidget::changeEvent(event);
}

void PlayerTab::toggleSearchLineView () {
    ui.playerSearchLineEdit->setVisible(!ui.playerSearchLineEdit->isVisible());
}

void PlayerTab::togglePlaylistView () {
    ui.playerAllTracksListWidget->setVisible(!ui.playerAllTracksListWidget->isVisible());
    ui.playerSearchButton->setVisible(!ui.playerSearchButton->isVisible());
    ui.playerSearchLineEdit->setVisible(false);
}

void PlayerTab::onPauseButtonClicked () {
    effects->togglePipelineState();

    if (ui.playerPauseButton->getState() == 0)
        audioTimer.stop();
    else
        audioTimer.start();
}

void PlayerTab::onPlayerPlaylistItemClicked (QListWidgetItem *item) {
    auto itemWidget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(item));
    effects->changePlayingAudio(itemWidget->filePath());
    ui.playerPauseButton->setState(1);
    audioTimer.start();
}

void PlayerTab::updateAudioInfo () {
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

#endif // _PLAYERTAB_HPP
