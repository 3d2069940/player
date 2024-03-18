
#ifndef _SETTINGSTAB_HPP_
#define _SETTINGSTAB_HPP_


#include "settingstab.h"


SettingsTab::SettingsTab (QWidget *parent) 
    : QWidget(parent) {
    ui.setupUi(this);
    createWidgets();
    setupWidgets();
    connectWidgets();
}

void SettingsTab::createWidgets () {

}

void SettingsTab::setupWidgets () {
    ui.settingsDirectoryListView->hide();
}

void SettingsTab::connectWidgets () {
    connect(ui.settingsMediaFoldersButton, &QPushButton::clicked, this, &SettingsTab::onSelectFoldersButtonClicked);
}

void SettingsTab::onSelectFoldersButtonClicked () {
    ui.settingsDirectoryListView->setVisible(
        !ui.settingsDirectoryListView->isVisible());
}

#endif // _SETTINGSTAB_HPP_
