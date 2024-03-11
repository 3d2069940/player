
#ifndef _EFFECTSTAB_HPP_
#define _EFFECTSTAB_HPP_


#include <QSlider>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <qdialogbuttonbox.h>
#include <qpushbutton.h>

#include "effectstab.h"

#include "src/effects/effects.h"
#include "src/tabs/effectsTab/metatypes.h"


EffectsTab::EffectsTab (QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    createWidgets();
    setupWidgets();
    connectWidgets();
}

EffectsTab::~EffectsTab () {
    
}

void EffectsTab::setEffects (Effects *_effects) {
    effects = _effects;
}

void EffectsTab::createWidgets () {

}

void EffectsTab::setupWidgets () {
    ui.delaySurroundDelayButton->setLabels({"Off", "On"});

    ui.filterModeToggleButton->setLabels({"Low Pass","High Pass"});
    ui.filterFilterTypeButton->setLabels({"I","II"});

    ui.compressorCompressorButton->setLabels({"Compressor", "Expander"});
    ui.compressorKneeTypeButton->setLabels({"Hard-knee","Soft-knee"});

    ui.panoramaPanningMethodButton->setLabels({"Psychoacoustic", "Simple"});
    ui.panoramaAutoPanoramaButton->setLabels({"Off","On"});
}

void EffectsTab::connectWidgets () {
    connectEqualizer ();
    connectDelay     ();
    connectFilter    ();
    connectPitch     ();
    connectCompressor();
    connectPanorama  ();
}

void EffectsTab::connectEqualizer () {
//  control buttons
    connect(ui.equalizerButtonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [&]() {
        equalizerResetButtonClicked(EqualizerPreset());});
//  sliders
    using namespace CODES;
    connect(ui.equalizer29HzSlider, &QSlider::valueChanged, [&](int value){
        onEqualizerValueChanged(EQUALIZER::Slider29Hz, value);});
    connect(ui.equalizer59HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider59Hz, value);});
    connect(ui.equalizer119HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider119Hz, value);});
    connect(ui.equalizer237HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider237Hz, value);});
    connect(ui.equalizer474HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider474Hz, value);});
    connect(ui.equalizer1000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider1kHz, value);});
    connect(ui.equalizer2000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider2kHz, value);});
    connect(ui.equalizer4000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider4kHz, value);});
    connect(ui.equalizer8000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider8kHz, value);});
    connect(ui.equalizer16000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(EQUALIZER::Slider16kHz, value);});
    connect(ui.equalizerVolumeSlider,&QSlider::valueChanged,[&](int value) {
        onEqualizerValueChanged(EQUALIZER::Volume, value);});
}

void EffectsTab::connectDelay () {
//  control buttons
    connect(ui.delayButtonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [&]() {
        delayResetButtonClicked(DelayPreset());});
//  effect
    using namespace CODES;
    connect(ui.delayDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onDelayValueChanged(DELAY::Delay, value);});
    connect(ui.delaySurroundDelayButton, &QPushButton::clicked, [&]() {
        onDelayValueChanged(DELAY::SurroundDelay);});
    connect(ui.delayIntensityDial, &QDial::valueChanged, [&](int value) {
        onDelayValueChanged(DELAY::Intensity, value);});
    connect(ui.delayMaxDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onDelayValueChanged(DELAY::MaxDelay, value);});
    connect(ui.delayFeedbackDial, &QDial::valueChanged, [&](int value) {
        onDelayValueChanged(DELAY::Feedback, value);});
}

void EffectsTab::connectFilter () {
//  control buttons
    connect(ui.filterButtonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [&]() {
        filterResetButtonClicked(FilterPreset());});
//  effect
    using namespace CODES;
    connect(ui.filterModeToggleButton, &QPushButton::clicked, [&]() {
        onFilterValueChanged(FILTER::Mode);});
    connect(ui.filterCutoffSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onFilterValueChanged(FILTER::Cutoff, value);});
    connect(ui.filterRippleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
        onFilterValueChanged(FILTER::Ripple, value);});
    connect(ui.filterPolesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onFilterValueChanged(FILTER::Poles, value);});
    connect(ui.filterFilterTypeButton, &QPushButton::clicked, [&]() {
        onFilterValueChanged(FILTER::FilterType);});
}

void EffectsTab::connectPitch () {
//  control buttons
    connect(ui.pitchButtonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [&]() {
        pitchResetButtonClicked(PitchPreset());});
//  effect
    using namespace CODES;
    connect(ui.pitchTempoDial, &QDial::valueChanged, [&](int value) {
        onPitchValueChanged(PITCH::Tempo, value);});
    connect(ui.pitchPitchDial, &QDial::valueChanged, [&](int value) {
        onPitchValueChanged(PITCH::Pitch, value);});
    connect(ui.pitchRateDial, &QDial::valueChanged, [&](int value) {
        onPitchValueChanged(PITCH::Rate, value);});
    connect(ui.pitchOutputRateDial,&QDial::valueChanged, [&](int value) {
        onPitchValueChanged(PITCH::OutputRate, value);});
}

void EffectsTab::connectCompressor () {
//  control buttons
    connect(ui.compressorButtonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [&]() {
        compressorResetButtonClicked(CompressorPreset());});
//  effects
    using namespace CODES;
    connect(ui.compressorCompressorButton, &QPushButton::clicked, [&]() {
        onCompressorValueChanged(COMPRESSOR::Compressor);});
    connect(ui.compressorKneeTypeButton, &QPushButton::clicked, [&]() {
        onCompressorValueChanged(COMPRESSOR::Knee);});
    connect(ui.compressorRatioDial, &QDial::valueChanged, [&](int value) {
        onCompressorValueChanged(COMPRESSOR::Ratio, value);});
    connect(ui.compressorThresholdDial, &QDial::valueChanged, [&](int value) {
        onCompressorValueChanged(COMPRESSOR::Threshold, value);});
}

void EffectsTab::connectPanorama () {
    using namespace CODES;
    connect(ui.panoramaPanningMethodButton, &QPushButton::clicked, [&]() {
        onPanoramaValueChanged(PANORAMA::Method);});
    connect(ui.panoramaPositionDial, &QDial::valueChanged, [&](int value) {
        onPanoramaValueChanged(PANORAMA::Position, value);});
}  

void EffectsTab::equalizerResetButtonClicked (const EqualizerPreset &preset) {
    ui.equalizer29HzSlider->setValue   (preset.slider29Hz);
    ui.equalizer59HzSlider->setValue   (preset.slider59Hz);
    ui.equalizer119HzSlider->setValue  (preset.slider119Hz);
    ui.equalizer237HzSlider->setValue  (preset.slider237Hz);
    ui.equalizer474HzSlider->setValue  (preset.slider474Hz);
    ui.equalizer1000HzSlider->setValue (preset.slider1kHz);
    ui.equalizer2000HzSlider->setValue (preset.slider2kHz);
    ui.equalizer4000HzSlider->setValue (preset.slider4kHz);
    ui.equalizer8000HzSlider->setValue (preset.slider8kHz);
    ui.equalizer16000HzSlider->setValue(preset.slider16kHz);
}

void EffectsTab::delayResetButtonClicked (const DelayPreset &preset) {
    ui.delayDelaySpinBox->setValue(preset.delay);
    if (ui.delaySurroundDelayButton->getState() != preset.surroundDelay)
        ui.delaySurroundDelayButton->click();
    ui.delayIntensityDial->setValue(preset.intensity);
    ui.delayMaxDelaySpinBox->setValue(preset.maxDelay);
    ui.delayFeedbackDial->setValue(preset.feedback);
}

void EffectsTab::filterResetButtonClicked (const FilterPreset &preset) {
    if (ui.filterModeToggleButton->getState() != preset.mode)
        ui.filterModeToggleButton->click();
    ui.filterCutoffSpinBox->setValue(preset.cutoff);
    ui.filterRippleSpinBox->setValue(preset.ripple);
    ui.filterPolesSpinBox->setValue(preset.poles);
    if (ui.filterFilterTypeButton->getState() != preset.filterType)
        ui.filterFilterTypeButton->click();
}

void EffectsTab::pitchResetButtonClicked (const PitchPreset &preset) {
    ui.pitchTempoDial->setValue(preset.tempo);
    ui.pitchPitchDial->setValue(preset.pitch);
    ui.pitchRateDial->setValue(preset.rate);
    ui.pitchOutputRateDial->setValue(preset.outputRate);
}  

void EffectsTab::compressorResetButtonClicked (const CompressorPreset &preset) {
    if (ui.compressorCompressorButton->getState() != preset.compressor)
        ui.compressorCompressorButton->click();
    if (ui.compressorKneeTypeButton->getState() != preset.kneeType)
        ui.compressorKneeTypeButton->click();
    ui.compressorRatioDial->setValue(preset.ratio);
    ui.compressorThresholdDial->setValue(preset.threshold);
}

void EffectsTab::onEqualizerValueChanged (CODES::EQUALIZER code, int value) {
    effects->changeEqualizerProps(code, value);
}

void EffectsTab::onDelayValueChanged (CODES::DELAY code, int value) {
    effects->changeDelayProps(code, value);
}

void EffectsTab::onFilterValueChanged (CODES::FILTER code, gfloat value) {
    effects->changeFilterProps(code, value);
}

void EffectsTab::onPitchValueChanged (CODES::PITCH code, int value) {
    effects->changePitchProps(code, value/10.0);
}

void EffectsTab::onCompressorValueChanged (CODES::COMPRESSOR code, int value) {
    effects->changeCompressorProps(code, value);
}

void EffectsTab::onPanoramaValueChanged (CODES::PANORAMA code, int value) {
    effects->changePanoramaProps(code, value);
}

#endif // _EFFECTSTAB_HPP_
