
#pragma once


#include <QCheckBox>
#include <QSignalBlocker>

#include "directoryListWidget.h"

class QLabel;
class QCheckBox;

DirectoryListWidget::DirectoryListWidget (const QString &text, Qt::CheckState state) {
    label    = new QLabel      (text);
    checkbox = new QCheckBox   ();
    layout   = new QHBoxLayout ();
    
    checkbox->setTristate(true);
    checkbox->setCheckState(state);
    checkbox->setFocusPolicy(Qt::NoFocus);
    
    layout->addWidget(checkbox);
    layout->addWidget(label);
    
    setLayout(layout);

    connect(checkbox, &QCheckBox::stateChanged, this, &DirectoryListWidget::onCheckBoxClickedWrapper);
}

DirectoryListWidget::~DirectoryListWidget () {
    
}

void DirectoryListWidget::spacePressed () {
    QSignalBlocker blocker (checkbox);
    switch (checkbox->checkState()) {
        case Qt::Unchecked:
        case Qt::PartiallyChecked:
            checkbox->setCheckState(Qt::Checked);
            break;
        case Qt::Checked:
            checkbox->setCheckState(Qt::Unchecked);
            break;
    }
}

QString DirectoryListWidget::dirName () {
    return label->text();
}

void DirectoryListWidget::onCheckBoxClickedWrapper (int state) {
    QSignalBlocker blocker (checkbox);
    switch (state) {
        case Qt::PartiallyChecked:
            checkbox->setCheckState(Qt::Checked);
        default:
            break;
    }
    emit onCheckBoxClicked(label->text());
}
