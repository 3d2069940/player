
#pragma once

#include "presetDialogWindow.h"


PresetDialogWindow::PresetDialogWindow(QWidget* parent, Qt::WindowFlags f) 
  : QDialog (parent, f), dialogNameLabel(QStringLiteral("Preset Name:")) {
    setup();
}

PresetDialogWindow::~PresetDialogWindow () {

}

void PresetDialogWindow::setup() {
    dialogVLayout      = new QVBoxLayout (this);
    dialogNameLayout   = new QHBoxLayout ();
    dialogButtonLayout = new QHBoxLayout ();
    dialogLineEdit     = new QLineEdit   ();
    dialogButtonBox    = new QDialogButtonBox (QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    
    dialogNameLayout->  addWidget(&dialogNameLabel);
    dialogNameLayout->  addWidget(dialogLineEdit);
    dialogButtonLayout->addWidget(dialogButtonBox);

    dialogVLayout->     addLayout(dialogNameLayout);
    dialogVLayout->     addLayout(dialogButtonLayout);

    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &PresetDialogWindow::rejectedClicked);
}

std::string PresetDialogWindow::getLineInput () {
    return dialogLineEdit->text().toStdString();
}

void PresetDialogWindow::rejectedClicked () {
    dialogLineEdit->clear();
    hide();
}

void PresetDialogWindow::show () {
//     QObject *senderObj = sender();
//     if (senderObj != nullptr) {
//
//     }
    QDialog::show();
}
