
#pragma once

//***********************************************************//
// STL
//***********************************************************//
#include <random>
#include <algorithm>
//***********************************************************//
// Qt5
//***********************************************************//
#include <QDir>
#include <QIcon>
#include <QDebug>
#include <QColor>
#include <QPalette>
#include <QEasingCurve>
#include <QStyleFactory>
#include <QStandardPaths>
#include <QLinearGradient>
#include <QPropertyAnimation>
//***********************************************************//
// Third-party libs
//***********************************************************//
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <modfile.h>
#include <opusfile.h>
#include <mpegfile.h>
#include <flacfile.h>
#include <aifffile.h>
#include <tpropertymap.h>
#include <attachedpictureframe.h>
//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "metaTypes.h"
#include "player.h"
#include "parser/parser.hpp"

//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "db/db.hpp"
#include "effects/effects.hpp"
#include "connector/connector.cpp"
#include "playlistWidgetItem.h"
#include "toggleButton.hpp"
#include "playlistWidgetItem.hpp"
#include "widgets/effects/presetDialogWindow.hpp"
#include "widgets/settings/directoryListView.hpp"
#include "widgets/delegation/playlistDelegate.hpp"

MainWindowUI::MainWindowUI () {
    ui.setupUi(this);
    
    setWindowTitle("Player");

    effects = std::make_unique<Effects>();

    parser.parseConfigFile();
    parser.parsePresetFile();

    parser.extractPresetInfo<EqualizerPreset> (ui.equalizerPresetsComboBox,"Equalizer");
    parser.extractPresetInfo<DelayPreset>     (ui.delayPresetsComboBox,"Delay");
    parser.extractPresetInfo<FilterPreset>    (ui.filterPresetsComboBox, "Filter");
    parser.extractPresetInfo<PitchPreset>     (ui.pitchPresetComboBox,"Pitch");
    parser.extractPresetInfo<CompressorPreset>(ui.compressorPresetComboBox,"Compressor");

    createWidgets   ();

    // if (parser.getConfigYaml()["DataBasePath"].IsDefined()) {
    //     std::string path = parser.getConfigYaml()["DataBasePath"].as<std::string>();
    //     db = std::make_unique<DataBase>(path, this);
    //     db->open();
    // }

    Connector::connectWidgets(this);

    setupWidgets    ();

    audioVisualizingTimer.start();
}

MainWindowUI::~MainWindowUI() {
    ui.visualizingCustomPlot->clearPlottables();

    effects.reset();
    presetDialogWindow.clear();
    audioPositionTimer.stop();
    audioPanningTimer.stop();
    audioVisualizingTimer.stop();
}

/**
 * @brief 
 * @return Does not return any value
 */
void MainWindowUI::createWidgets () {
//  
    presetDialogWindow = QSharedPointer<PresetDialogWindow>::create();
//      
    updatePlaylistWidget();
//
    previousShortcut    = QSharedPointer<QShortcut>::create(QKeySequence(Qt::Key_F1), this);
    playPauseShortcut   = QSharedPointer<QShortcut>::create(QKeySequence(Qt::Key_F2), this);
    nextShortcut        = QSharedPointer<QShortcut>::create(QKeySequence(Qt::Key_F3), this);
    // seekForwardShortcut = QSharedPointer<QShortcut>::create(QKeySequence(Qt::Key_Right), ui.playerTab);
//
    extMenu = QSharedPointer<QMenu>::create(ui.settingsExtensionsButton);

    QStringList extActions = {"mp3", "wav", "flac", "aac", "ogg", "opus", "wma", "aiff", "ac3", "amr"};
    foreach (const QString &ext, extActions) {
        extMenu->addAction(ext);
    }
}

void MainWindowUI::setupWidgets () {
    // 
    audioPositionTimer.setInterval(300);
    audioPanningTimer.setInterval(100);
    audioVisualizingTimer.setInterval(100);
    
    ui.playerSearchButton->hide();
    ui.playerTagListFrame->hide();
    ui.playerPlayListWidget->hide();
    ui.playerSearchLineEdit->hide();

    playlistDelegate = QSharedPointer<PlaylistDelegate>::create();
    ui.playerPlayListWidget->setItemDelegate(playlistDelegate.data());

    presetDialogWindow->setFixedSize(250, 100);

    ui.delaySurroundDelayButton->      setLabels({"Off", "On"});
    ui.compressorExpanderToggleButton->setLabels({"Compressor", "Expander"});
    ui.compressorKneeToggleButton->    setLabels({"Hard-knee", "Soft-knee"});
            
    setupIcons ();
    setupVisualizationWidgets();
    setupSettingsWidgets();
}

void MainWindowUI::setupVisualizationWidgets () {
    ui.visualizingCustomPlot->xAxis->setVisible(false);
    ui.visualizingCustomPlot->yAxis->setVisible(false);

    ui.visualizingCustomPlot->axisRect()->setAutoMargins(QCP::msNone);
    ui.visualizingCustomPlot->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    ui.visualizingCustomPlot->yAxis->setRange(0, 120);
    ui.visualizingCustomPlot->xAxis->setRange(0, 10);
    
    ui.visualizingCustomPlot->setBackground(palette().color(QPalette::Window));
    
    gradient.setStart (width()/2, height()/2);
    gradient.setFinalStop(width()/2, height()-ui.visualizingCustomPlot->height());
      
    gradient.setColorAt(1, qApp->palette().color(QPalette::Window));
    gradient.setColorAt(0, qApp->palette().color(QPalette::Highlight));
      
    bars  = new QCPBars(ui.visualizingCustomPlot->xAxis, ui.visualizingCustomPlot->yAxis);
    
    bars->setPen(Qt::NoPen);
    bars->setWidth(1);
    bars->setBrush(QBrush(gradient));
}

void MainWindowUI::setupSettingsWidgets () {
    QString labelText = QStringLiteral("Audio Extensions to Parse: %1");
    // ui.settingsExtensionsLabel->setText(labelText.arg(extensions.join(QChar(','))));
    //  Add installed themes to settings combobox
    QStringList styleNames = QStyleFactory::keys();
    ui.settingsColorThemeComboBox->addItems(styleNames);
    //  Set current theme
    QStyle *style = QApplication::style();
    ui.settingsColorThemeComboBox->setCurrentText(style->objectName());

    if (parser.getShowAudioCover())
        ui.settingsAudioCoverCheckBox->click();

    if (parser.getFlatButtons())
        ui.settingsFlatButtonsCheckBox->click();

    if (parser.getSaveLastAudio())
        ui.settingsSaveLastAudioCheckBox->click();

    if (parser.getSaveLastAudio() && parser.getConfigYaml()["LastAudioFilePath"].IsDefined()) {
        for (int i = 0; i < playlistItems.size(); ++i) {
            auto itemWidget = ui.playerPlayListWidget->itemWidget(playlistItems.at(i));
            auto playlistWidget = qobject_cast<PlaylistWidgetItem*>(itemWidget);
            if (playlistWidget->filePath() == parser.getConfigYaml()["LastAudioFilePath"].as<std::string>()) {
                currentAudio = playlistItems.at(i);
                effects->changePlayingAudio(playlistWidget->filePath());
                effects->togglePipelineState();
                ui.playerPlayListWidget->setCurrentRow(i);
                break;
            }
        }
    }
    if (parser.getPlayAtStartup()) {
        ui.settingsPlayAtStartupCheckBox->click();
        ui.playerPauseButton->click();
    }

    extMenu->setMinimumWidth(300);
    ui.settingsExtensionsButton->setMenu(extMenu.data());

    ui.settingsDirectoryListView->markSelectedDirs(parser.getMusicFolders());
    ui.settingsDirectoryListView->hide();
}

void MainWindowUI::setupIcons () {
    QPalette palette = qApp->palette();
    QColor   backgroundColor = palette.color(QPalette::Window);
    
    QString iconPath = backgroundColor.value() < 128 ? QStringLiteral(":icons/white/") :
                                                       QStringLiteral(":icons/black/");

    ui.playerPreviousButton->setIcon(QIcon(iconPath+"previous_track.svg"));

    ui.playerRepeatButton->setIcons({QIcon(iconPath+"repeat_all.svg")});

    ui.playerPauseButton->setIcons({QIcon(iconPath+"play_music.svg"),
                                    QIcon(iconPath+"pause_music.svg")});
    ui.playerShuffleButton->setIcon(QIcon(iconPath+"shuffle_playlist.svg"));
    ui.playerNextButton->setIcon(QIcon(iconPath+"next_track.svg"));
    ui.playerSearchButton->setIcon(QIcon(iconPath+"search.svg"));
        
    ui.playerTagListVisibilityButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playerPlaylistVisibilityButton->setIcon(QIcon(iconPath+"playlist_hide.svg"));
}

void MainWindowUI::setupAnimations () {

}

void MainWindowUI::updatePlaylistWidget () {
    //  Add playlist items
    ui.playerPlayListWidget->clear();
    QFileInfoList musicFiles;
    if (parser.getMusicFolders().size() == 0)
        parser.setMusicFolders(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));

    foreach (const QString &folder, parser.getMusicFolders())
        parser.parseMusic(folder, musicFiles);
    
    QSize               size (ui.playerPlayListWidget->width(), 40);
    PlaylistButtonValue playlistValue;
    foreach (const QFileInfo &file, musicFiles) {
        QListWidgetItem *item         = new QListWidgetItem (ui.playerPlayListWidget);
        playlistItems.append(item);
        
        playlistValue.filePath   = file.absoluteFilePath ();
        playlistValue.listWidget = ui.playerPlayListWidget;
        playlistValue.mainwindow = this;
        playlistValue.item       = item;
        
        QVariant value = QVariant::fromValue(playlistValue);
        PlaylistWidgetItem *playlistItem = new PlaylistWidgetItem (file.fileName(), value);
        connect(playlistItem, &PlaylistWidgetItem::infoActionShow, this, &MainWindowUI::showTagListWidget);
        
        item->setSizeHint(size);
        ui.playerPlayListWidget->setItemWidget(item, playlistItem);
    }

    stopAudio = ui.playerPlayListWidget->item(ui.playerPlayListWidget->count()-1);
}

void MainWindowUI::serializeEqualizerParams  (YAML::Node *node) {
    node->push_back(ui.equalizer29HzSlider->value());
    node->push_back(ui.equalizer59HzSlider->value());
    node->push_back(ui.equalizer119HzSlider->value());
    node->push_back(ui.equalizer237HzSlider->value());
    node->push_back(ui.equalizer474HzSlider->value());
    node->push_back(ui.equalizer1000HzSlider->value());
    node->push_back(ui.equalizer2000HzSlider->value());
    node->push_back(ui.equalizer4000HzSlider->value());
    node->push_back(ui.equalizer8000HzSlider->value());
    node->push_back(ui.equalizer16000HzSlider->value());
}

void MainWindowUI::serializeDelayParams      (YAML::Node *node) {
    node->push_back(ui.delayDelaySpinBox->value());
    node->push_back(ui.delaySurroundDelayButton->getState() != 0);
    node->push_back(ui.delayIntensityDial->value());
    node->push_back(ui.delayMaxDelaySpinBox->value());
    node->push_back(ui.delayFeedbackDial->value());
}

void MainWindowUI::serializeFilterParams     (YAML::Node *node) {
    node->push_back(ui.filterModeToggleButton->text() != "Low Pass");
    node->push_back(ui.filterCutoffSpinBox->value());
    node->push_back(ui.filterRippleSpinBox->value());
    node->push_back(ui.filterPolesSpinBox->value());
    node->push_back(ui.filterFilterTypeButton->text() != "I");
}

void MainWindowUI::serializePitchParams      (YAML::Node *node) {
    node->push_back(ui.pitchTempoDial->value());
    node->push_back(ui.pitchPitchDial->value());
    node->push_back(ui.pitchRateDial->value());
    node->push_back(ui.pitchOutputRateDial->value());
}

void MainWindowUI::serializeCompressorParams (YAML::Node *node) {
    node->push_back(ui.compressorExpanderToggleButton->getState() != 0);
    node->push_back(ui.compressorKneeToggleButton->getState() != 0);
    node->push_back(ui.compressorRatioDial->value());
    node->push_back(ui.compressorThresholdDial->value());
}

void MainWindowUI::changeEqualizerParams  (const QVariant &data) {
    EqualizerPreset params = data.value<EqualizerPreset>();
    ui.equalizer29HzSlider->setValue(params.slider29Hz);
    ui.equalizer59HzSlider->setValue(params.slider59Hz);
    ui.equalizer119HzSlider->setValue(params.slider119Hz);
    ui.equalizer237HzSlider->setValue(params.slider237Hz);
    ui.equalizer474HzSlider->setValue(params.slider474Hz);
    ui.equalizer1000HzSlider->setValue(params.slider1kHz);
    ui.equalizer2000HzSlider->setValue(params.slider2kHz);
    ui.equalizer4000HzSlider->setValue(params.slider4kHz);
    ui.equalizer8000HzSlider->setValue(params.slider8kHz);
    ui.equalizer16000HzSlider->setValue(params.slider16kHz);
}

void MainWindowUI::changeDelayParams      (const QVariant &data) {
    DelayPreset params = data.value<DelayPreset>();
    ui.delayDelaySpinBox->setValue(params.delay);
    if ((ui.delaySurroundDelayButton->getState() == 1) != params.surroundDelay)
        ui.delaySurroundDelayButton->click();
    ui.delayIntensityDial->setValue(params.intensity);
    ui.delayMaxDelaySpinBox->setValue(params.maxDelay);
    ui.delayFeedbackDial->setValue(params.feedback);
}

void MainWindowUI::changeFilterParams     (const QVariant &data) {
    FilterPreset params = data.value<FilterPreset>();
    if ((ui.filterModeToggleButton->text() == "Low Pass") == params.mode)
        ui.filterModeToggleButton->click();
    ui.filterCutoffSpinBox->setValue(params.cutoff);
    ui.filterRippleSpinBox->setValue(params.ripple);
    ui.filterPolesSpinBox->setValue(params.poles);
    if ((ui.filterFilterTypeButton->text() == "I") == params.filterType)
        ui.filterFilterTypeButton->click();
}

void MainWindowUI::changePitchParams      (const QVariant &data) {
    PitchPreset params = data.value<PitchPreset>();
    ui.pitchTempoDial->setValue(params.tempo);
    ui.pitchPitchDial->setValue(params.pitch);
    ui.pitchRateDial->setValue(params.rate);
    ui.pitchOutputRateDial->setValue(params.outputRate);
}

void MainWindowUI::changeCompressorParams (const QVariant &data) {
    CompressorPreset params = data.value<CompressorPreset>();
    if (ui.compressorExpanderToggleButton->getState() != params.compressor)
        ui.compressorExpanderToggleButton->click();
    if (ui.compressorKneeToggleButton->getState() != params.kneeType)
        ui.compressorKneeToggleButton->click();
    ui.compressorRatioDial->setValue(params.ratio);
    ui.compressorThresholdDial->setValue(params.threshold);
}

//********Others********//
void MainWindowUI::updateCurrentAudioCover (const std::string &filePath) {
    if (!parser.getShowAudioCover()) {
        albumCoverSet = false;
        ui.playerMusicPicture->setText("*LOGO HERE*");
        return;
    }
    TagLib::FileRef file (filePath.c_str());
    QByteArray imageData;
    if (TagLib::MPEG::File *mpegFile = dynamic_cast<TagLib::MPEG::File*>(file.file())) {
        if (auto mpegTag   = mpegFile->ID3v2Tag()) {
            if (auto mpegFrame = mpegTag->frameList("APIC"); mpegFrame.size() > 0) {
                auto mpegImage = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(mpegFrame.front());
                if (mpegImage)
                    imageData = QByteArray (mpegImage->picture().data(), mpegImage->picture().size());
            }
        }
  } else if (TagLib::FLAC::File *flacFile = dynamic_cast<TagLib::FLAC::File*>(file.file())) {
        if (auto flacTag = flacFile->pictureList(); flacTag.size() > 0) {
            auto flacImage = flacTag.front();
            if (flacImage)
                imageData = QByteArray (flacImage->data().data(), flacImage->data().size());
        }
  } else if (TagLib::Ogg::Opus::File *opusFile = dynamic_cast<TagLib::Ogg::Opus::File*>(file.file())) {
        auto opusTag = opusFile->tag();
        if (auto opusFrame = opusTag->pictureList(); opusFrame.size() > 0) {
            auto opusImage = opusFrame.front();
            if (opusImage)
                imageData = QByteArray (opusImage->data().data(), opusImage->data().size());
        }
    }
    if (imageData.size() > 0) {
        int albumCoverSize = qMin(ui.playerHLine_1->width(), width()/2-10);
        albumCover.loadFromData(imageData, "JPEG");
        QImage image = albumCover.scaled(QSize(albumCoverSize, albumCoverSize),
                                         Qt::KeepAspectRatio);
        QPixmap pixmap = QPixmap::fromImage(image);
        ui.playerMusicPicture->setPixmap(pixmap);
        ui.playerMusicPicture->setScaledContents(true);
        albumCoverSet = true;
  } else {
        albumCoverSet = false;
        ui.playerMusicPicture->setText("*LOGO HERE*");
  }
}

void MainWindowUI::setupPlayerMusicIcons () {

}

void MainWindowUI::updateAudioPositionLabel (gint64 position, gint64 duration) {
    long long hours_dur, minutes_dur, seconds_dur, 
              hours_pos, minutes_pos, seconds_pos;
    QString string = "%1:%2:%3/%4:%5:%6";
    
    seconds_pos = GST_TIME_AS_SECONDS(position);
    minutes_pos = seconds_pos/60;
    hours_pos   = seconds_pos/3600;

    seconds_dur = GST_TIME_AS_SECONDS(duration);
    minutes_dur = seconds_dur/60;
    hours_dur   = seconds_dur/3600;
    
    string = string.arg(hours_pos), 
    string = string.arg(minutes_pos % 60, 2, 10, QChar('0')),
    string = string.arg(seconds_pos % 60, 2, 10, QChar('0')), 
    string = string.arg(hours_dur),
    string = string.arg(minutes_dur % 60, 2, 10, QChar('0')),
    string = string.arg(seconds_dur % 60, 2, 10, QChar('0'));
    
    ui.playerProgressLabel->setText(string);
}

void MainWindowUI::bandsChanged (int newValue) {
    effects->changeSpectrumProps(CODES::Bands, newValue);
    
    ui.visualizingCustomPlot->xAxis->setRange(0, newValue);
//     ui.visualizingCustomPlot->rescaleAxes();
}

void MainWindowUI::updateRateChanged (int newValue) {
    effects->changeSpectrumProps(CODES::UpdateRate, newValue);
    
    audioVisualizingTimer.setInterval(newValue);
}

void MainWindowUI::thresholdChanged (int newValue) {
    effects->changeSpectrumProps(CODES::AudioThreshold, newValue);
//     ui.visualizingCustomPlot->yAxis->setRange(0, 60);
//     ui.visualizingCustomPlot->rescaleAxes();
}

void MainWindowUI::updateVisualizingWidget () {
    if (!effects->magnitudesChanged.load())
        return;
                    
    QVector<double> dataX (effects->currentMagnitudes.size()), 
                    dataY (effects->currentMagnitudes.begin(), effects->currentMagnitudes.end());
    
    int thresholdValue = ui.visualizingThresholdSpinBox->value();
    
    for (int i = 0; i < dataX.size(); ++i) {
        dataY[i] -= thresholdValue;
        dataX[i] = i+0.5;
    }
    
    bars->setData(dataX, dataY, true);
    
    ui.visualizingCustomPlot->replot();
    effects->magnitudesChanged.store(false);
}

//**************SLOTS**************//

void MainWindowUI::addNewPreset () {
    std::string presetName = presetDialogWindow->getLineInput();

    YAML::Node node;

    if (currentPresetType == "Equalizer") {
        serializeEqualizerParams(&node);
        parser.addToPreset<EqualizerPreset>(ui.equalizerPresetsComboBox, &node, 
                                                currentPresetType, presetName);
  } else if (currentPresetType == "Delay") {
        serializeDelayParams(&node);
        parser.addToPreset<DelayPreset>(ui.delayPresetsComboBox, &node, 
                                        currentPresetType, presetName);
  } else if (currentPresetType == "Filter") {
        serializeFilterParams(&node);
        parser.addToPreset<FilterPreset>(ui.filterPresetsComboBox, &node, 
                                        currentPresetType, presetName);
  } else if (currentPresetType == "Pitch") {
        serializeFilterParams(&node);
        parser.addToPreset<PitchPreset>(ui.pitchPresetComboBox, &node,
                                        currentPresetType, presetName);
  } else if (currentPresetType == "Compressor") {
        serializeCompressorParams(&node);
        parser.addToPreset<CompressorPreset>(ui.compressorPresetComboBox, &node, 
                                            currentPresetType, presetName);
    }

    presetDialogWindow->dialogLineEdit->clear();
    presetDialogWindow->hide();
}

void MainWindowUI::showTagListWidget (const QString& filePath) {
    ui.playerTagListAuthorLabel->clear();
    ui.playerTagListAuthorLabel->hide();

    ui.playerTagListAlbumLabel->clear();
    ui.playerTagListAlbumLabel->hide();

    ui.playerTagListTitleLabel->clear();
    ui.playerTagListTitleLabel->hide();

    ui.playerTagListSizeLabel->clear();
    ui.playerTagListChannelsLabel->clear();
    ui.playerTagListSampleRateLabel->clear();
    ui.playerTagListBitRateLabel->clear();

    QString authorStr     = "Author: %1",
            albumStr      = "Album: %1",
            codecStr      = "Codec: %1",
            titleStr      = "Title: %1",
            sizeStr       = "File size: %1 MB",
            channelStr    = "Channels: %1",
            sampleRateStr = "Sample rate: %1 Hz",
            bitRateStr    = "Bitrate: %1 KB/s";

    TagLib::FileRef file (filePath.toStdString().c_str());

    if (!file.isNull() && file.tag()) {
        TagLib::Tag *tag = file.tag();
        TagLib::AudioProperties *audioProperties = file.audioProperties();

        if (tag->artist().size()) {
            ui.playerTagListAuthorLabel->setText(authorStr.arg(tag->artist().toCString()));
            ui.playerTagListAuthorLabel->show();
        }
        if (tag->album().size()) {
            ui.playerTagListAlbumLabel->setText(albumStr.arg(tag->album().toCString()));
            ui.playerTagListAlbumLabel->show();
        }
        if (tag->title().size()) {
            ui.playerTagListTitleLabel->setText(titleStr.arg(tag->title().toCString()));
            ui.playerTagListTitleLabel->show();
        }
        ui.playerTagListSizeLabel      ->setText(sizeStr      .arg(file.file()->length() / 1024 / 1024.0, 0, 'f', 2));
        ui.playerTagListChannelsLabel  ->setText(channelStr   .arg(audioProperties->channels()));
        ui.playerTagListSampleRateLabel->setText(sampleRateStr.arg(audioProperties->sampleRate()));
        ui.playerTagListBitRateLabel   ->setText(bitRateStr   .arg(audioProperties->bitrate()));
    }
    ui.playerTagListFrame->show();
    
    ui.playerSearchButton->hide();
    ui.playerPlayListWidget->hide();
    ui.playerSearchLineEdit->hide();
    ui.playerPlaylistVisibilityButton->hide();
}

void MainWindowUI::closeTagListWidget () {
    ui.playerTagListFrame->hide();
    
    ui.playerSearchButton->show();
    ui.playerPlayListWidget->show();
    ui.playerPlaylistVisibilityButton->show();
}

void MainWindowUI::togglePlaylistView () {
    ui.playerSearchButton->setVisible(!ui.playerSearchButton->isVisible());
    ui.playerPlayListWidget->setVisible(!ui.playerPlayListWidget->isVisible());
    ui.playerSearchLineEdit->clear();
    ui.playerSearchLineEdit->setVisible(false);
}


void MainWindowUI::previousButtonClicked () {
    int currentAudioId = playlistItems.indexOf(currentAudio);
    if (currentAudioId > 0) {
        int audioId = currentAudioId-1;
        while (audioId >= 0 && playlistItems.at(audioId)->isHidden())
            audioId--;

        if (audioId >= 0) {
            currentAudio   = playlistItems.at(audioId);
            currentAudioId = audioId;
        }

        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        updateCurrentAudioCover(playlistItem->filePath());
        audioPositionTimer.start();
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
        ui.playerPauseButton->setState(1);
    }
}

void MainWindowUI::repeatButtonClicked () {
    // qDebug() << "repeat button clicked";
}

void MainWindowUI::pauseButtonClicked () {
    if (playlistItems.size() == 0)
        return;
        
    if (currentAudio == nullptr) {
        currentAudio = playlistItems.at(0);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        updateCurrentAudioCover(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
        ui.playerPauseButton->setState(1);
        audioPositionTimer.start();
        return;
    }
    effects->togglePipelineState();
    if (audioPositionTimer.isActive()) {
        ui.playerPauseButton->setState(0);
        audioPositionTimer.stop();
  } else {
        ui.playerPauseButton->setState(1);
        audioPositionTimer.start();
  }
}

void MainWindowUI::shuffleButtonClicked () {
    std::random_device randDevice;
    std::mt19937 g (randDevice());
    std::shuffle(playlistItems.begin(), playlistItems.end(), g);
}

void MainWindowUI::nextButtonClicked () {
    int currentAudioId = playlistItems.indexOf(currentAudio);
    if (currentAudio == nullptr) {
        currentAudio = playlistItems.at(0);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        updateCurrentAudioCover(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
        ui.playerPauseButton->setState(1);
        audioPositionTimer.start();
        return;
    }
    if (currentAudioId < playlistItems.size()-1 && currentAudio != stopAudio) {
        int audioId = currentAudioId+1;
        while (audioId < playlistItems.size() && playlistItems.at(audioId)->isHidden())
            audioId++;

        if (audioId < playlistItems.size()) {
            currentAudio   = playlistItems.at(audioId);
            currentAudioId = audioId;
        }

        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        updateCurrentAudioCover(playlistItem->filePath());
        audioPositionTimer.start();
        ui.playerPauseButton->setState(1);
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
    }
}

void MainWindowUI::searchTextChanged (const QString &searchText) {
    if (searchText == "")
        foreach (QListWidgetItem *item, playlistItems)
            item->setHidden(false);
    else {
        foreach (QListWidgetItem *item, playlistItems) {
            auto playlistItem = qobject_cast<PlaylistWidgetItem*>(ui.playerPlayListWidget->itemWidget(item));
            QString labelText = playlistItem->label->text();
            if (labelText.contains(searchText, Qt::CaseInsensitive))
                item->setHidden(false);
            else
                item->setHidden(true);
        }
  }
}

void MainWindowUI::toggleSearchLineView () {
    ui.playerSearchLineEdit->setVisible(!ui.playerSearchLineEdit->isVisible());
}

void MainWindowUI::playlistItemClicked (QListWidgetItem *item) {
    currentAudio   = item;
    PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
        (ui.playerPlayListWidget->itemWidget(item));
    effects->changePlayingAudio(playlistItem->filePath());
    updateCurrentAudioCover(playlistItem->filePath());
    audioPositionTimer.start();
    ui.playerPlayListWidget->setCurrentItem(currentAudio);
    ui.playerPauseButton->setState(1);
}

void MainWindowUI::seekForward () {
    effects->updateAudioDuration();
    effects->updateAudioPosition();

    int maxValue = ui.playerSeekSlider->maximum(),
        curValue = ui.playerSeekSlider->value();

    double ratio = effects->audioPosition+GST_SECOND*5 / effects->audioDuration;

    if (ratio < 1.0) {
        gint64 curPosition = effects->audioDuration*curValue*ratio/maxValue;
        effects->seekPlayingPosition(curPosition);
    }

}

void MainWindowUI::seekBackward () {

}

void MainWindowUI::updateAudioState () {
    effects->updateAudioDuration();
    effects->updateAudioPosition();
    
    if (!effects->isEOSReached()) {
        updateAudioPositionLabel(effects->audioPosition, effects->audioDuration);
        ui.playerSeekSlider->setValue(1000*effects->audioPosition/effects->audioDuration);
    }
    
    if (currentAudio != *playlistItems.end() && effects->isEOSReached() && currentAudio != stopAudio) {
        int currentAudioId = playlistItems.indexOf(currentAudio);
        int audioId = currentAudioId+1;
        while (audioId >= 0 && playlistItems.at(audioId)->isHidden())
            audioId++;

        if (audioId >= 0) {
            currentAudio   = playlistItems.at(audioId);
            currentAudioId = audioId;
        }
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
    }
}

void MainWindowUI::updatePanPosition () {
    static bool direction = true;
    int panPosition = ui.panoramaPositionDial->value();
    direction = (panPosition + 10 > ui.panoramaPositionDial->maximum() || 
                 panPosition - 10 < ui.panoramaPositionDial->minimum()) ? 
                 !direction : direction;
    ui.panoramaPositionDial->setValue(panPosition + (direction ? 10 : -10));
}

void MainWindowUI::infoActionClicked () {
    ui.playerPlayListWidget->hide();
    ui.playerPlaylistVisibilityButton->hide();
    ui.playerTagListFrame->show();
}

void MainWindowUI::setPlaylistStopAudio (QListWidgetItem *item) {
    stopAudio = item;
}

void MainWindowUI::removeFromPlaylist (QListWidgetItem *item) {
    if (currentAudio == item)
        nextButtonClicked();
    int index = playlistItems.indexOf(item);
    playlistItems.swapItemsAt(index, playlistItems.size()-1);
    playlistItems.removeLast();
    delete item;
}

void MainWindowUI::themeComboBoxClicked (const QString &theme) {
    QStyle *style = QStyleFactory::create(theme);
    QApplication::setStyle(style);
    // when changing the color theme, selects the appropriate icons
    setupIcons ();                                          
}

void MainWindowUI::extensionsMenuClicked () {
    // QObject *obj = sender();
    // if (!obj)
    //     return;

    // QAction *action = qobject_cast<QAction*>(obj);
    // QString actionText = QString("*.%1").arg(action->text());

    // // prevent all files from being shown
    // if (extensions.contains(actionText) && extensions.size() == 1)
    //     return;
    // else if (extensions.contains(actionText))
    //     extensions.removeOne(actionText);
    // else
    //     extensions << actionText;

    // QString labelText = QStringLiteral("Audio Extensions to Parse: %1");
    // ui.settingsExtensionsLabel->setText(labelText.arg(extensions.join(QChar(','))));

    // updatePlaylistWidget();
}

void MainWindowUI::newFolderSelected (const QStringList& folders) {
    parser.setMusicFolders(folders);
    updatePlaylistWidget();
}

void MainWindowUI::showSettingsTreeWidget () {
    ui.settingsDirectoryListView->setVisible(!ui.settingsDirectoryListView->isVisible());
}

void MainWindowUI::flatButtonsClicked (int state) {
    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    foreach (QPushButton *button, buttons)
        button->setFlat(state != Qt::Unchecked);
    parser.setConfigYaml<bool>("FlatButtons",state != Qt::Unchecked);
}

void MainWindowUI::showAudioCoverClicked (int state) {
    parser.setShowAudioCover(state != Qt::Unchecked);
    parser.setConfigYaml<bool>("ShowAudioCover",parser.getShowAudioCover());

    if (currentAudio == nullptr)
        return;

    if (!parser.getShowAudioCover())
        ui.playerMusicPicture->setText("*LOGO HERE*");
    else {
        int currentAudioId  = playlistItems.indexOf(currentAudio);
        auto item           = ui.playerPlayListWidget->item(currentAudioId);
        auto playlistWidget = qobject_cast<PlaylistWidgetItem*>
                                          (ui.playerPlayListWidget->itemWidget(item));
        updateCurrentAudioCover(playlistWidget->filePath());
    }
}

void MainWindowUI::playAtStartupClicked (int state) {
    parser.setPlayAtStartup(state != Qt::Unchecked);
    parser.setConfigYaml("PlayAtStartup",parser.getPlayAtStartup());
}

void MainWindowUI::saveLastAudioClicked (int state) {
    parser.setSaveLastAudio(state != Qt::Unchecked);
    if (!currentAudio)
        return;
    auto playlistWidget = qobject_cast<PlaylistWidgetItem*>(ui.playerPlayListWidget->itemWidget(currentAudio));
    parser.setConfigYaml("LastAudioFilePath", playlistWidget->filePath().c_str());
}

void MainWindowUI::showEvent (QShowEvent *event) {
    if (parser.getPlayAtStartup()) {
        auto widget = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(ui.playerPlayListWidget->currentItem()));
        updateCurrentAudioCover(widget->filePath());
    }
    QMainWindow::showEvent(event);
}

void MainWindowUI::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange) {
        ui.visualizingCustomPlot->setBackground(palette().color(QPalette::Window));
        setupIcons();
        gradient.setColorAt(1, qApp->palette().color(QPalette::Window));
        gradient.setColorAt(0, qApp->palette().color(QPalette::Highlight));
        bars->setBrush(QBrush(gradient));
    }
    QMainWindow::changeEvent(event);
}

void MainWindowUI::resizeEvent (QResizeEvent *event) {
    gradient.setStart (width()/2, height()/4);
    gradient.setFinalStop(width()/2, height());
    bars->setBrush(QBrush(gradient));

    if (!albumCover.isNull() && albumCoverSet) {
        int albumCoverSize = qMin(ui.playerHLine_1->width(), width()/2-10);
        QImage image = albumCover.scaled(QSize(albumCoverSize, albumCoverSize), Qt::KeepAspectRatio);
        QPixmap pixmap = QPixmap::fromImage(image);
        ui.playerMusicPicture->setPixmap(pixmap);
    }
    QMainWindow::resizeEvent(event);
}

void MainWindowUI::closeEvent (QCloseEvent *event) {
    presetDialogWindow->hide();

    if (currentAudio != nullptr) {
        auto widget = ui.playerPlayListWidget->itemWidget(currentAudio);
        auto playlistWidget = qobject_cast<PlaylistWidgetItem*>(widget);
        parser.saveConfigFile(playlistWidget->filePath());
    }
    QMainWindow::closeEvent(event);
}



