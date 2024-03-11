
#ifndef _TOGGLEBUTTON_HPP_
#define _TOGGLEBUTTON_HPP_


#include "togglebutton.h"

#include <QDebug>
#include <qevent.h>
#include <qnamespace.h>
#include <QPushButton>


ToggleButton::ToggleButton (QWidget *parent) : QPushButton(parent) {
    connect(this, &QPushButton::clicked, this, &ToggleButton::onButtonClicked);
}

ToggleButton::~ToggleButton () {

}

void ToggleButton::setLabels (const QStringList &_labels) {
    iconsSet = false;
    labels = qMove(_labels);
    setText(labels.at(stateId));
}

void ToggleButton::setIcons (const QList<QIcon> &_icons) {
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

void ToggleButton::onButtonClicked () {
    if (iconsSet) {
        ++stateId %= icons.size();
        setIcon(icons.at(stateId));
    } else {
        ++stateId %= labels.size();
        setText(labels.at(stateId));
    }
}



#endif // _TOGGLEBUTTON_HPP_
