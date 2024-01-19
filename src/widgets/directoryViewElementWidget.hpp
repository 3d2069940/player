
#pragma once

#include <QCheckBox>
#include <QSignalBlocker>

#include "directoryViewElementWidget.h"


DirectoryViewElementWidget::DirectoryViewElementWidget (const QString& absolutePath, Qt::CheckState state) {
    layout   = new QHBoxLayout ();
    checkbox = new QCheckBox   ();
    filePath = new QLabel      (absolutePath);
    
    checkbox->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Preferred);
    checkbox->setTristate   (true);
    checkbox->setCheckState (state);
    
    layout->addWidget(checkbox);
    layout->addWidget(filePath);
    setLayout(layout);
    
    connect(checkbox, &QCheckBox::stateChanged, this, &DirectoryViewElementWidget::onCheckBoxClickedWrapper);   
}

DirectoryViewElementWidget::~DirectoryViewElementWidget () {
        
}

void DirectoryViewElementWidget::mouseDoubleClickEvent (QMouseEvent * event) {
    if (event->button() == Qt::LeftButton) {
        emit onLabelClicked(filePath->text());
    }
    QWidget::mouseDoubleClickEvent(event);
}
    
void DirectoryViewElementWidget::onCheckBoxClickedWrapper (int state) {
    if (state > 0) {
        QSignalBlocker blocker (checkbox);
        checkbox->setCheckState(Qt::Checked);
        state = 2;
    }
    emit onCheckBoxClicked(state, filePath->text());
}
