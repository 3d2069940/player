
#ifndef _MAINWINDOW_HPP_
#define _MAINWINDOW_HPP_

#include "mainwindow.h"


#include "src/tabs/settingsTab/settingstab.h"
#include "tabs/playerTab/playertab.hpp"
#include "tabs/playlistTab/playlisttab.hpp"
#include "tabs/effectsTab/effectstab.hpp"
#include "tabs/visualizingTab/visualizingtab.hpp"
#include "tabs/settingsTab/settingstab.hpp" 

#include "effects/effects.hpp"


#include "widgets/directoryListView/directorylistview.hpp"


MainWindowUI::MainWindowUI (QWidget *parent) 
    : QMainWindow(parent) {
    ui.setupUi(this);

    playerTab = QSharedPointer<PlayerTab>::create();
    playerTab->setEffects(&effects);
    ui.playerGridLayout->addWidget(playerTab.data());


    playlistTab = QSharedPointer<PlaylistTab>::create();
    ui.playlistGridLayout->addWidget(playlistTab.data());

    effectsTab = QSharedPointer<EffectsTab>::create();
    ui.effectsGridLayout->addWidget(effectsTab.data());

    visualizingTab = QSharedPointer<VisualizingTab>::create();
    ui.visualizingGridLayout->addWidget(visualizingTab.data());

    settingsTab = QSharedPointer<SettingsTab>::create();
    ui.settingsGridLayout->addWidget(settingsTab.data());
}

MainWindowUI::~MainWindowUI () {

}

#endif // _MAINWINDOW_HPP_
