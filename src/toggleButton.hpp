
#pragma once

#include "toggleButton.h"

ToggleButton::ToggleButton (QList<QPair<QString, QIcon>> states) {
    setFlat(true);
    statesList = states;
    if (statesList.size() > 0) {
        stateId = 0;
        setText(statesList.at(stateId).first);
        setIcon(statesList.at(stateId).second);
    }
}

ToggleButton::~ToggleButton () {
    
}

void ToggleButton::mousePressEvent (QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        stateId++;
        if (stateId >= statesList.size())
            stateId = 0;
        setText(statesList.at(stateId).first);
        setIcon(statesList.at(stateId).second);
    }
    QPushButton::mousePressEvent (event);
}
