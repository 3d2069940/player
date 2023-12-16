
#pragma once

#include "playlistWidgetItem.h"


PlaylistWidgetItem::PlaylistWidgetItem (const QString &labelText, const QVariant &data, QWidget *parent)  
  : QWidget (parent), m_data(data) {
    layout = new QHBoxLayout ();
    label  = new QLabel(labelText);
    button = new QPushButton ();
    button->setFixedWidth(25);
    layout->addWidget(label);
    layout->addWidget(button);
    
    setLayout(layout);
}

PlaylistWidgetItem::~PlaylistWidgetItem () {
    delete button;
    delete label;
    delete layout;
}

QString PlaylistWidgetItem::filePath () const {
    return m_data.toString();
}
