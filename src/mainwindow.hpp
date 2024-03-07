
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
#include "parser/parser.hpp"
#include "db/db.hpp"


#include "widgets/directoryListView/directorylistview.hpp"
#include <qscopedpointer.h>
#include <qstandardpaths.h>


MainWindowUI::MainWindowUI (QWidget *parent) 
    : QMainWindow(parent) {
    ui.setupUi(this);

    QString path = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0); 
    db.setPath(path.toStdString() + "/player/database.db");
    db.open();

    playerTab = QSharedPointer<PlayerTab>::create(&db);
    playerTab->setEffects(&effects);
    playerTab->setParser(&parser);
    ui.playerGridLayout->addWidget(playerTab.data());

    playlistTab = QSharedPointer<PlaylistTab>::create(&db);
    ui.playlistGridLayout->addWidget(playlistTab.data());

    effectsTab = QSharedPointer<EffectsTab>::create();
    effectsTab->setEffects(&effects);
    ui.effectsGridLayout->addWidget(effectsTab.data());

    visualizingTab = QSharedPointer<VisualizingTab>::create();
    visualizingTab->setEffects(&effects);
    ui.visualizingGridLayout->addWidget(visualizingTab.data());

    settingsTab = QSharedPointer<SettingsTab>::create();
    ui.settingsGridLayout->addWidget(settingsTab.data());
}

MainWindowUI::~MainWindowUI () {

}

#endif // _MAINWINDOW_HPP_
