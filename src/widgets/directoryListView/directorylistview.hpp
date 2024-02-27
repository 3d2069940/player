
#ifndef _DIRECTORYLISTVIEW_HPP_
#define _DIRECTORYLISTVIEW_HPP_


#include "directorylistview.h"

#include "directoryTree.hpp"
#include <qevent.h>


DirectoryListView::DirectoryListView (QWidget *parent) 
    : QListWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    tree = new DirectoryTree (nullptr);
}

DirectoryListView::~DirectoryListView () {

}

void DirectoryListView::markSelectedDirectories (const QStringList &directories) {

}

void DirectoryListView::update () {

}

void DirectoryListView::wheelEvent (QWheelEvent *event) {

}

void DirectoryListView::keyPressEvent (QKeyEvent *event) {

} 

void DirectoryListView::mouseDoubleClickEvent (QMouseEvent *event) {

}

void DirectoryListView::focusOutEvent (QFocusEvent *event) {
    
}

#endif // _DIRECTORYLISTVIEW_HPP_
