
#pragma once


#include "player.h"

#include "widgets/player/toggleButtonWidget.hpp"
#include "widgets/effects/presetDialogWindow.h"


class Connector {

public:
    static void connectTabs              (MainWindowUI* ref);
    static void connectPlayerTab         (MainWindowUI* ref);
      static void connectEqualizerTab      (MainWindowUI* ref);
      static void connectEffectWidgets     (MainWindowUI* ref);
      static void connectDelayWidgets      (MainWindowUI *ref);
      static void connectFilterWidgets     (MainWindowUI* ref);
      static void connectPitchWidgets      (MainWindowUI* ref);
      static void connectCompressorWidgets (MainWindowUI* ref);
      static void connectPanoramaWidgets   (MainWindowUI* ref);
    static void connectVisualizingTab    (MainWindowUI* ref);
    static void connectSettingsTab       (MainWindowUI* ref);
    
private:
    
};

void Connector::connectTabs (MainWindowUI* ref) {
    connectPlayerTab(ref);
    
    ref->connect(&ref->audioPositionTimer,   &QTimer::timeout, ref, &MainWindowUI::updateAudioState);
    ref->connect(&ref->audioPanningTimer,    &QTimer::timeout, ref, &MainWindowUI::updatePanPosition);
    ref->connect(&ref->audioVisualizingTime, &QTimer::timeout, ref, &MainWindowUI::updateVisualizingWidget);
    
    ref->connect(ref->presetDialogWindow->dialogButtonBox, &QDialogButtonBox::accepted, ref, &MainWindowUI::addNewPreset);
    
    ref->connect(ref->presetDialogWindow->dialogButtonBox, &QDialogButtonBox::rejected, [=]() {
        ref->presetDialogWindow->dialogLineEdit->clear();
        ref->presetDialogWindow->hide();
    });
}

void Connector::connectPlayerTab (MainWindowUI* ref) {
    connectEqualizerTab(ref);
    connectEffectWidgets(ref);
    connectDelayWidgets(ref);
    connectFilterWidgets(ref);
    connectPitchWidgets(ref);
    connectCompressorWidgets(ref);
    connectPanoramaWidgets(ref);
        
    ref->connect(ref->ui.playerPlayListWidget, &QListWidget::itemClicked, ref, &MainWindowUI::playlistItemClicked);
    
    ref->connect(ref->ui.playerPlaylistVisibilityButton, &QPushButton::clicked, ref, &MainWindowUI::togglePlaylistView);
    
    ref->connect(ref->ui.playerTagListVisibilityButton, &QPushButton::clicked, ref, &MainWindowUI::closeTagListWidget);
    
    ref->connect(ref->ui.playerSeekSlider, &QSlider::sliderPressed, &ref->audioPositionTimer, &QTimer::stop);
    
    ref->connect(ref->ui.playerSeekSlider, &QSlider::valueChanged, [ref] (int value) {
        ref->updateAudioPositionLabel(ref->effects->audioDuration*value/1000, ref->effects->audioDuration);
    });
    ref->connect(ref->ui.playerSeekSlider, &QSlider::sliderReleased, [ref]() {
        ref->effects->updateAudioDuration();
        int value = ref->ui.playerSeekSlider->value(), 
            maxValue = ref->ui.playerSeekSlider->maximum();
        gint64 currentPosition = ref->effects->audioDuration * value / maxValue;
        ref->effects->seekPlayingPosition(currentPosition);
        ref->audioPositionTimer.start();
    });
    ref->connect(ref->previousButton.data(), &QPushButton::clicked, ref, &MainWindowUI::previousButtonClicked);
    ref->connect(ref->repeatButton  .data(), &QPushButton::clicked, ref, &MainWindowUI::repeatButtonClicked);
    ref->connect(ref->pauseButton   .data(), &QPushButton::clicked, ref, &MainWindowUI::pauseButtonClicked);
    ref->connect(ref->shuffleButton .data(), &QPushButton::clicked, ref, &MainWindowUI::shuffleButtonClicked);
    ref->connect(ref->nextButton    .data(), &QPushButton::clicked, ref, &MainWindowUI::nextButtonClicked);
}

void Connector::connectEqualizerTab (MainWindowUI *ref) {
    
}

void Connector::connectEffectWidgets (MainWindowUI *ref) {
    
}

void Connector::connectDelayWidgets (MainWindowUI *ref) {
    
}

void Connector::connectFilterWidgets (MainWindowUI *ref) {
    
}

void Connector::connectPitchWidgets (MainWindowUI *ref) {
    
}

void Connector::connectCompressorWidgets (MainWindowUI *ref) {
    
}

void Connector::connectPanoramaWidgets (MainWindowUI *ref) {
    
}

void Connector::connectVisualizingTab (MainWindowUI *ref) {
    
}
void Connector::connectSettingsTab (MainWindowUI *ref) {
    
}
