
#pragma once

#include "directoryViewList.h"


DirectoryViewList::DirectoryViewList (QWidget *parent) : QListWidget(parent) {
    
}

DirectoryViewList::~DirectoryViewList () {
    
}

void DirectoryViewList::wheelEvent (QWheelEvent *event) {
    event->accept();
    QListWidget::wheelEvent(event); 
}

void DirectoryViewList::keyPressEvent (QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Space:
            break;
        case Qt::Key_Enter: case Qt::Key_Return:
            break;
        default:
            break;
    }
    event->accept();
}
