
#pragma once

#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "presetTypes.h"
#include "player.h"

#include "presetDialogWindow.hpp"
#include "playlistWidgetItem.hpp"
#include "effects.hpp"



MainWindowUI::MainWindowUI () {
    setupUi(&mainwindow_);
    playerPlayListWidget->hide();
    
    effects      = std::make_unique<Effects>();
    dialogWindow = std::make_unique<PresetDialogWindow>();
   
    parseConfigFile();

    setupWidgets();
    connectWidgets();
}

MainWindowUI::~MainWindowUI() {
    
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

    parsePresets<EqualizerPreset>  ("Equalizer",  equalizerPresetsComboBox);
    parsePresets<DelayPreset>      ("Delay",      delayPresetsComboBox);
    parsePresets<FilterPreset>     ("Filter",     filterPresetsComboBox);
    parsePresets<PitchPreset>      ("Pitch",      pitchPresetComboBox);
    parsePresets<CompressorPreset> ("Compressor", compressorPresetComboBox);
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

void MainWindowUI::updatePresetConfig (std::string presetType, std::string presetName) {
    YAML::Node newPresetSeq (YAML::NodeType::Sequence);
    newPresetSeq.SetStyle(YAML::EmitterStyle::Flow);
    if (presetType == "Equalizer") {
        serializeEqualizerParams(&newPresetSeq);
        config[presetType][presetName] = std::move(newPresetSeq);
        parsePresets<EqualizerPreset>(presetType, equalizerPresetsComboBox);
  } else if (presetType == "Delay") {
        serializeDelayParams(&newPresetSeq);
        config[presetType][presetName] = std::move(newPresetSeq);
        parsePresets<DelayPreset>(presetType, delayPresetsComboBox);
  } else if (presetType == "Filter") {
        serializeFilterParams(&newPresetSeq);
        config[presetType][presetName] = std::move(newPresetSeq);
        parsePresets<FilterPreset>(presetType, filterPresetsComboBox);
  } else if (presetType == "Pitch") {
        serializePitchParams(&newPresetSeq);
        config[presetType][presetName] = std::move(newPresetSeq);
        parsePresets<PitchPreset>(presetType, pitchPresetComboBox);
  } else if (presetType == "Compressor") {
        serializeCompressorParams(&newPresetSeq);
        config[presetType][presetName] = std::move(newPresetSeq);
        parsePresets<CompressorPreset>(presetType, compressorPresetComboBox);
  }
}

template <class T>
void MainWindowUI::parsePresets(std::string presetKeyName, QComboBox* combobox) {
    QVariant data;
    std::string presetName;
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
            // Delay presets
          } else if constexpr (std::is_same<T, DelayPreset>::value) {
                data = QVariant::fromValue(it->second.as<DelayPreset>());
            // Filter presets
          } else if constexpr (std::is_same<T, FilterPreset>::value) {
                data = QVariant::fromValue(it->second.as<FilterPreset>());
            // Compressor presets
          } else if constexpr (std::is_same<T, CompressorPreset>::value) {
                data = QVariant::fromValue(it->second.as<CompressorPreset>());
          }
        } catch (YAML::BadConversion &e) {
            qWarning() << "[Warning]:" << e.what();
            continue;
        }
        combobox->addItem(QString::fromStdString(presetName), data);
    }
}

void MainWindowUI::serializeEqualizerParams  (YAML::Node *node) {
    node->push_back(equalizer29HzSlider->value());
    node->push_back(equalizer59HzSlider->value());
    node->push_back(equalizer119HzSlider->value());
    node->push_back(equalizer237HzSlider->value());
    node->push_back(equalizer474HzSlider->value());
    node->push_back(equalizer1000HzSlider->value());
    node->push_back(equalizer2000HzSlider->value());
    node->push_back(equalizer4000HzSlider->value());
    node->push_back(equalizer8000HzSlider->value());
    node->push_back(equalizer16000HzSlider->value());
}

void MainWindowUI::serializeDelayParams      (YAML::Node *node) {
    node->push_back(delayDelaySpinBox->value());
    node->push_back(delaySurrounDelayButton->text() != "Off");
    node->push_back(delayIntensityDial->value());
    node->push_back(delayMaxDelaySpinBox->value());
    node->push_back(delayFeedbackDial->value());
}

void MainWindowUI::serializeFilterParams     (YAML::Node *node) {
    node->push_back(filterModeToggleButton->text() != "Low Pass");
    node->push_back(filterCutoffSpinBox->value());
    node->push_back(filterRippleSpinBox->value());
    node->push_back(filterPolesSpinBox->value());
    node->push_back(filterFilterTypeButton->text() != "I");
}

void MainWindowUI::serializePitchParams      (YAML::Node *node) {
    node->push_back(pitchTempoDial->value());
    node->push_back(pitchPitchDial->value());
    node->push_back(pitchRateDial->value());
    node->push_back(pitchOutputRateDial->value());
}

void MainWindowUI::serializeCompressorParams (YAML::Node *node) {
    node->push_back(compressorExpanderToggleButton->text() != "Compressor");
    node->push_back(compressorKneeToggleButton->text() != "Hard-knee");
    node->push_back(compressorRatioDial->value());
    node->push_back(compressorThresholdDial->value());
}

void MainWindowUI::changeEqualizerParams (QVariant data) {
    EqualizerPreset params = data.value<EqualizerPreset>();
    equalizer29HzSlider->setValue(params.slider29Hz);
    equalizer59HzSlider->setValue(params.slider59Hz);
    equalizer119HzSlider->setValue(params.slider119Hz);
    equalizer237HzSlider->setValue(params.slider237Hz);
    equalizer474HzSlider->setValue(params.slider474Hz);
    equalizer1000HzSlider->setValue(params.slider1kHz);
    equalizer2000HzSlider->setValue(params.slider2kHz);
    equalizer4000HzSlider->setValue(params.slider4kHz);
    equalizer8000HzSlider->setValue(params.slider8kHz);
    equalizer16000HzSlider->setValue(params.slider16kHz);
}

void MainWindowUI::changeDelayParams (QVariant data) {
    DelayPreset params = data.value<DelayPreset>();
    delayDelaySpinBox->setValue(params.delay);
    if ((delaySurrounDelayButton->text() == "On") != params.surroundDelay)
        delaySurrounDelayButton->click();
    delayIntensityDial->setValue(params.intensity);
    delayMaxDelaySpinBox->setValue(params.maxDelay);
    delayFeedbackDial->setValue(params.feedback);
}

void MainWindowUI::changeFilterParams (QVariant data) {
    FilterPreset params = data.value<FilterPreset>();
    if ((filterModeToggleButton->text() == "Low Pass") == params.mode)
        filterModeToggleButton->click();
    filterCutoffSpinBox->setValue(params.cutoff);
    filterRippleSpinBox->setValue(params.ripple);
    filterPolesSpinBox->setValue(params.poles);
    if ((filterFilterTypeButton->text() == "I") == params.filterType)
        filterFilterTypeButton->click();
}

void MainWindowUI::changePitchParams (QVariant data) {
    PitchPreset params = data.value<PitchPreset>();
    pitchTempoDial->setValue(params.tempo);
    pitchPitchDial->setValue(params.pitch);
    pitchRateDial->setValue(params.rate);
    pitchOutputRateDial->setValue(params.outputRate);
}

void MainWindowUI::changeCompressorParams (QVariant data) {
    CompressorPreset params = data.value<CompressorPreset>();
    if ((compressorExpanderToggleButton->text() == "Compressor") == params.compressor)
        compressorExpanderToggleButton->click();
    if ((compressorKneeToggleButton->text() == "Hard-knee") == params.kneeType)
        compressorKneeToggleButton->click();
    compressorRatioDial->setValue(params.ratio);
    compressorThresholdDial->setValue(params.threshold);
}

void MainWindowUI::setupWidgets () {
    
    audioPositionTimer.setInterval(300);
    audioPanningTimer.setInterval(100);
    
    dialogWindow->setFixedSize(250, 100);
    
    QFileInfoList musicFiles;
    
    parseMusic(musicFiles);
    
    foreach (const QFileInfo &file, musicFiles) {
        QVariant value = QVariant::fromValue(file.absoluteFilePath());
        QListWidgetItem *item = new QListWidgetItem (playerPlayListWidget);
        PlaylistWidgetItem *playlistItem = new PlaylistWidgetItem (file.fileName(), value);
        item->setSizeHint(playlistItem->sizeHint());
        playerPlayListWidget->setItemWidget(item, playlistItem);
    }
}

void MainWindowUI::connectWidgets () {
    connectEqualizerTabWidgets();
    connectPlayerTabWidgets();
    
    QObject::connect(&audioPositionTimer,    &QTimer::timeout, [this]() {updateAudioState();});
    QObject::connect(&audioPanningTimer,     &QTimer::timeout, [this]() {updatePanPosition();});
    
    QObject::connect(dialogWindow->dialogButtonBox, &QDialogButtonBox::accepted, [this]() {
        this->updatePresetConfig(this->currentPresetType, dialogWindow->getLineInput());
        dialogWindow->dialogLineEdit->clear();
        dialogWindow->hide();
    });
    QObject::connect(dialogWindow->dialogButtonBox, &QDialogButtonBox::rejected, [this]() {
        dialogWindow->dialogLineEdit->clear();
        dialogWindow->hide();
    });
}

//********Player********//
void MainWindowUI::connectPlayerTabWidgets() {
    QObject::connect(playerPlayListWidget, &QListWidget::itemClicked, [this] (QListWidgetItem* item) {
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>(playerPlayListWidget->itemWidget(item));
        effects->changePlayingAudio(playlistItem->filePath().toStdString());
        currentAudio   = item;
        currentAudioId = playerPlayListWidget->row(currentAudio);
        audioPositionTimer.start();
    });
    QObject::connect(playerTogglePlaylistVisibilityButton, &QPushButton::clicked, [this] () {
        if (playerPlayListWidget->isVisible())
            playerTogglePlaylistVisibilityButton->setText("Show Playlist");
        else
            playerTogglePlaylistVisibilityButton->setText("Hide Playlist");
        playerPlayListWidget->setVisible(!playerPlayListWidget->isVisible());
    });
    QObject::connect(playerSeekSlider, &QSlider::sliderPressed, [this] () {
        audioPositionTimer.stop();
    });
    QObject::connect(playerSeekSlider, &QSlider::valueChanged, [this] (int value) {
        updateAudioPositionLabel(effects->audioDuration*value/1000, effects->audioDuration);
    });
    QObject::connect(playerSeekSlider, &QSlider::sliderReleased, [this]() {
        effects->seekPlayingPosition(playerSeekSlider->value());
        audioPositionTimer.start();
    });
    QObject::connect(playerPauseButton, &QPushButton::released, [this]() {
         effects->togglePipelineState();
        if (playerPauseButton->text() == "Play")
            playerPauseButton->setText("Pause");
        else
            playerPauseButton->setText("Play");
    });
    QObject::connect(playerNextButton, &QPushButton::pressed, [this]() {
        if (currentAudioId < playerPlayListWidget->count()-1) {
            currentAudioId += 1;
            currentAudio = playerPlayListWidget->item(currentAudioId);
            PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>(playerPlayListWidget->itemWidget(currentAudio));
            effects->changePlayingAudio(playlistItem->filePath().toStdString());
            playerPlayListWidget->setCurrentRow(currentAudioId);
        }
    });
    QObject::connect(playerPreviousButton, &QPushButton::pressed, [this]() {
        if (currentAudioId > 0) {
            currentAudioId -= 1;
            currentAudio = playerPlayListWidget->item(currentAudioId);
            PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>(playerPlayListWidget->itemWidget(currentAudio));
            effects->changePlayingAudio(playlistItem->filePath().toStdString());
            playerPlayListWidget->setCurrentRow(currentAudioId);
        }
    });
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
    QObject::connect(equalizerDefaultPresetButton, &QPushButton::clicked, [this]() {
        EqualizerPreset defaultPreset = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        changeEqualizerParams(defaultData);
    });
    QObject::connect(equalizerPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeEqualizerParams(equalizerPresetsComboBox->itemData(id));
    });
    QObject::connect(equalizerSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Equalizer";
        dialogWindow->exec();
    });
    
    using namespace CODES;
    QObject::connect(equalizer29HzSlider,   &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider29Hz,  value);
    });
    QObject::connect(equalizer59HzSlider,   &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider59Hz,  value);
    });
    QObject::connect(equalizer119HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider119Hz, value);
    });
    QObject::connect(equalizer237HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider237Hz, value);
    });
    QObject::connect(equalizer474HzSlider,  &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider474Hz, value);
    });
    QObject::connect(equalizer1000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider1kHz,  value);
    });
    QObject::connect(equalizer2000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider2kHz,  value);
    });
    QObject::connect(equalizer4000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider4kHz,  value);
    });
    QObject::connect(equalizer8000HzSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider8kHz,  value);
    });
    QObject::connect(equalizer16000HzSlider,&QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Slider16kHz, value);
    });
    QObject::connect(equalizerVolumeSlider, &QSlider::valueChanged, [this](int value) {
        effects->changeEqualizerProps(EQUALIZER::Volume,      value);
    });
}

void MainWindowUI::connectDelayWidgets() {
    QObject::connect(delayDefaultPresetButton, &QPushButton::clicked, [this]() {
        DelayPreset defaultPreset = {1, false, 0, 5000, 0};
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        this->changeDelayParams(defaultData);
    });
    QObject::connect(delayPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeDelayParams(delayPresetsComboBox->itemData(id));
    });
    QObject::connect(delaySavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Delay";
        dialogWindow->exec();
    });
    
    using namespace CODES;
    QObject::connect(delayDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeDelayProps(CODES::Delay, (gdouble)value);
    });
    QObject::connect(delaySurrounDelayButton, &QPushButton::clicked, [this]() {
        effects->changeDelayProps(CODES::SurroundDelay);
        if (delaySurrounDelayButton->text() == "Off")
            delaySurrounDelayButton->setText("On");
        else
            delaySurrounDelayButton->setText("Off");
    });
    QObject::connect(delayIntensityDial, &QDial::valueChanged, [this](int value) {
        effects->changeDelayProps(CODES::Intensity, value);
    });
    QObject::connect(delayMaxDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeDelayProps(CODES::MaxDelay, value);
    });
    QObject::connect(delayFeedbackDial, &QDial::valueChanged, [this](int value) {
        effects->changeDelayProps(CODES::Feedback, value);
    });
}

void MainWindowUI::connectFilterWidgets() {
    QObject::connect(filterPresetSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Filter";
        dialogWindow->exec();
    });
    QObject::connect(filterDefaultPresetButton, &QPushButton::clicked, [this]() {
        FilterPreset defaultPreset = {false, 48000, 0.25, 4, false};
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        changeFilterParams(defaultData);
    });
    QObject::connect(filterPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeFilterParams(filterPresetsComboBox->itemData(id));
    });
    
    using namespace CODES;
    QObject::connect(filterModeToggleButton, &QPushButton::clicked, [this]() {
        effects->changeFilterProps(FILTER::Mode);
        if (filterModeToggleButton->text() == "Low Pass")
            filterModeToggleButton->setText("High Pass");
        else
            filterModeToggleButton->setText("Low Pass");
    });
    QObject::connect(filterCutoffSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeFilterProps(FILTER::Cutoff, (gfloat)value);
    });
    QObject::connect(filterRippleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        effects->changeFilterProps(FILTER::Ripple, (gfloat)value);
    });
    QObject::connect(filterPolesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        effects->changeFilterProps(FILTER::Poles, (gfloat)value);
    });
    QObject::connect(filterFilterTypeButton, &QPushButton::clicked, [this]() {
        effects->changeFilterProps(FILTER::FilterType);
        if (filterFilterTypeButton->text() == "I")
            filterFilterTypeButton->setText("II");
        else
            filterFilterTypeButton->setText("I");
    });
}

void MainWindowUI::connectPitchWidgets () {
    QObject::connect(pitchSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Pitch";
        dialogWindow->exec();
    });
    QObject::connect(pitchDefaultPresetButton, &QPushButton::clicked, [this]() {
        PitchPreset defaultPreset = {10, 10, 10, 10};
        QVariant data = QVariant::fromValue(defaultPreset);
        this->changePitchParams(data);
    });
    QObject::connect(pitchPresetComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changePitchParams(pitchPresetComboBox->itemData(id));
    });
    
    using namespace CODES;
    QObject::connect(pitchTempoDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Tempo, value/10.0);
    });
    QObject::connect(pitchPitchDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Pitch, value/10.0);
    });
    QObject::connect(pitchRateDial, &QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::Rate, value/10.0);
    });
    QObject::connect(pitchOutputRateDial,&QDial::valueChanged, [this](int value) {
        effects->changePitchProps(PITCH::OutputRate, value/10.0);
    });
}

void MainWindowUI::connectCompressorWidgets () {
    QObject::connect(compressorSavePresetButton, &QPushButton::clicked, [this]() {
        currentPresetType = "Compressor";
        dialogWindow->exec();
    });
    
    QObject::connect(compressorDefaultPresetButton, &QPushButton::clicked, [this]() {
        CompressorPreset defaultPreset = {false, false, 10, 0};
        QVariant data = QVariant::fromValue(defaultPreset);
        this->changeCompressorParams(data);
    });
    QObject::connect(compressorPresetComboBox, QOverload<int>::of(&QComboBox::activated), [this](int id) {
        this->changeCompressorParams(compressorPresetComboBox->itemData(id));
    });
    
    using namespace CODES;
    QObject::connect(compressorExpanderToggleButton, &QPushButton::clicked, [this]() {
        effects->changeCompressorProps(COMPRESSOR::Compressor);
        if (compressorExpanderToggleButton->text() == "Compressor")
            compressorExpanderToggleButton->setText("Expander");
        else
            compressorExpanderToggleButton->setText("Compressor");
    });
    QObject::connect(compressorKneeToggleButton, &QPushButton::clicked, [this]() {
        effects->changeCompressorProps(COMPRESSOR::Knee);
        if (compressorKneeToggleButton->text() == "Hard-knee")
            compressorKneeToggleButton->setText("Soft-knee");
        else
            compressorKneeToggleButton->setText("Hard-knee");
    });
    QObject::connect(compressorRatioDial, &QDial::valueChanged, [this](int value) {
        effects->changeCompressorProps(COMPRESSOR::Ratio, value);
    });
    QObject::connect(compressorThresholdDial, &QDial::valueChanged, [this](int value) {
        effects->changeCompressorProps(COMPRESSOR::Threshold, value);
    });
}

void MainWindowUI::connectPanoramaWidgets () {
    using namespace CODES;
    
    QObject::connect(panoramaPanningMethodButton, &QPushButton::clicked, [this]() {
        effects->changePanoramaProps(PANORAMA::Method);
        if (panoramaPanningMethodButton->text() == "Simple")
            panoramaPanningMethodButton->setText("Psychoacoustic");
        else
            panoramaPanningMethodButton->setText("Simple");
    });
    QObject::connect(panoramaPositionDial, &QDial::valueChanged, [this](int value) {
        effects->changePanoramaProps(PANORAMA::Position, value);
    });
    QObject::connect(panoramaAutoPanoramaButton, &QPushButton::clicked, [this]() {
        if (audioPanningTimer.isActive()) {
            panoramaAutoPanoramaButton->setText("Off");
            audioPanningTimer.stop();
        }
        else {
            panoramaAutoPanoramaButton->setText("On");
            audioPanningTimer.start();
        }
        panoramaSpeedDial->setEnabled(!panoramaSpeedDial->isEnabled());
    });
    QObject::connect(panoramaSpeedDial, &QDial::valueChanged, [this](int value) {
        audioPanningTimer.stop();
        audioPanningTimer.setInterval(panoramaSpeedDial->maximum()+10-value);
        audioPanningTimer.start();
    });
}
//********Others********//

void MainWindowUI::lockWidgetFor (QWidget *widget, quint64 time) {
    widget->blockSignals(true);
    QTimer::singleShot(time, [widget]() {widget->blockSignals(false);});
};

void MainWindowUI::updateAudioState () {
    effects->updateAudioDuration();
    effects->updateAudioPosition();
    
    if (!effects->isEOSReached()) {
        updateAudioPositionLabel(effects->audioPosition, effects->audioDuration);
        playerSeekSlider->setValue(1000*effects->audioPosition/effects->audioDuration);
    }
        
    if (currentAudioId < playerPlayListWidget->count()-1 && effects->isEOSReached()) {
        currentAudioId += 1;
        currentAudio = playerPlayListWidget->item(currentAudioId);
        PlaylistWidgetItem *playlistItem = qobject_cast<PlaylistWidgetItem*>(playerPlayListWidget->itemWidget(currentAudio));
        effects->changePlayingAudio(playlistItem->filePath().toStdString());
        playerPlayListWidget->setCurrentRow(currentAudioId);
    }
}

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
    
    playerProgressLabel->setText(string);
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

void MainWindowUI::updatePanPosition () {
    static bool direction = true;
    int panPosition = panoramaPositionDial->value();
    direction = (panPosition + 10 > panoramaPositionDial->maximum() || 
                 panPosition - 10 < panoramaPositionDial->minimum()) ? 
                 !direction : direction;
    panoramaPositionDial->setValue(panPosition + (direction ? 10 : -10));
}
