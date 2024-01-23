
#pragma once

//***********************************************************//
// STL
//***********************************************************//
#include <type_traits>
#include <algorithm>

//***********************************************************//
// Qt5
//***********************************************************//
#include <QDir>
#include <QIcon>
#include <QDebug>
#include <QColor>
#include <QPalette>
#include <QStyleFactory>
#include <QStandardPaths>
#include <QLinearGradient>

//***********************************************************//
// Third-party libs
//***********************************************************//
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <wavfile.h>
// #include <modfile.h> ??
#include <oggfile.h>
#include <mpegfile.h>
#include <opusfile.h>
#include <flacfile.h>
#include <aifffile.h>
#include <tpropertymap.h>
//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "player.h"

//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "effects.hpp"

#include "widgets/player/toggleButton.hpp"
#include "widgets/player/playlistWidgetItem.hpp"

#include "widgets/effects/presetDialogWindow.hpp"

#include "widgets/settings/directoryListView.hpp"


MainWindowUI::MainWindowUI () {
    ui.setupUi(this);
    
    setWindowTitle("Player");
    
    effects = std::make_unique<Effects>();
   
    parseConfigFile ();
    parsePresetFile ();
    createWidgets   ();
    setupWidgets    ();
    connectWidgets  ();
    
    audioVisualizingTime.start();
}

MainWindowUI::~MainWindowUI() {
    effects.reset();
    audioPositionTimer.stop();
    audioPanningTimer.stop();
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
}

void MainWindowUI::setupWidgets () {
    // 
    audioPositionTimer.setInterval(300);
    audioPanningTimer.setInterval(100);
    audioVisualizingTime.setInterval(100);
    
    ui.playerTagListFrame->hide();
    ui.playerPlayListWidget->hide();
        
    presetDialogWindow->setFixedSize(250, 100);
            
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
    gradient.setFinalStop(width()/2, height());
      
    gradient.setColorAt(1, qApp->palette().color(QPalette::Window));
    gradient.setColorAt(0, qApp->palette().color(QPalette::Highlight));
      
    brush = QSharedPointer<QBrush>::create(gradient);
    bars  = QSharedPointer<QCPBars>::create(ui.visualizingCustomPlot->xAxis, ui.visualizingCustomPlot->yAxis);
    
    bars->setPen(Qt::NoPen);
    bars->setWidth(1);
    bars->setBrush(*brush.data());
    
    ui.settingsDirectoryListView->hide();
}

void MainWindowUI::setupIcons () {
    QPalette palette = qApp->palette();
    QColor   backgroundColor = palette.color(QPalette::Window);
    
    QString iconPath = backgroundColor.value() < 128 ? ":icons/white/" : ":icons/black/";
    
    ui.playerPreviousButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playerRepeatButton->setStates({QIcon(iconPath+"repeat_all.svg")});
    ui.playerPauseButton->setStates({QIcon(iconPath+"play_music.svg"), 
                                     QIcon(iconPath+"pause_music.svg")});
    ui.playerShuffleButton->setIcon(QIcon(iconPath+"shuffle_playlist.svg"));
    ui.playerNextButton->setIcon(QIcon(iconPath+"next_track.svg"));
        
    ui.playerTagListVisibilityButton->setIcon(QIcon(iconPath+"previous_track.svg"));
    ui.playerPlaylistVisibilityButton->setIcon(QIcon(iconPath+"playlist_hide.svg"));
}

void MainWindowUI::connectWidgets () {
    connectEffectsTabWidgets();
    connectPlayerTabWidgets();
    connectVisualizingTabWidgets();
    connectSettingsTabWidgets();
    
    connect(&audioPositionTimer,   &QTimer::timeout, this, &MainWindowUI::updateAudioState);
    connect(&audioPanningTimer,    &QTimer::timeout, this, &MainWindowUI::updatePanPosition);
    connect(&audioVisualizingTime, &QTimer::timeout, this, &MainWindowUI::updateVisualizingWidget);
    
    connect(presetDialogWindow->dialogButtonBox, &QDialogButtonBox::accepted, this, &MainWindowUI::addNewPreset);
    
    connect(presetDialogWindow->dialogButtonBox, &QDialogButtonBox::rejected, [=]() {
        presetDialogWindow->dialogLineEdit->clear();
        presetDialogWindow->hide();
    });
}

void MainWindowUI::connectPlayerTabWidgets() {            
    connect(ui.playerPlayListWidget, &QListWidget::itemClicked, this, &MainWindowUI::playlistItemClicked);
    
    connect(ui.playerPlaylistVisibilityButton, &QPushButton::clicked, this, &MainWindowUI::togglePlaylistView);
    
    connect(ui.playerTagListVisibilityButton, &QPushButton::clicked, this, &MainWindowUI::closeTagListWidget);
    
    connect(ui.playerSeekSlider, &QSlider::sliderPressed, &audioPositionTimer, &QTimer::stop);
    
    connect(ui.playerSeekSlider, &QSlider::valueChanged, [this] (int value) {
        updateAudioPositionLabel(effects->audioDuration*value/1000, effects->audioDuration);
    });
    connect(ui.playerSeekSlider, &QSlider::sliderReleased, [this]() {
        effects->updateAudioDuration();
        int value = ui.playerSeekSlider->value(), 
            maxValue = ui.playerSeekSlider->maximum();
        gint64 currentPosition = effects->audioDuration * value / maxValue;
        effects->seekPlayingPosition(currentPosition);
        audioPositionTimer.start();
    });
    connect(ui.playerPreviousButton, &QPushButton::clicked, this, &MainWindowUI::previousButtonClicked);
    connect(ui.playerRepeatButton,   &QPushButton::clicked, this, &MainWindowUI::repeatButtonClicked);
    connect(ui.playerPauseButton,    &QPushButton::clicked, this, &MainWindowUI::pauseButtonClicked);
    connect(ui.playerShuffleButton,  &QPushButton::clicked, this, &MainWindowUI::shuffleButtonClicked);
    connect(ui.playerNextButton,     &QPushButton::clicked, this, &MainWindowUI::nextButtonClicked);
}

void MainWindowUI::connectEffectsTabWidgets () {
    connectEqualizerWidgets  ();
    connectDelayWidgets      ();
    connectFilterWidgets     ();
    connectPitchWidgets      ();
    connectCompressorWidgets ();
    connectPanoramaWidgets   ();
}

void MainWindowUI::connectEqualizerWidgets () {
    connect(ui.equalizerPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeEqualizerParams(ui.equalizerPresetsComboBox->itemData(id));
    });
    connect(ui.equalizerDefaultPresetButton, &QPushButton::clicked, [this]() {
        EqualizerPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        changeEqualizerParams(defaultData);
    });
    connect(ui.equalizerDeletePresetButton, &QPushButton::clicked, [this] () {
        this->removePreset("Equalizer", ui.equalizerPresetsComboBox);
    });
    connect(ui.equalizerSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Equalizer";
        presetDialogWindow->show();
    });
    
    using namespace CODES;
    connect(ui.equalizer29HzSlider,   &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider29Hz,  value);
    });
    connect(ui.equalizer59HzSlider,   &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider59Hz,  value);
    });
    connect(ui.equalizer119HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider119Hz, value);
    });
    connect(ui.equalizer237HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider237Hz, value);
    });
    connect(ui.equalizer474HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider474Hz, value);
    });
    connect(ui.equalizer1000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider1kHz,  value);
    });
    connect(ui.equalizer2000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider2kHz,  value);
    });
    connect(ui.equalizer4000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider4kHz,  value);
    });
    connect(ui.equalizer8000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider8kHz,  value);
    });
    connect(ui.equalizer16000HzSlider,&QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider16kHz, value);
    });
    connect(ui.equalizerVolumeSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Volume,      value);
    });
}

void MainWindowUI::connectDelayWidgets() {
    connect(ui.delayDefaultPresetButton, &QPushButton::clicked, [this]() {
        DelayPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        this->changeDelayParams(defaultData);
    });
    connect(ui.delayPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeDelayParams(ui.delayPresetsComboBox->itemData(id));
    });
    connect(ui.delaySavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Delay";
        presetDialogWindow->show();
    });
    connect(ui.delayDeletePresetButton, &QPushButton::clicked, [this]() {
        this->removePreset("Delay", ui.delayPresetsComboBox);
    });
    
    using namespace CODES;
    connect(ui.delayDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeDelayProps(CODES::Delay, (gdouble)value);
    });
    connect(ui.delaySurrounDelayButton, &QPushButton::clicked, [this]() {
        effects->changeDelayProps(CODES::SurroundDelay);
        if (ui.delaySurrounDelayButton->text() == "Off")
            ui.delaySurrounDelayButton->setText("On");
        else
            ui.delaySurrounDelayButton->setText("Off");
    });
    connect(ui.delayIntensityDial, &QDial::valueChanged, [this](int value) {
        effects->changeDelayProps(CODES::Intensity, value);
    });
    connect(ui.delayMaxDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeDelayProps(CODES::MaxDelay, value);
    });
    connect(ui.delayFeedbackDial, &QDial::valueChanged, [this](int value) {
        effects->changeDelayProps(CODES::Feedback, value);
    });
}

void MainWindowUI::connectFilterWidgets() {
    connect(ui.filterPresetSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Filter";
        presetDialogWindow->show();
    });
    connect(ui.filterDeletePresetButton, &QPushButton::clicked, [this]() {
        this->removePreset("Filter", ui.filterPresetsComboBox);
    });
    connect(ui.filterDefaultPresetButton, &QPushButton::clicked, [this]() {
        FilterPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        changeFilterParams(defaultData);
    });
    connect(ui.filterPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeFilterParams(ui.filterPresetsComboBox->itemData(id));
    });
    
    using namespace CODES;
    connect(ui.filterModeToggleButton, &QPushButton::clicked, [this]() {
        effects->changeFilterProps(FILTER::Mode);
        if (ui.filterModeToggleButton->text() == "Low Pass")
            ui.filterModeToggleButton->setText("High Pass");
        else
            ui.filterModeToggleButton->setText("Low Pass");
    });
    connect(ui.filterCutoffSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeFilterProps(FILTER::Cutoff, (gfloat)value);
    });
    connect(ui.filterRippleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        effects->changeFilterProps(FILTER::Ripple, (gfloat)value);
    });
    connect(ui.filterPolesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeFilterProps(FILTER::Poles, (gfloat)value);
    });
    connect(ui.filterFilterTypeButton, &QPushButton::clicked, [this]() {
        effects->changeFilterProps(FILTER::FilterType);
        if (ui.filterFilterTypeButton->text() == "I")
            ui.filterFilterTypeButton->setText("II");
        else
            ui.filterFilterTypeButton->setText("I");
    });
}

void MainWindowUI::connectPitchWidgets () {
    connect(ui.pitchSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Pitch";
        presetDialogWindow->show();
    });
    connect(ui.pitchDeletePresetButton, &QPushButton::clicked, [this]() {
        removePreset("Pitch", ui.pitchPresetComboBox);
    });
    connect(ui.pitchDefaultPresetButton, &QPushButton::clicked, [this]() {
        PitchPreset defaultPreset;
        QVariant data = QVariant::fromValue(defaultPreset);
        this->changePitchParams(data);
    });
    connect(ui.pitchPresetComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changePitchParams(ui.pitchPresetComboBox->itemData(id));
    });
    
    using namespace CODES;
    connect(ui.pitchTempoDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Tempo, value/10.0);
    });
    connect(ui.pitchPitchDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Pitch, value/10.0);
    });
    connect(ui.pitchRateDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Rate, value/10.0);
    });
    connect(ui.pitchOutputRateDial,&QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::OutputRate, value/10.0);
    });
}

void MainWindowUI::connectCompressorWidgets () {
    connect(ui.compressorSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Compressor";
        presetDialogWindow->show();
    });
    connect(ui.compressorDeletePresetButton, &QPushButton::clicked, [this]() {
        removePreset("Compressor", ui.compressorPresetComboBox);
    });
    connect(ui.compressorDefaultPresetButton, &QPushButton::clicked, [this]() {
        CompressorPreset defaultPreset;
        QVariant data = QVariant::fromValue(defaultPreset);
        this->changeCompressorParams(data);
    });
    connect(ui.compressorPresetComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeCompressorParams(ui.compressorPresetComboBox->itemData(id));
    });
    
    using namespace CODES;
    connect(ui.compressorExpanderToggleButton, &QPushButton::clicked, [this]() {
        effects->changeCompressorProps(COMPRESSOR::Compressor);
        if (ui.compressorExpanderToggleButton->text() == "Compressor")
            ui.compressorExpanderToggleButton->setText("Expander");
        else
            ui.compressorExpanderToggleButton->setText("Compressor");
    });
    connect(ui.compressorKneeToggleButton, &QPushButton::clicked, [this]() {
        effects->changeCompressorProps(COMPRESSOR::Knee);
        if (ui.compressorKneeToggleButton->text() == "Hard-knee")
            ui.compressorKneeToggleButton->setText("Soft-knee");
        else
            ui.compressorKneeToggleButton->setText("Hard-knee");
    });
    connect(ui.compressorRatioDial, &QDial::valueChanged, [this](int value) {
        effects->changeCompressorProps(COMPRESSOR::Ratio, value);
    });
    connect(ui.compressorThresholdDial, &QDial::valueChanged, [this](int value) {
        effects->changeCompressorProps(COMPRESSOR::Threshold, value);
    });
}

void MainWindowUI::connectPanoramaWidgets () {
    using namespace CODES;
    
    connect(ui.panoramaPanningMethodButton, &QPushButton::clicked, [this]() {
        effects->changePanoramaProps(PANORAMA::Method);
        if (ui.panoramaPanningMethodButton->text() == "Simple")
            ui.panoramaPanningMethodButton->setText("Psychoacoustic");
        else
            ui.panoramaPanningMethodButton->setText("Simple");
    });
    connect(ui.panoramaPositionDial, &QDial::valueChanged, [this](int value) {
        effects->changePanoramaProps(PANORAMA::Position, value);
    });
    connect(ui.panoramaAutoPanoramaButton, &QPushButton::clicked, [this]() {
        if (audioPanningTimer.isActive()) {
            ui.panoramaAutoPanoramaButton->setText("Off");
            audioPanningTimer.stop();
        }
        else {
            ui.panoramaAutoPanoramaButton->setText("On");
            audioPanningTimer.start();
        }
        ui.panoramaSpeedDial->setEnabled(!ui.panoramaSpeedDial->isEnabled());
    });
    connect(ui.panoramaSpeedDial, &QDial::valueChanged, [this](int value) {
        audioPanningTimer.stop();
        audioPanningTimer.setInterval(ui.panoramaSpeedDial->maximum()+10-value);
        audioPanningTimer.start();
    });
}

void MainWindowUI::connectVisualizingTabWidgets () {
    connect(ui.visualizingBandsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindowUI::bandsChanged);
    connect(ui.visualizingRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindowUI::updateRateChanged);
    connect(ui.visualizingThresholdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindowUI::thresholdChanged);
}

void MainWindowUI::connectSettingsTabWidgets () {
    connect(ui.settingsColorThemeComboBox, &QComboBox::currentTextChanged,          this, &MainWindowUI::themeComboBoxClicked);
    connect(ui.settingsMediaFoldersButton, &QPushButton::clicked,                   this, &MainWindowUI::showSettingsTreeWidget);
    connect(ui.settingsDirectoryListView, &DirectoryListView::selectedFolderUpdated, this, &MainWindowUI::newFolderSelected);
}

void MainWindowUI::createConfigFile () {
    std::string configData = R"~(---
PresetFilePath: null
MusicFolders: []
...
)~";
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
    configPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/gstreamer_player/config.yaml";
    
    QFile configFile (configPath);
    
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
        qWarning() << "[Warning]:" << "Unknown error. Creating config file";
        createConfigFile ();
    }
    
    extractConfigInfo<QString>    ("PresetFilePath");
    extractConfigInfo<QStringList>("MusicFolders");
}

template <class T>
void MainWindowUI::extractConfigInfo (std::string key) {
    if (!configYaml[key].IsDefined()) {
        qWarning() << "[Warning]:" << key.c_str() << "doesn't exist. Skipping";
        return;
    }
    const YAML::Node &element = configYaml[key];
    try {
        if      constexpr (std::is_same<T, QString>::value)
            presetPath = element.as<T>();
        else if constexpr (std::is_same<T, QStringList>::value)
            musicFolders = element.as<T>();
    } catch (YAML::BadConversion &e) {
        qWarning() << "[Warning]:" << e.what();
    } catch (...) {
        qWarning() << "[Warning]:" << "Unknown error. Skipping";
    }
}

void MainWindowUI::createPresetFile () {
    std::string presetData = R"~(---
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
Delay:
  null
Filter:
  null
Pitch:
  null
Compressor:
  null
...
)~";    
    presetYaml = YAML::Load(presetData.c_str());
    
    QFile configFile (presetPath);
    if (!configFile.open (QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning () << "[Warning]:" << "Unable to open preset file. Skipping";
        return;
    }
    if (configFile.write (presetData.c_str(), presetData.size()) == -1)
        qWarning() << "[Warning]:" << "Writing presets to a file failed. Skipping";
    configFile.close ();
}

void MainWindowUI::parsePresetFile() {
    QFile presetFile (presetPath);
    
    if (!presetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "parsePresetFile" <<  presetFile.errorString();
        createPresetFile ();
    }
    if (presetFile.size() == 0) {
        qWarning() << "[Warning]:" << "Empty config file. Creating new one";
        createPresetFile ();
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

    parsePresets<EqualizerPreset>  ("Equalizer");
    parsePresets<DelayPreset>      ("Delay");
    parsePresets<FilterPreset>     ("Filter");
    parsePresets<PitchPreset>      ("Pitch");
    parsePresets<CompressorPreset> ("Compressor");
}

template <class T>
void MainWindowUI::parsePresets(std::string presetKeyName) {
    QVariant data;
    std::string presetName;
    QComboBox *combobox;
    if (!presetYaml[presetKeyName].IsDefined()) {
        qDebug() << presetKeyName.c_str() << "does not exit. Skipping";
        return;
    }
    const YAML::Node &element = presetYaml[presetKeyName];
    for (YAML::const_iterator it = element.begin(); it != element.end(); ++it) {
        presetName = it->first.as<std::string>();
        try {
            // Effects presets
            if constexpr (std::is_same<T, EqualizerPreset>::value) {
                data = QVariant::fromValue(it->second.as<EqualizerPreset>());
                combobox = ui.equalizerPresetsComboBox;
            // Delay presets
          } else if constexpr (std::is_same<T, DelayPreset>::value) {
                data = QVariant::fromValue(it->second.as<DelayPreset>());
                combobox = ui.delayPresetsComboBox;
            // Filter presets
          } else if constexpr (std::is_same<T, FilterPreset>::value) {
                data = QVariant::fromValue(it->second.as<FilterPreset>());
                combobox = ui.filterPresetsComboBox;
            // Compressor presets
          } else if constexpr (std::is_same<T, PitchPreset>::value) {
                data = QVariant::fromValue(it->second.as<PitchPreset>());
                combobox = ui.pitchPresetComboBox;
          } else if constexpr (std::is_same<T, CompressorPreset>::value) {
                data = QVariant::fromValue(it->second.as<CompressorPreset>());
                combobox = ui.compressorPresetComboBox;
          }
        } catch (YAML::BadConversion &e) {
            qWarning() << "[Warning]:" << e.what();
            continue;
        } catch (YAML::BadSubscript &e) {
            qWarning() << "[Warning]:" << presetKeyName.c_str() << presetName.c_str() << e.what();
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
    
    if      constexpr (std::is_same<T, EqualizerPreset>::value)
        serializeEqualizerParams(&newPresetSeq);
    else if constexpr (std::is_same<T, DelayPreset>::value)
        serializeDelayParams(&newPresetSeq);
    else if constexpr (std::is_same<T, FilterPreset>::value)
        serializeFilterParams(&newPresetSeq);
    else if constexpr (std::is_same<T, PitchPreset>::value)
        serializePitchParams(&newPresetSeq);
    else if constexpr (std::is_same<T, CompressorPreset>::value)
        serializeCompressorParams(&newPresetSeq);

    presetYaml[currentPresetType][presetName] = std::move(newPresetSeq);
    parsePresets<T>(currentPresetType);
    savePresets();
}

void MainWindowUI::parseMusic (QFileInfoList& musicFiles) {
    QStringList nameFilters;
    nameFilters << "*.mp3" << ".flac" << "*.opus" << "*.wav";
    
    ui.playerPlayListWidget->clear();
    
    foreach (const QString &folder, musicFolders) {
            QDir musicDir (folder);
            musicDir.setNameFilters(nameFilters);
            musicFiles += musicDir.entryInfoList(QDir::Files);
    }
    if (musicFolders.size() == 0) {
        QDir musicDir (QStandardPaths::standardLocations(QStandardPaths::MusicLocation).at(0));
        musicDir.setNameFilters(nameFilters);
        musicFiles += musicDir.entryInfoList(QDir::Files);
    }
}

void MainWindowUI::updatePlaylistWidget () {
    //  Add playlist items
    QFileInfoList musicFiles;
    parseMusic(musicFiles);
    
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
    node->push_back(ui.delaySurrounDelayButton->text() != "Off");
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
    node->push_back(ui.compressorExpanderToggleButton->text() != "Compressor");
    node->push_back(ui.compressorKneeToggleButton->text() != "Hard-knee");
    node->push_back(ui.compressorRatioDial->value());
    node->push_back(ui.compressorThresholdDial->value());
}

void MainWindowUI::changeEqualizerParams  (QVariant data) {
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

void MainWindowUI::changeDelayParams      (QVariant data) {
    DelayPreset params = data.value<DelayPreset>();
    ui.delayDelaySpinBox->setValue(params.delay);
    if ((ui.delaySurrounDelayButton->text() == "On") != params.surroundDelay)
        ui.delaySurrounDelayButton->click();
    ui.delayIntensityDial->setValue(params.intensity);
    ui.delayMaxDelaySpinBox->setValue(params.maxDelay);
    ui.delayFeedbackDial->setValue(params.feedback);
}

void MainWindowUI::changeFilterParams     (QVariant data) {
    FilterPreset params = data.value<FilterPreset>();
    if ((ui.filterModeToggleButton->text() == "Low Pass") == params.mode)
        ui.filterModeToggleButton->click();
    ui.filterCutoffSpinBox->setValue(params.cutoff);
    ui.filterRippleSpinBox->setValue(params.ripple);
    ui.filterPolesSpinBox->setValue(params.poles);
    if ((ui.filterFilterTypeButton->text() == "I") == params.filterType)
        ui.filterFilterTypeButton->click();
}

void MainWindowUI::changePitchParams      (QVariant data) {
    PitchPreset params = data.value<PitchPreset>();
    ui.pitchTempoDial->setValue(params.tempo);
    ui.pitchPitchDial->setValue(params.pitch);
    ui.pitchRateDial->setValue(params.rate);
    ui.pitchOutputRateDial->setValue(params.outputRate);
}

void MainWindowUI::changeCompressorParams (QVariant data) {
    CompressorPreset params = data.value<CompressorPreset>();
    if ((ui.compressorExpanderToggleButton->text() == "Compressor") == params.compressor)
        ui.compressorExpanderToggleButton->click();
    if ((ui.compressorKneeToggleButton->text() == "Hard-knee") == params.kneeType)
        ui.compressorKneeToggleButton->click();
    ui.compressorRatioDial->setValue(params.ratio);
    ui.compressorThresholdDial->setValue(params.threshold);
}

//********Others********//
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
    string = string.arg(minutes_pos, 2, 10, QChar('0')), 
    string = string.arg(seconds_pos % 60, 2, 10, QChar('0')), 
    string = string.arg(hours_dur).arg(minutes_dur, 2, 10, QChar('0')), 
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
    
    audioVisualizingTime.setInterval(newValue);
}

void MainWindowUI::thresholdChanged (int newValue) {
    effects->changeSpectrumProps(CODES::AudioThreshold, newValue);
//     ui.visualizingCustomPlot->yAxis->setRange(0, 60);
//     ui.visualizingCustomPlot->rescaleAxes();
}

//     ui.visualizingCustomPlot->clearPlottables();

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
    if      (this->currentPresetType == "Equalizer")
        this->updatePresetConfig<EqualizerPreset> (presetDialogWindow->getLineInput());
    else if (this->currentPresetType == "Delay")
        this->updatePresetConfig<DelayPreset>     (presetDialogWindow->getLineInput());
    else if (this->currentPresetType == "Filter")
        this->updatePresetConfig<FilterPreset>    (presetDialogWindow->getLineInput());
    else if (this->currentPresetType == "Pitch")
        this->updatePresetConfig<PitchPreset>     (presetDialogWindow->getLineInput());
    else if (this->currentPresetType == "Compressor")
        this->updatePresetConfig<CompressorPreset>(presetDialogWindow->getLineInput());
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
        
        ui.playerTagListSizeLabel      ->setText(sizeStr      .arg(file.file()->length() / 1048576.0, 0, 'f', 2));
        ui.playerTagListChannelsLabel  ->setText(channelStr   .arg(audioProperties->channels()));
        ui.playerTagListSampleRateLabel->setText(sampleRateStr.arg(audioProperties->sampleRate()));
        ui.playerTagListBitRateLabel   ->setText(bitRateStr   .arg(audioProperties->bitrate()));
    }
    ui.playerTagListFrame->show();
    
    ui.playerPlayListWidget->hide();
    ui.playerPlaylistVisibilityButton->hide();
}

void MainWindowUI::closeTagListWidget () {
    ui.playerTagListFrame->hide();
    
    ui.playerPlayListWidget->show();
    ui.playerPlaylistVisibilityButton->show();
}

void MainWindowUI::togglePlaylistView () {
    ui.playerPlayListWidget->setVisible(!ui.playerPlayListWidget->isVisible());
}

void MainWindowUI::previousButtonClicked () {
    int currentAudioId = playlistItems.indexOf(currentAudio);
    if (currentAudioId > 0) {
        currentAudio = playlistItems.at(currentAudioId-1);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        audioPositionTimer.start();
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
    }
}

void MainWindowUI::repeatButtonClicked () {
    qDebug() << "repeat button clicked";
}

void MainWindowUI::pauseButtonClicked () {
    if (playlistItems.size() == 0)
        return;
        
    if (currentAudio == nullptr) {
        currentAudio = playlistItems.at(0);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
        audioPositionTimer.start();
        return;
    }
    effects->togglePipelineState();
    if (audioPositionTimer.isActive())
        audioPositionTimer.stop();
    else
        audioPositionTimer.start();
}

void MainWindowUI::shuffleButtonClicked () {
    std::random_shuffle(playlistItems.begin(), playlistItems.end());
}

void MainWindowUI::nextButtonClicked () {
    int currentAudioId = playlistItems.indexOf(currentAudio);
    if (currentAudioId < playlistItems.size()-1 && currentAudio != stopAudio) {
        currentAudio = playlistItems.at(currentAudioId+1);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        audioPositionTimer.start();
        ui.playerPlayListWidget->setCurrentItem(currentAudio);
    }
}

void MainWindowUI::playlistItemClicked (QListWidgetItem *item) {
    currentAudio   = item;
    PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
        (ui.playerPlayListWidget->itemWidget(item));
    effects->changePlayingAudio(playlistItem->filePath());
    audioPositionTimer.start();
    ui.playerPlayListWidget->setCurrentItem(currentAudio);
    ui.playerPauseButton->setStateId(1);
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
    }
    QMainWindow::changeEvent(event);
}

void MainWindowUI::resizeEvent (QResizeEvent *event) {
//     gradient.setStart (width()/2, height()/2);
//     gradient.setFinalStop(width()/2, height());
//     
//     brush.release();
//     brush = QSharedPointer<QBrush>::create(gradient);
    
    QMainWindow::resizeEvent(event);
}

void MainWindowUI::closeEvent (QCloseEvent *event) {
    presetDialogWindow->hide();
    QMainWindow::closeEvent(event);
}



