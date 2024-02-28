
#ifndef _EFFECTSTAB_HPP_
#define _EFFECTSTAB_HPP_

#include <functional>
#include <qslider.h>
#include <qspinbox.h>

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
    connect(ui.equalizer29HzSlider, &QSlider::valueChanged, [&](int value){
        onEqualizerValueChanged(CODES::EQUALIZER::Slider29Hz, value);});
    connect(ui.equalizer59HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider59Hz, value);});
    connect(ui.equalizer119HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider119Hz, value);});
    connect(ui.equalizer237HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider237Hz, value);});
    connect(ui.equalizer474HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider474Hz, value);});
    connect(ui.equalizer1000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider1kHz, value);});
    connect(ui.equalizer2000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider2kHz, value);});
    connect(ui.equalizer4000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider4kHz, value);});
    connect(ui.equalizer8000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider8kHz, value);});
    connect(ui.equalizer16000HzSlider, &QSlider::valueChanged, [&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Slider16kHz, value);});
    connect(ui.equalizerVolumeSlider,&QSlider::valueChanged,[&](int value) {
        onEqualizerValueChanged(CODES::EQUALIZER::Volume, value);});
}

void EffectsTab::connectDelay () {
    using namespace CODES;
    connect(ui.delayDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onDelayValueChanged(CODES::DELAY::Delay, value);});
    connect(ui.delaySurroundDelayButton, &QPushButton::clicked, [&]() {
        onDelayValueChanged(CODES::DELAY::SurroundDelay);});
    connect(ui.delayIntensityDial, &QDial::valueChanged, [&](int value) {
        onDelayValueChanged(CODES::DELAY::Intensity, value);});
    connect(ui.delayMaxDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        onDelayValueChanged(CODES::DELAY::MaxDelay, value);});
    connect(ui.delayFeedbackDial, &QDial::valueChanged, [&](int value) {
        onDelayValueChanged(CODES::DELAY::Feedback, value);});
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

}

void EffectsTab::connectCompressor () {

}

void EffectsTab::connectPanorama () {

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

#endif // _EFFECTSTAB_HPP_
