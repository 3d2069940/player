
#pragma once

#include "toggleButton.h"

ToggleButton::ToggleButton (QWidget *parent) : QPushButton(parent) {
    connect(this, &QPushButton::clicked, this, &ToggleButton::buttonClicked);
}

ToggleButton::~ToggleButton () {

}

void ToggleButton::setLabels (QStringList &&_labels) {
    iconsSet = false;
    labels = qMove(_labels);
    setText(labels.at(stateId));
}

void ToggleButton::setIcons (QList<QIcon> &&_icons) {
    iconsSet = true;
    icons = qMove(_icons);
    setIcon(icons.at(stateId));
}

void ToggleButton::setState (int id) {
    stateId = id;
    if (iconsSet)
        setIcon(icons.at(stateId));
    else
        setText(labels.at(stateId));
}

int ToggleButton::getState () const {
    return stateId;
}

void ToggleButton::buttonClicked () {
    stateId++;
    if (iconsSet) {
        if (stateId >= icons.size())
            stateId = 0;
        setIcon(icons.at(stateId));
  } else {
        if (stateId >= labels.size())
            stateId = 0;
        setText(labels.at(stateId));
  }
}
