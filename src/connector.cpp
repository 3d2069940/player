
#include "metaTypes.h"
#include "player.h"
#include "connector.h"
#include "widgets/effects/presetDialogWindow.h"
#include "widgets/settings/directoryListView.h"
#include "effects.h"


void Connector::connectWidgets(MainWindowUI* ptr) {
    connect(&ptr->audioPositionTimer, &QTimer::timeout, ptr, &MainWindowUI::updateAudioState);
    connect(&ptr->audioPanningTimer, &QTimer::timeout, ptr, &MainWindowUI::updatePanPosition);
    connect(&ptr->audioVisualizingTimer, &QTimer::timeout, ptr, &MainWindowUI::updateVisualizingWidget);
    connect(ptr->presetDialogWindow->dialogButtonBox, &QDialogButtonBox::accepted, ptr, &MainWindowUI::addNewPreset);

    Connector::connectPlayerTab       (ptr);
    Connector::connectEqualizer       (ptr);
    Connector::connectDelay           (ptr);
    Connector::connectFilter          (ptr);
    Connector::connectPitch           (ptr);
    Connector::connectCompressor      (ptr);
    Connector::connectPanorama        (ptr);
    Connector::connectVisualizationTab(ptr);
    Connector::connectSettingsTab     (ptr);
}

void Connector::connectPlayerTab (MainWindowUI *ptr) {
    connect(ptr->ui.playerPlayListWidget,           &QListWidget::itemClicked, ptr, &MainWindowUI::playlistItemClicked);
    connect(ptr->ui.playerPlaylistVisibilityButton, &QPushButton::clicked,     ptr, &MainWindowUI::togglePlaylistView);
    connect(ptr->ui.playerTagListVisibilityButton,  &QPushButton::clicked,     ptr, &MainWindowUI::closeTagListWidget);
    connect(ptr->ui.playerSeekSlider,               &QSlider::sliderPressed,   &ptr->audioPositionTimer, &QTimer::stop);

    connect(ptr->ui.playerSeekSlider, &QSlider::valueChanged, [ptr] (int value) {
        ptr->updateAudioPositionLabel(ptr->effects->audioDuration*value/1000, ptr->effects->audioDuration);
    });
    connect(ptr->ui.playerSeekSlider, &QSlider::sliderReleased, [ptr]() {
        ptr->effects->updateAudioDuration();
        int value = ptr->ui.playerSeekSlider->value(),
            maxValue = ptr->ui.playerSeekSlider->maximum();
        gint64 currentPosition = ptr->effects->audioDuration * value / maxValue;
        ptr->effects->seekPlayingPosition(currentPosition);
        ptr->audioPositionTimer.start();
    });
    connect(ptr->ui.playerPreviousButton, &QPushButton::clicked, ptr, &MainWindowUI::previousButtonClicked);
    connect(ptr->ui.playerRepeatButton,   &QPushButton::clicked, ptr, &MainWindowUI::repeatButtonClicked);
    connect(ptr->ui.playerPauseButton,    &QPushButton::clicked, ptr, &MainWindowUI::pauseButtonClicked);
    connect(ptr->ui.playerShuffleButton,  &QPushButton::clicked, ptr, &MainWindowUI::shuffleButtonClicked);
    connect(ptr->ui.playerNextButton,     &QPushButton::clicked, ptr, &MainWindowUI::nextButtonClicked);
    // shortcuts
    connect(ptr->playPauseShortcut.data(),   &QShortcut::activated, ptr, &MainWindowUI::pauseButtonClicked);
    connect(ptr->nextShortcut.data(),        &QShortcut::activated, ptr, &MainWindowUI::nextButtonClicked);
    connect(ptr->previousShortcut.data(),    &QShortcut::activated, ptr, &MainWindowUI::previousButtonClicked);
    // connect(ptr->seekForwardShortcut.data(), &QShortcut::activated, ptr, &MainWindowUI::seekForward);
}

void Connector::connectEqualizer(MainWindowUI* ptr) {
    connect(ptr->ui.equalizerPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [ptr](int id) {
        ptr->changeEqualizerParams(ptr->ui.equalizerPresetsComboBox->itemData(id));
    });
    connect(ptr->ui.equalizerDefaultPresetButton, &QPushButton::clicked, [ptr]() {
        EqualizerPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        ptr->changeEqualizerParams(defaultData);
    });
    connect(ptr->ui.equalizerDeletePresetButton, &QPushButton::clicked, [ptr] () {
        ptr->removePreset("Equalizer", ptr->ui.equalizerPresetsComboBox);
    });
    connect(ptr->ui.equalizerSavePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->currentPresetType = "Equalizer";
        ptr->presetDialogWindow->show();
    });

    using namespace CODES;
    connect(ptr->ui.equalizer29HzSlider,   &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer29HzGainLabel->setText(QString("%1 dB").arg(value/10.0, 2, 'g'));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider29Hz,  value);
    });
    connect(ptr->ui.equalizer59HzSlider,   &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer59HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider59Hz,  value);
    });
    connect(ptr->ui.equalizer119HzSlider,  &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer119HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider119Hz, value);
    });
    connect(ptr->ui.equalizer237HzSlider,  &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer237HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider237Hz, value);
    });
    connect(ptr->ui.equalizer474HzSlider,  &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer474HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider474Hz, value);
    });
    connect(ptr->ui.equalizer1000HzSlider, &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer1000HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider1kHz,  value);
    });
    connect(ptr->ui.equalizer2000HzSlider, &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer2000HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider2kHz,  value);
    });
    connect(ptr->ui.equalizer4000HzSlider, &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer4000HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider4kHz,  value);
    });
    connect(ptr->ui.equalizer8000HzSlider, &QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer8000HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider8kHz,  value);
    });
    connect(ptr->ui.equalizer16000HzSlider,&QSlider::valueChanged, [ptr](int value) {
        ptr->ui.equalizer16000HzGainLabel->setText(QString("%1 dB").arg(value/10.0));
        ptr->effects->changeEqualizerProps(EQUALIZER::Slider16kHz, value);
    });
    connect(ptr->ui.equalizerVolumeSlider, &QSlider::valueChanged, [ptr](int value) {
        ptr->effects->changeEqualizerProps(EQUALIZER::Volume,      value);
    });
}

void Connector::connectDelay(MainWindowUI* ptr) {
    connect(ptr->ui.delayDefaultPresetButton, &QPushButton::clicked, [ptr]() {
        DelayPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        ptr->changeDelayParams(defaultData);
    });
    connect(ptr->ui.delayPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [ptr](int id) {
        ptr->changeDelayParams(ptr->ui.delayPresetsComboBox->itemData(id));
    });
    connect(ptr->ui.delaySavePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->currentPresetType = "Delay";
        ptr->presetDialogWindow->show();
    });
    connect(ptr->ui.delayDeletePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->removePreset("Delay", ptr->ui.delayPresetsComboBox);
    });

    using namespace CODES;
    connect(ptr->ui.delayDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [ptr](int value) {
        ptr->effects->changeDelayProps(CODES::Delay, (gdouble)value);
    });
    connect(ptr->ui.delaySurroundDelayButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changeDelayProps(CODES::SurroundDelay);
    });
    connect(ptr->ui.delayIntensityDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changeDelayProps(CODES::Intensity, value);
    });
    connect(ptr->ui.delayMaxDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [ptr](int value) {
        ptr->effects->changeDelayProps(CODES::MaxDelay, value);
    });
    connect(ptr->ui.delayFeedbackDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changeDelayProps(CODES::Feedback, value);
    });
}

void Connector::connectFilter(MainWindowUI* ptr) {
    connect(ptr->ui.filterPresetSavePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->currentPresetType = "Filter";
        ptr->presetDialogWindow->show();
    });
    connect(ptr->ui.filterDeletePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->removePreset("Filter", ptr->ui.filterPresetsComboBox);
    });
    connect(ptr->ui.filterDefaultPresetButton, &QPushButton::clicked, [ptr]() {
        FilterPreset defaultPreset;
        QVariant defaultData = QVariant::fromValue(defaultPreset);
        ptr->changeFilterParams(defaultData);
    });
    connect(ptr->ui.filterPresetsComboBox, QOverload<int>::of(&QComboBox::activated), [ptr](int id) {
        ptr->changeFilterParams(ptr->ui.filterPresetsComboBox->itemData(id));
    });

    using namespace CODES;
    connect(ptr->ui.filterModeToggleButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changeFilterProps(FILTER::Mode);
        if (ptr->ui.filterModeToggleButton->text() == "Low Pass")
            ptr->ui.filterModeToggleButton->setText("High Pass");
        else
            ptr->ui.filterModeToggleButton->setText("Low Pass");
    });
    connect(ptr->ui.filterCutoffSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [ptr](int value) {
        ptr->effects->changeFilterProps(FILTER::Cutoff, (gfloat)value);
    });
    connect(ptr->ui.filterRippleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [ptr](double value) {
        ptr->effects->changeFilterProps(FILTER::Ripple, (gfloat)value);
    });
    connect(ptr->ui.filterPolesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [ptr](int value) {
        ptr->effects->changeFilterProps(FILTER::Poles, (gfloat)value);
    });
    connect(ptr->ui.filterFilterTypeButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changeFilterProps(FILTER::FilterType);
        if (ptr->ui.filterFilterTypeButton->text() == "I")
            ptr->ui.filterFilterTypeButton->setText("II");
        else
            ptr->ui.filterFilterTypeButton->setText("I");
    });
}

void Connector::connectPitch(MainWindowUI* ptr) {
    connect(ptr->ui.pitchSavePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->currentPresetType = "Pitch";
        ptr->presetDialogWindow->show();
    });
    connect(ptr->ui.pitchDeletePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->removePreset("Pitch", ptr->ui.pitchPresetComboBox);
    });
    connect(ptr->ui.pitchDefaultPresetButton, &QPushButton::clicked, [ptr]() {
        PitchPreset defaultPreset;
        QVariant data = QVariant::fromValue(defaultPreset);
        ptr->changePitchParams(data);
    });
    connect(ptr->ui.pitchPresetComboBox, QOverload<int>::of(&QComboBox::activated), [ptr](int id) {
        ptr->changePitchParams(ptr->ui.pitchPresetComboBox->itemData(id));
    });

    using namespace CODES;
    connect(ptr->ui.pitchTempoDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changePitchProps(PITCH::Tempo, value/10.0);
    });
    connect(ptr->ui.pitchPitchDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changePitchProps(PITCH::Pitch, value/10.0);
    });
    connect(ptr->ui.pitchRateDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changePitchProps(PITCH::Rate, value/10.0);
    });
    connect(ptr->ui.pitchOutputRateDial,&QDial::valueChanged, [ptr](int value) {
        ptr->effects->changePitchProps(PITCH::OutputRate, value/10.0);
    });
}

void Connector::connectCompressor(MainWindowUI* ptr) {
    connect(ptr->ui.compressorSavePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->currentPresetType = "Compressor";
        ptr->presetDialogWindow->show();
    });
    connect(ptr->ui.compressorDeletePresetButton, &QPushButton::clicked, [ptr]() {
        ptr->removePreset("Compressor", ptr->ui.compressorPresetComboBox);
    });
    connect(ptr->ui.compressorDefaultPresetButton, &QPushButton::clicked, [ptr]() {
        CompressorPreset defaultPreset;
        QVariant data = QVariant::fromValue(defaultPreset);
        ptr->changeCompressorParams(data);
    });
    connect(ptr->ui.compressorPresetComboBox, QOverload<int>::of(&QComboBox::activated), [ptr](int id) {
        ptr->changeCompressorParams(ptr->ui.compressorPresetComboBox->itemData(id));
    });

    using namespace CODES;
    connect(ptr->ui.compressorExpanderToggleButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changeCompressorProps(COMPRESSOR::Compressor);
    });
    connect(ptr->ui.compressorKneeToggleButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changeCompressorProps(COMPRESSOR::Knee);
    });
    connect(ptr->ui.compressorRatioDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changeCompressorProps(COMPRESSOR::Ratio, value);
    });
    connect(ptr->ui.compressorThresholdDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changeCompressorProps(COMPRESSOR::Threshold, value);
    });
}

void Connector::connectPanorama(MainWindowUI* ptr) {
    using namespace CODES;

    connect(ptr->ui.panoramaPanningMethodButton, &QPushButton::clicked, [ptr]() {
        ptr->effects->changePanoramaProps(PANORAMA::Method);
        if (ptr->ui.panoramaPanningMethodButton->text() == "Simple")
            ptr->ui.panoramaPanningMethodButton->setText("Psychoacoustic");
        else
            ptr->ui.panoramaPanningMethodButton->setText("Simple");
    });
    connect(ptr->ui.panoramaPositionDial, &QDial::valueChanged, [ptr](int value) {
        ptr->effects->changePanoramaProps(PANORAMA::Position, value);
    });
    connect(ptr->ui.panoramaAutoPanoramaButton, &QPushButton::clicked, [ptr]() {
        if (ptr->audioPanningTimer.isActive()) {
            ptr->ui.panoramaAutoPanoramaButton->setText("Off");
            ptr->audioPanningTimer.stop();
        }
        else {
            ptr->ui.panoramaAutoPanoramaButton->setText("On");
            ptr->audioPanningTimer.start();
        }
        ptr->ui.panoramaSpeedDial->setEnabled(!ptr->ui.panoramaSpeedDial->isEnabled());
    });
    connect(ptr->ui.panoramaSpeedDial, &QDial::valueChanged, [ptr](int value) {
        ptr->audioPanningTimer.stop();
        ptr->audioPanningTimer.setInterval(ptr->ui.panoramaSpeedDial->maximum()+10-value);
        ptr->audioPanningTimer.start();
    });
}

void Connector::connectVisualizationTab(MainWindowUI* ptr) {
    connect(ptr->ui.visualizingBandsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            ptr, &MainWindowUI::bandsChanged);
    connect(ptr->ui.visualizingRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            ptr, &MainWindowUI::updateRateChanged);
    connect(ptr->ui.visualizingThresholdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            ptr, &MainWindowUI::thresholdChanged);
}

void Connector::connectSettingsTab(MainWindowUI* ptr) {
    connect(ptr->ui.settingsColorThemeComboBox, &QComboBox::currentTextChanged, ptr, &MainWindowUI::themeComboBoxClicked);
    connect(ptr->ui.settingsMediaFoldersButton, &QPushButton::clicked, ptr, &MainWindowUI::showSettingsTreeWidget);
    connect(ptr->ui.settingsDirectoryListView, &DirectoryListView::selectedFolderUpdated, ptr, &MainWindowUI::newFolderSelected);
    foreach (QAction *action, ptr->extMenu->actions())
        connect(action, &QAction::triggered, ptr, &MainWindowUI::extensionsMenuClicked);
}




