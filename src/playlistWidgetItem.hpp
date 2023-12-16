
#pragma once

#include <string>

//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "playlistWidgetItem.h"


PlaylistWidgetItem::PlaylistWidgetItem (const QString &labelText, const QVariant &data, QWidget *parent)  
  : QWidget (parent), m_data(data) {
    layout = new QHBoxLayout ();
    button = new QPushButton ();
    label  = new QLabel(labelText);
    button->setFixedWidth(25);
    layout->addWidget(label);
    layout->addWidget(button);
    
    setLayout(layout);
}

PlaylistWidgetItem::~PlaylistWidgetItem () {
    delete label;
    delete button;
    delete layout;
}

std::string PlaylistWidgetItem::filePath () const {
    return m_data.toString().toStdString();
}
