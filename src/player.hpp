
#pragma once

//***********************************************************//
// STL
//***********************************************************//
#include <type_traits>

//***********************************************************//
// Qt5
//***********************************************************//
#include <QDir>
#include <QDebug>

// #include <QPalette>
#include <QStandardPaths>

//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "player.h"
#include "presetTypes.h"

//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "effects.hpp"
#include "presetDialogWindow.hpp"
#include "playlistWidgetItem.hpp"



MainWindowUI::MainWindowUI () {
    ui.setupUi(&mainwindow_);
 
    ui.playerPlayListWidget->hide();
    
    effects      = std::make_unique<Effects>();
    dialogWindow = std::make_unique<PresetDialogWindow>();
   
    parseConfigFile ();

    setupWidgets    ();
    connectWidgets  ();
}

MainWindowUI::~MainWindowUI() {
    effects.reset();
}

QMainWindow* MainWindowUI::mainwindow () {
    return &mainwindow_;
}

void MainWindowUI::parseMusic (QFileInfoList& musicFiles) {
    musicFolderPath = QStandardPaths::standardLocations(QStandardPaths::MusicLocation).at(0);
    
    QDir musicDir (musicFolderPath);
    
    QStringList nameFilters;
    nameFilters << "*.mp3" << ".flac" << "*.opus" << "*.wav";
    musicDir.setNameFilters(nameFilters);
    
    musicFiles = musicDir.entryInfoList(QDir::Files);
}

void MainWindowUI::parseConfigFile() {
    QString configPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) +
                         "/gstreamer_player/presets.yaml";
    QFile configFile (configPath);
    
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << configFile.errorString() << '\n';
        createConfigFile(configPath);
    } 
    else {
        QString configString = configFile.readAll();
        configFile.close();
        try {
            config = YAML::Load(configString.toStdString());
        } catch (YAML::ParserException &e) {
            qWarning() << "[Warning]:" << "bad file data. Updating";
            createConfigFile(configPath);
        }
    }

    parsePresets<EqualizerPreset>  ("Equalizer");
    parsePresets<DelayPreset>      ("Delay");
    parsePresets<FilterPreset>     ("Filter");
    parsePresets<PitchPreset>      ("Pitch");
    parsePresets<CompressorPreset> ("Compressor");
}

void MainWindowUI::createConfigFile (QString &configPath) {
    YAML::Emitter out;
    out << YAML::BeginDoc;
    
    out << YAML::BeginMap;
    out << YAML::Key << "Equalizer" << 
             YAML::BeginMap << 
               YAML::Key << "Default" << 
               YAML::BeginSeq << YAML::Flow << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << YAML::EndSeq << 
               YAML::Key << "Bass" << 
               YAML::BeginSeq << YAML::Flow << 30 << 30 << 20 << 10 << 0 << 0 << 0 << 0 << 0 << 0 << YAML::EndSeq << 
               YAML::Key << "Treble" << 
               YAML::BeginSeq << YAML::Flow << 0 << 0 << 0 << 0 << 0 << 10 << 20 << 30 << 30 << 30 << YAML::EndSeq << 
               YAML::Key << "Midrange" << 
               YAML::BeginSeq << YAML::Flow << 0 << 10 << 20 << 30 << 30 << 30 << 20 << 10 << 0 << 0 << YAML::EndSeq << 
               YAML::Key << "Soft" << 
               YAML::BeginSeq << YAML::Flow << -20 << -10 << -10 << 0 << 10 << 10 << 0 << -10 << -20 << -20 << YAML::EndSeq << 
               YAML::Key << "Bright" << 
               YAML::BeginSeq << YAML::Flow << 0 << 0 << 10 << 20 << 30 << 30 << 20 << 10 << 0 << 0 << YAML::EndSeq << 
               YAML::Key << "Full Bass" << 
               YAML::BeginSeq << YAML::Flow << 40 << 30 << 20 << 10 << 0 << -10 << -20 << -30 << -40 << -40 << YAML::EndSeq << 
               YAML::Key << "High-pitched" << 
               YAML::BeginSeq << YAML::Flow << 0 << -10 << -20 << -20 << -10 << 10 << 20 << 20 << 10 << 0 << YAML::EndSeq << 
               YAML::Key << "Bass boost" << 
               YAML::BeginSeq << YAML::Flow << 10 << 20 << 20 << 10 << -10 << -20 << -20 << -10 << 10 << 20 << YAML::EndSeq << 
               YAML::Key << "All" << 
               YAML::BeginSeq << YAML::Flow << 20 << 20 << 20 << 20 << 20 << 20 << 20 << 20 << 20 << 20 << YAML::EndSeq << 
             YAML::EndMap;
    out << YAML::Key << "Delay" <<
             YAML::BeginMap << 
               YAML::Null << 
             YAML::EndMap;
    out << YAML::Key << "Filter" <<
             YAML::BeginMap <<
               YAML::Null << 
             YAML::EndMap;
    out << YAML::Key << "Pitch" <<
             YAML::BeginMap << 
               YAML::Null << 
             YAML::EndMap;
    out << YAML::Key << "Compressor" <<
             YAML::BeginMap << 
               YAML::Null << 
             YAML::EndMap;
    out << YAML::EndMap;
    out << YAML::EndDoc;
    
    config = YAML::Load(out.c_str());
    
    QFile configFile (configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << configFile.errorString() << '\n';
        return;
    }
    configFile.write(out.c_str(), out.size());
    configFile.close();
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

    config[currentPresetType][presetName] = std::move(newPresetSeq);
    parsePresets<T>(currentPresetType);

}

template <class T>
void MainWindowUI::parsePresets(std::string presetKeyName) {
    QVariant data;
    std::string presetName;
    QComboBox *combobox;
    if (!config[presetKeyName].IsDefined()) {
        qDebug() << presetKeyName.c_str() << "does not exits. Skipping";
        return;
    }
    const YAML::Node &element = config[presetKeyName];
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
        }
        combobox->addItem(QString::fromStdString(presetName), data);
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

void MainWindowUI::changeEqualizerParams (QVariant data) {
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

void MainWindowUI::changeDelayParams (QVariant data) {
    DelayPreset params = data.value<DelayPreset>();
    ui.delayDelaySpinBox->setValue(params.delay);
    if ((ui.delaySurrounDelayButton->text() == "On") != params.surroundDelay)
        ui.delaySurrounDelayButton->click();
    ui.delayIntensityDial->setValue(params.intensity);
    ui.delayMaxDelaySpinBox->setValue(params.maxDelay);
    ui.delayFeedbackDial->setValue(params.feedback);
}

void MainWindowUI::changeFilterParams (QVariant data) {
    FilterPreset params = data.value<FilterPreset>();
    if ((ui.filterModeToggleButton->text() == "Low Pass") == params.mode)
        ui.filterModeToggleButton->click();
    ui.filterCutoffSpinBox->setValue(params.cutoff);
    ui.filterRippleSpinBox->setValue(params.ripple);
    ui.filterPolesSpinBox->setValue(params.poles);
    if ((ui.filterFilterTypeButton->text() == "I") == params.filterType)
        ui.filterFilterTypeButton->click();
}

void MainWindowUI::changePitchParams (QVariant data) {
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

void MainWindowUI::setupWidgets () {
    
    audioPositionTimer.setInterval(300);
    audioPanningTimer.setInterval(100);
    
    dialogWindow->setFixedSize(250, 100);
    
    QFileInfoList musicFiles;
    
    parseMusic(musicFiles);
    
    foreach (const QFileInfo &file, musicFiles) {
        QVariant value = QVariant::fromValue(file.absoluteFilePath());
        QListWidgetItem *item = new QListWidgetItem (ui.playerPlayListWidget);
        PlaylistWidgetItem *playlistItem = new PlaylistWidgetItem (file.fileName(), value);
        item->setSizeHint(playlistItem->sizeHint());
        ui.playerPlayListWidget->setItemWidget(item, playlistItem);
    }
}

void MainWindowUI::connectWidgets () {
    connectEqualizerTabWidgets();
    connectPlayerTabWidgets();
    
    connect(&audioPositionTimer,    &QTimer::timeout, this, &MainWindowUI::updateAudioState);
    connect(&audioPanningTimer,     &QTimer::timeout, this, &MainWindowUI::updatePanPosition);
    
    connect(dialogWindow->dialogButtonBox, &QDialogButtonBox::accepted, this, &MainWindowUI::addNewPreset);
    
    connect(dialogWindow->dialogButtonBox, &QDialogButtonBox::rejected, [this]() {
        dialogWindow->dialogLineEdit->clear();
        dialogWindow->hide();
    });
}

//********Player********//
void MainWindowUI::connectPlayerTabWidgets() {
    connect(ui.playerPlayListWidget, &QListWidget::itemClicked, this, &MainWindowUI::playlistItemClicked);
    
    connect(ui.playerTogglePlaylistVisibilityButton, &QPushButton::clicked, this, &MainWindowUI::togglePlaylistView);
    
    connect(ui.playerSeekSlider, &QSlider::sliderPressed, &audioPositionTimer, &QTimer::stop);
    
    connect(ui.playerSeekSlider, &QSlider::valueChanged, [this] (int value) {
        updateAudioPositionLabel(effects->audioDuration*value/1000, effects->audioDuration);
    });
    connect(ui.playerSeekSlider, &QSlider::sliderReleased, [this]() {
        effects->seekPlayingPosition(ui.playerSeekSlider->value());
        audioPositionTimer.start();
    });
    connect(ui.playerPauseButton, &QPushButton::released, [this]() {
        effects->togglePipelineState();
        if (ui.playerPauseButton->text() == "Play")
            ui.playerPauseButton->setText("Pause");
        else
            ui.playerPauseButton->setText("Play");
    });
    connect(ui.playerNextButton, &QPushButton::pressed, this, &MainWindowUI::nextButtonClicked);
    connect(ui.playerPreviousButton, &QPushButton::pressed, this, &MainWindowUI::previousButtonClicked);
}

//********Effects*******//
void MainWindowUI::connectEqualizerTabWidgets () {
    connectEqualizerWidgets  ();
    connectDelayWidgets      ();
    connectFilterWidgets     ();
    connectPitchWidgets      ();
    connectCompressorWidgets ();
    connectPanoramaWidgets   ();
}

void MainWindowUI::connectEqualizerWidgets () {
    connect(ui.equalizerDefaultPresetButton, &QPushButton::clicked, [this]() {
        EqualizerPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        changeEqualizerParams(defaultData);
    });
    connect(ui.equalizerPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeEqualizerParams(ui.equalizerPresetsComboBox->itemData(id));
    });
    connect(ui.equalizerSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Equalizer";
        dialogWindow->show();
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
        dialogWindow->show();
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
        dialogWindow->show();
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
        dialogWindow->show();
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
        dialogWindow->show();
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
//********Others********//

void MainWindowUI::lockWidgetFor (QWidget *widget, quint64 time) {
    widget->blockSignals(true);
    QTimer::singleShot(time, [widget]() {widget->blockSignals(false);});
};

void MainWindowUI::updateAudioPositionLabel(gint64 position, gint64 duration) {
    long long hours_dur, minutes_dur, seconds_dur, 
              hours_pos, minutes_pos, seconds_pos;
    QString string = "%1:%2:%3/%4:%5:%6";
    
    seconds_pos = GST_TIME_AS_SECONDS(position);
    minutes_pos = seconds_pos/60;
    hours_pos   = seconds_pos/3600;
    seconds_dur = GST_TIME_AS_SECONDS(duration);
    minutes_dur = seconds_dur/60;
    hours_dur   = seconds_dur/3600;
    string = string.arg(hours_pos).arg(minutes_pos, 2, 10, QChar('0')).arg(seconds_pos % 60, 2, 10, QChar('0'))
                   .arg(hours_dur).arg(minutes_dur, 2, 10, QChar('0')).arg(seconds_dur % 60, 2, 10, QChar('0'));
    
    ui.playerProgressLabel->setText(string);
}

void MainWindowUI::updateVisualizingWidget () {
//     std::cerr << "update visual\n";
//     
//     std::cerr << visualizingGraphicsView->pos().rx() << ' '
//               << visualizingGraphicsView->pos().ry() << '\n';
// //     std::cerr << visualizingGraphicsView->size()
//     
//     QGraphicsScene *scene = visualizingGraphicsView->scene();
//     
//     QRectF rect (-100, 200, 150, 150);
//     QGraphicsRectItem *rectangle = new QGraphicsRectItem();
//     rectangle->setRect(rect);
//     
//     scene->addItem(rectangle);
}

//**************SLOTS**************//

void MainWindowUI::togglePlaylistView () {
    if (ui.playerPlayListWidget->isVisible()) {
        ui.playerTogglePlaylistVisibilityButton->setText("Show Playlist");
        ui.playerPlayListWidget->setVisible(false);
  } else {
        ui.playerTogglePlaylistVisibilityButton->setText("Hide Playlist");
        ui.playerPlayListWidget->setVisible(true);
  }
}

void MainWindowUI::nextButtonClicked () {
    if (currentAudioId < ui.playerPlayListWidget->count()-1) {
        currentAudioId += 1;
        currentAudio = ui.playerPlayListWidget->item(currentAudioId);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentRow(currentAudioId);
    }
}

void MainWindowUI::previousButtonClicked () {
    if (currentAudioId > 0) {
        currentAudioId -= 1;
        currentAudio = ui.playerPlayListWidget->item(currentAudioId);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
            (ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentRow(currentAudioId);
    }    
}

void MainWindowUI::playlistItemClicked (QListWidgetItem *item) {
    PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>
        (ui.playerPlayListWidget->itemWidget(item));
    effects->changePlayingAudio(playlistItem->filePath());
    currentAudio   = item;
    currentAudioId = ui.playerPlayListWidget->row(currentAudio);
    audioPositionTimer.start();
}

void MainWindowUI::addNewPreset () {
    if      (this->currentPresetType == "Equalizer")
        this->updatePresetConfig<EqualizerPreset>(dialogWindow->getLineInput());
    else if (this->currentPresetType == "Delay")
        this->updatePresetConfig<DelayPreset>(dialogWindow->getLineInput());
    else if (this->currentPresetType == "Filter")
        this->updatePresetConfig<FilterPreset>(dialogWindow->getLineInput());
    else if (this->currentPresetType == "Pitch")
        this->updatePresetConfig<PitchPreset>(dialogWindow->getLineInput());
    else if (this->currentPresetType == "Compressor")
        this->updatePresetConfig<CompressorPreset>(dialogWindow->getLineInput());
    dialogWindow->dialogLineEdit->clear();
    dialogWindow->hide();
}

void MainWindowUI::updateAudioState () {
    effects->updateAudioDuration();
    effects->updateAudioPosition();
    
    if (!effects->isEOSReached()) {
        updateAudioPositionLabel(effects->audioPosition, effects->audioDuration);
        ui.playerSeekSlider->setValue(1000*effects->audioPosition/effects->audioDuration);
    }
        
    if (currentAudioId < ui.playerPlayListWidget->count()-1 && effects->isEOSReached()) {
        currentAudioId += 1;
        currentAudio = ui.playerPlayListWidget->item(currentAudioId);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>(ui.playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath());
        ui.playerPlayListWidget->setCurrentRow(currentAudioId);
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



