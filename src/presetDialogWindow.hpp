
#pragma once

#include "presetDialogWindow.h"


PresetDialogWindow::PresetDialogWindow(QWidget* parent, Qt::WindowFlags f) 
  : QDialog (parent, f), dialogNameLabel("Preset Name:") {
    setup();
}

PresetDialogWindow::~PresetDialogWindow() {
     delete dialogLineEdit;
     delete dialogButtonBox;
     delete dialogNameLayout;
     delete dialogButtonLayout;
     delete dialogVLayout;
}

void PresetDialogWindow::setup() {
    dialogVLayout      = new QVBoxLayout (this);
    dialogNameLayout   = new QHBoxLayout ();
    dialogButtonLayout = new QHBoxLayout ();
    dialogLineEdit     = new QLineEdit ();
    dialogButtonBox    = new QDialogButtonBox (QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    
    dialogNameLayout->addWidget(&dialogNameLabel);
    dialogNameLayout->addWidget(dialogLineEdit);
    dialogButtonLayout->addWidget(dialogButtonBox);
    dialogVLayout->addLayout(dialogNameLayout);
    dialogVLayout->addLayout(dialogButtonLayout);
}

std::string PresetDialogWindow::getLineInput () {
    return dialogLineEdit->text().toStdString();
}
