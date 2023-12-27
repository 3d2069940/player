
#pragma once

#include "toggleButton.h"


ToggleButton::ToggleButton (QWidget *parent) : QPushButton(parent) {
    setFlat(true);
}

ToggleButton::~ToggleButton () {
    
}

void ToggleButton::setStates (QList<QIcon> states) {
    setFlat(true);
    statesList = states;
    if (statesList.size() > 0) {
        stateId = 0;
        setIcon(statesList.at(stateId));
    }
}

void ToggleButton::setStateId (int newState) {
    stateId = newState;
    setIcon(statesList.at(stateId));
}

void ToggleButton::mousePressEvent (QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        stateId++;
        if (stateId >= statesList.size())
            stateId = 0;
        setIcon(statesList.at(stateId));
    }
    QPushButton::mousePressEvent (event);
}
