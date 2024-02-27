
#ifndef _TOGGLEBUTTON_HPP_
#define _TOGGLEBUTTON_HPP_


#include "togglebutton.h"
#include <qevent.h>
#include <qnamespace.h>
#include <qpushbutton.h>


ToggleButton::ToggleButton (QWidget *parent) : QPushButton(parent) {

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

void ToggleButton::mousePressEvent (QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (iconsSet) {
            ++stateId %= icons.size();
            setIcon(icons.at(stateId));
        } else {
            ++stateId %= labels.size();
            setText(labels.at(stateId));
        }
  } QPushButton::mousePressEvent(event);
}



#endif // _TOGGLEBUTTON_HPP_
