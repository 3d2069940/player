
#pragma once

//***********************************************************//
// STL
//***********************************************************//
#include <random>
#include <algorithm>
#include <type_traits>
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
#include <QRandomGenerator>
#include <QPropertyAnimation>
//***********************************************************//
// Third-party libs
//***********************************************************//
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <wavfile.h>
// #include <modfile.h> ??
#include <oggfile.h>
#include <opusfile.h>
#include <mpegfile.h>
#include <flacfile.h>
#include <aifffile.h>
#include <oggflacfile.h>
#include <tpropertymap.h>
#include <attachedpictureframe.h>

#include <sndfile.h>
//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "player.h"

//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "effects.hpp"
#include "connector.cpp"
#include "toggleButton.hpp"


#include "playlistWidgetItem.hpp"
#include "widgets/effects/presetDialogWindow.hpp"
#include "widgets/settings/directoryListView.hpp"

#include "widgets/delegation/playlistDelegate.cpp"

MainWindowUI::MainWindowUI () {
    ui.setupUi(this);
    
    setWindowTitle("Player");
    
    effects = std::make_unique<Effects>();
   
    parseConfigFile ();
    parsePresetFile ();
    createWidgets   ();
    setupWidgets    ();

    Connector::connectWidgets(this);
    
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
}

void MainWindowUI::setupWidgets () {
    // 
    audioPositionTimer.setInterval(300);
    audioPanningTimer.setInterval(100);
    audioVisualizingTimer.setInterval(100);
    
    ui.playerTagListFrame->hide();
    ui.playerPlayListWidget->hide();
    ui.playerSearchLineEdit->hide();

    ui.playerPlayListWidget->setItemDelegate(new PlaylistDelegate());

    presetDialogWindow->setFixedSize(250, 100);

    ui.delaySurroundDelayButton->setLabels({"Off", "On"});
    ui.compressorExpanderToggleButton->setLabels({"Compressor", "Expander"});
    ui.compressorKneeToggleButton->setLabels({"Hard-knee", "Soft-knee"});
            
    setupIcons ();
    //  Add installed themes to settings combobox
    QStringList styleNames = QStyleFactory::keys();
    ui.settingsColorThemeComboBox->addItems(styleNames);
    //  Set current theme
    QStyle *style = QApplication::style();
    ui.settingsColorThemeComboBox->setCurrentText(style->objectName());
    
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

    QStringList extActions = {"mp3", "wav", "flac", "aac", "ogg", "opus", "wma", "aiff", "ac3", "amr"};
    foreach (const QString &ext, extActions) {
        extMenu->addAction(ext);
    }

    extMenu->setMinimumWidth(300);
    ui.settingsExtensionsButton->setMenu(extMenu.data());

    ui.settingsDirectoryListView->markSelectedDirs(musicFolders);
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
        
    ui.playerTagListVisibilityButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playerPlaylistVisibilityButton->setIcon(QIcon(iconPath+"playlist_hide.svg"));
}

void MainWindowUI::setupAnimations () {

}

void MainWindowUI::createConfigFile () {
    std::string configData = R"~(---
PresetFilePath: ""
Extensions: ["*.mp3","*.flac","*.opus","*.wav"]
ShowAudioCover: false
FlatButtons: true
PlayAtStartup: false
SaveLastAudio: false
MusicFolders: []
...)~";
    configYaml = YAML::Load(configData.c_str());
    
    QFile configFile (configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[Warning]:" << "Opening a file to write the configuration failed. Skipping";
        return;
    }
    if (configFile.write(configData.c_str(), configData.size()) == -1)
        qWarning() << "[Warning]:" << "Writing to the configuration file failed. Skipping";
    configFile.close();
}

void MainWindowUI::parseConfigFile () {
    configPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/player/config.yaml";
    
    QFile configFile (configPath);
    QFileInfo configFileInfo (configFile);

    configFileInfo.absoluteDir().mkpath(".");
    
    if (!configFile.exists())
        createConfigFile ();

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Warning]:" << configFile.errorString();
        createConfigFile ();
  } else if (configFile.size() == 0) {
        qWarning() << "[Warning]:" << "Config file is empty. Creating new one";
        createConfigFile ();
    }
    QString configData = configFile.readAll();
    configFile.close();
    
    try {
        configYaml = YAML::Load(configData.toStdString());
    } catch (YAML::ParserException &e) {
        qWarning() << "[Warning]:" << "Bad config file data. Creating new one";
        createConfigFile ();
    } catch (...) {
        qWarning() << "[Unknown Error]:" << "Creating config file";
        createConfigFile ();
    }
    extractConfigInfo<QStringList>(&extensions,     "Extensions");
    extractConfigInfo<QString>    (&presetPath,     "PresetFilePath");
    if (presetPath == "") {
        presetPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/player/presets.yaml";

    }
    extractConfigInfo<QStringList>(&musicFolders,   "MusicFolders");
    extractConfigInfo<bool>       (&flatButtons,    "FlatButtons");
    extractConfigInfo<bool>       (&showAudioCover, "ShowAudioCover");
    extractConfigInfo<bool>       (&playAtStartup,  "PlayAtStartup");
    extractConfigInfo<bool>       (&saveLastAudio,  "SaveLastAudio");
}

template <class T>
void MainWindowUI::extractConfigInfo (T* var, std::string key) {
    if (!configYaml[key].IsDefined()) {
        qWarning() << "[Warning]:" << key.c_str() << "doesn't exist. Skipping";
        return;
    }
    const YAML::Node &element = configYaml[key];
    try {
        *var = element.as<T>();
    } catch (YAML::BadConversion &e) {
        qWarning() << "[Warning]:" << e.what();
    } catch (...) {
        qWarning() << "[Warning]:" << "Unknown error. Skipping";
    }
}

void MainWindowUI::createPresetFile () {
    loadDefaultPresets();

    QFile presetFile (presetPath);

    if (!presetFile.open (QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning () << "[Warning]:" << "Unable to open preset file. Skipping";
        return;
    }

    YAML::Emitter emitter;

    emitter << presetYaml;

    if (presetFile.write (emitter.c_str()) == -1)
        qWarning() << "[Warning]:" << "Writing presets to a file failed. Skipping";

    presetFile.close ();
}

void MainWindowUI::loadDefaultPresets () {
    std::string presetData =
        R"~(---
Equalizer:
    Default: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    Bass: [30, 30, 20, 10, 0, 0, 0, 0, 0, 0]
    Treble: [0, 0, 0, 0, 0, 10, 20, 30, 30, 30]
    Midrange: [0, 10, 20, 30, 30, 30, 20, 10, 0, 0]
    Soft: [-20, -10, -10, 0, 10, 10, 0, -10, -20, -20]
    Bright: [0, 0, 10, 20, 30, 30, 20, 10, 0, 0]
    Full Bass: [40, 30, 20, 10, 0, -10, -20, -30, -40, -40]
    High-pitched: [0, -10, -20, -20, -10, 10, 20, 20, 10, 0]
    Bass boost: [10, 20, 20, 10, -10, -20, -20, -10, 10, 20]
    All boost: [20, 20, 20, 20, 20, 20, 20, 20, 20, 20]
Delay: ~
Filter: ~
Pitch: ~
Compressor: ~
...)~";
    presetYaml = YAML::Load(presetData.c_str());
}

void MainWindowUI::parsePresetFile() {
    QFile presetFile (presetPath);

    if (!presetFile.exists())
        createPresetFile ();

    if (!presetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        loadDefaultPresets();
        qWarning() << "parsePresetFile" <<  presetFile.errorString();
    }

    if (presetFile.size() == 0) {
        qWarning() << "[Warning]:" << "Empty config file. Creating new one";
        createPresetFile();
    }

    QString presetData = presetFile.readAll();
    presetFile.close();
        
    try {
        presetYaml = YAML::Load(presetData.toStdString());
    } catch (YAML::ParserException &e) {
        qWarning() << "[Warning]:" << "Bad config file data. Creating new one";
        createPresetFile ();
    } catch (...) {
        qWarning() << "[Warning]:" << "Unknown error. Creating config file";
        createPresetFile ();
    }

    parsePresets<EqualizerPreset>  (ui.equalizerPresetsComboBox, "Equalizer");
    parsePresets<DelayPreset>      (ui.delayPresetsComboBox,     "Delay");
    parsePresets<FilterPreset>     (ui.filterPresetsComboBox,    "Filter");
    parsePresets<PitchPreset>      (ui.pitchPresetComboBox,      "Pitch");
    parsePresets<CompressorPreset> (ui.compressorPresetComboBox, "Compressor");
}

template <class T>
void MainWindowUI::parsePresets(QComboBox *combobox, const std::string &key) {
    QVariant data;
    std::string presetName;
    if (!presetYaml[key].IsDefined()) {
        qWarning() << key.c_str() << "does not exit. Skipping";
        return;
    }
    const YAML::Node &element = presetYaml[key];
    for (YAML::const_iterator it = element.begin(); it != element.end(); ++it) {
        presetName = it->first.as<std::string>();
        try {
            // Effects presets
            if constexpr (std::is_same<T, EqualizerPreset>::value)
                data = QVariant::fromValue(it->second.as<EqualizerPreset>());
            // Delay presets
            else if constexpr (std::is_same<T, DelayPreset>::value)
                data = QVariant::fromValue(it->second.as<DelayPreset>());
            // Filter presets
            else if constexpr (std::is_same<T, FilterPreset>::value)
                data = QVariant::fromValue(it->second.as<FilterPreset>());
            // Pitch presets
            else if constexpr (std::is_same<T, PitchPreset>::value)
                data = QVariant::fromValue(it->second.as<PitchPreset>());
            // Compressor presets
            else if constexpr (std::is_same<T, CompressorPreset>::value)
                data = QVariant::fromValue(it->second.as<CompressorPreset>());
        } catch (YAML::BadConversion &e) {
            qWarning() << "[Warning]:" << e.what();
            continue;
        } catch (...) {
            qWarning() << "[Unknown Error with]:" << key.c_str() << presetName.c_str();
            continue;
        }
        combobox->addItem(QString::fromStdString(presetName), data);
    }
}

void MainWindowUI::savePresets () {
    QFile presetFile (presetPath);

    if (!presetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << presetFile.errorString() << '\n';
        return;
    }

    YAML::Emitter out;
    out << presetYaml;
    if (presetFile.write(out.c_str(), out.size()) == -1) {
        qWarning() << "[Warning]:" << "Saving preset to a file failed. Skipping";
    }
    presetFile.close();
}

void MainWindowUI::removePreset (std::string presetType, QComboBox* combobox) {
    int         currentItemId   = combobox->currentIndex();
    std::string currentItemText = combobox->currentText().toStdString();
    combobox->removeItem(currentItemId);
    
    if (presetYaml[presetType].IsDefined()) 
        presetYaml[presetType].remove(currentItemText);
    savePresets();
}

template <class T>
void MainWindowUI::updatePresetConfig (std::string presetName) {
    YAML::Node newPresetSeq (YAML::NodeType::Sequence);
    newPresetSeq.SetStyle(YAML::EmitterStyle::Flow);
    QComboBox *combobox;
    if      constexpr (std::is_same<T, EqualizerPreset>::value) {
        serializeEqualizerParams(&newPresetSeq);
        combobox = ui.equalizerPresetsComboBox;
  } else if constexpr (std::is_same<T, DelayPreset>::value) {
        serializeDelayParams(&newPresetSeq);
        combobox = ui.delayPresetsComboBox;
  } else if constexpr (std::is_same<T, FilterPreset>::value) {
        serializeFilterParams(&newPresetSeq);
        combobox = ui.filterPresetsComboBox;
  } else if constexpr (std::is_same<T, PitchPreset>::value) {
        serializePitchParams(&newPresetSeq);
        combobox = ui.pitchPresetComboBox;
  } else if constexpr (std::is_same<T, CompressorPreset>::value) {
        serializeCompressorParams(&newPresetSeq);
        combobox = ui.compressorPresetComboBox;
  }

    presetYaml[currentPresetType][presetName] = std::move(newPresetSeq);
    parsePresets<T>(combobox, currentPresetType);
    savePresets();
}

void MainWindowUI::parseMusic (const QString &path, QFileInfoList& musicFiles) {

    QDir musicDir (path);
    musicDir.setNameFilters(extensions);
    musicFiles += musicDir.entryInfoList(QDir::Files);

    QDir folderDir (path);
    QFileInfoList folderList;
    folderDir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    folderList = folderDir.entryInfoList();
    foreach (const QFileInfo &folder, folderList)
        parseMusic (folder.absoluteFilePath(), musicFiles);
}

void MainWindowUI::updatePlaylistWidget () {
    //  Add playlist items
    ui.playerPlayListWidget->clear();
    QFileInfoList musicFiles;
    if (musicFolders.size() == 0)
        musicFolders << QStandardPaths::standardLocations(QStandardPaths::MusicLocation).at(0);

    foreach (const QString &folder, musicFolders)
        parseMusic(folder, musicFiles);
    
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
        albumCover.loadFromData(imageData, "JPEG");
        // albumCover = albumCover.convertToFormat(QImage::Format_ARGB32);
        // for (int y = 0; y < albumCover.height(); ++y) {
        //     for (int x = 0; x < albumCover.width(); ++x) {
        //         QRgb pixel = albumCover.pixel(x, y);
        //         int alpha = qAlpha(pixel);
        //         albumCover.setPixel(x, y, qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), alpha*0.5));
        //     }
        // }
        QImage image = albumCover.scaled(QSize(ui.playerHLine_1->width(), ui.playerHLine_1->width()), Qt::KeepAspectRatio);
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

    if      (currentPresetType == "Equalizer")
        updatePresetConfig<EqualizerPreset> (presetName);
    else if (currentPresetType == "Delay")
        updatePresetConfig<DelayPreset>     (presetName);
    else if (currentPresetType == "Filter")
        updatePresetConfig<FilterPreset>    (presetName);
    else if (currentPresetType == "Pitch")
        updatePresetConfig<PitchPreset>     (presetName);
    else if (currentPresetType == "Compressor")
        updatePresetConfig<CompressorPreset>(presetName);

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
    
    ui.playerPlayListWidget->hide();
    ui.playerPlaylistVisibilityButton->hide();
    ui.playerSearchLineEdit->hide();
    ui.showPlaylistsButton->hide();
}

void MainWindowUI::closeTagListWidget () {
    ui.playerTagListFrame->hide();
    
    ui.playerSearchLineEdit->show();
    ui.showPlaylistsButton->show();
    ui.playerPlayListWidget->show();
    ui.playerPlaylistVisibilityButton->show();
}

void MainWindowUI::togglePlaylistView () {
    ui.playerSearchLineEdit->setVisible(!ui.playerSearchLineEdit->isVisible());
    ui.playerPlayListWidget->setVisible(!ui.playerPlayListWidget->isVisible());
}

void MainWindowUI::previousButtonClicked () {
    int currentAudioId = playlistItems.indexOf(currentAudio);
    if (currentAudioId > 0) {
        currentAudio = playlistItems.at(currentAudioId-1);
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
        currentAudio = playlistItems.at(currentAudioId+1);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        updateCurrentAudioCover(playlistItem->filePath());
        audioPositionTimer.start();
        ui.playerPauseButton->setState(1);
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
    }
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

    if (ratio >= 1.0)
        nextButtonClicked();
    else {
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
        currentAudio = playlistItems.at(currentAudioId+1);
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
    QObject *obj = sender();
    if (!obj)
        return;

    QAction *action = qobject_cast<QAction*>(obj);
    QString actionText = QString("*.%1").arg(action->text());

    qDebug() << extensions;

    if (extensions.contains(actionText))
        extensions.removeOne(actionText);
    else
        extensions << actionText;

    qDebug() << extensions;
    updatePlaylistWidget();
}

void MainWindowUI::newFolderSelected (const QStringList& folders) {
    musicFolders = folders;
    updatePlaylistWidget();
}

void MainWindowUI::showSettingsTreeWidget () {
    ui.settingsDirectoryListView->setVisible(!ui.settingsDirectoryListView->isVisible());
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
        int albumCoverWidth = qMin(ui.playerHLine_1->width(), width()/2-10);
        QImage image = albumCover.scaled(QSize(albumCoverWidth, albumCoverWidth), Qt::KeepAspectRatio);
        QPixmap pixmap = QPixmap::fromImage(image);
        ui.playerMusicPicture->setPixmap(pixmap);
    }
    QMainWindow::resizeEvent(event);
}

void MainWindowUI::closeEvent (QCloseEvent *event) {
    presetDialogWindow->hide();
    QMainWindow::closeEvent(event);
}



