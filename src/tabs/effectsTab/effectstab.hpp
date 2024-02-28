
#ifndef _EFFECTSTAB_HPP_
#define _EFFECTSTAB_HPP_


#include <QSlider>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <qimageiohandler.h>
#include <qpushbutton.h>

#include "effectstab.h"

#include "src/effects/effects.h"
#include "src/effects/effects.hpp"


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
