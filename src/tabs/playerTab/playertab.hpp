
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
#include "src/parser/parser.h"
#include "../../widgets/playerTabPlaylist/playertabplaylistitem.hpp"


PlayerTab::PlayerTab (QWidget *parent) 
    : QWidget (parent) {
    ui.setupUi(this);

    createWidgets  ();
    setupWidgets   ();
    connectWidgets ();
    updateIcons    ();
}

PlayerTab::~PlayerTab () {

}

void PlayerTab::createWidgets () {
    delegate = QSharedPointer<PlayerTabItemDelegate>::create();
}

void PlayerTab::setupWidgets () {
    ui.playerTagFrame->hide();
    ui.playerAllTracksListWidget->hide();
    ui.playerSearchLineEdit->hide();
    ui.playerSearchButton->hide();

    ui.playerAllTracksListWidget->setItemDelegate(delegate.data());

    audioTimer.setInterval(50);
}

void PlayerTab::connectWidgets () {
    connect(ui.playerTogglePlaylistButton, &QPushButton::clicked, this, &PlayerTab::togglePlaylistView);
//  search
    connect(ui.playerSearchButton, &QPushButton::clicked, this, &PlayerTab::toggleSearchLineView);
    connect(ui.playerSearchLineEdit, &QLineEdit::textChanged, this, &PlayerTab::onLineEditTextChanged);

    connect(ui.playerAllTracksListWidget, &QListWidget::itemClicked, this, &PlayerTab::onPlayerPlaylistItemClicked);
//  control buttons
    connect(ui.playerPrevButton, &QPushButton::clicked, this, &PlayerTab::onPrevButtonClicked);
    connect(ui.playerPauseButton, &QPushButton::clicked, this, &PlayerTab::onPauseButtonClicked);
    connect(ui.playerNextButton, &QPushButton::clicked, this, &PlayerTab::onNextButtonClicked);
//  timer
    connect(&audioTimer, &QTimer::timeout, this, &PlayerTab::updateAudioInfo);
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
}

void PlayerTab::setExtensions (const QStringList &_extensions) {
    extensions = _extensions;
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

void PlayerTab::onLineEditTextChanged (const QString &newText) {
    foreach (QListWidgetItem* item, playlistItems) {
        auto widget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(item));
        if (QString::fromStdString(widget->filePath()).contains(newText))
            item->setHidden(false);
        else
            item->setHidden(true);
    }
}

void PlayerTab::onPrevButtonClicked () {
    int id = playlistItems.indexOf(currentAudio);
    if (id != -1 && id > 0) {
        currentAudio = playlistItems.at(id-1);
        auto widget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(widget->filePath());
        ui.playerAllTracksListWidget->setCurrentItem(currentAudio);
    }
}

void PlayerTab::onPauseButtonClicked () {
    if (currentAudio != nullptr) {
        effects->togglePipelineState();

        if (ui.playerPauseButton->getState() == 0)
            audioTimer.stop();
        else    
            audioTimer.start();
    } else
        ui.playerPauseButton->setState(0);
}

void PlayerTab::onNextButtonClicked () {
    int id = playlistItems.indexOf(currentAudio);
    if (id != -1 && id < playlistItems.size()-1) {
        currentAudio = playlistItems.at(id+1);
        auto widget = qobject_cast<PlayerTabPlaylistItem*>(ui.playerAllTracksListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(widget->filePath());
        ui.playerAllTracksListWidget->setCurrentItem(currentAudio);
    }

}

void PlayerTab::onPlayerPlaylistItemClicked (QListWidgetItem *item) {
    currentAudio = item;
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
