
#ifndef _DIRECTORYLISTVIEW_HPP_
#define _DIRECTORYLISTVIEW_HPP_


#include <QEvent>
#include <QCheckBox>
#include <QScrollBar>
#include <QWheelEvent>
#include <QSignalBlocker>


#include <QDebug>
#include <qnamespace.h>


#include "directorytree.h"
#include "directorylistview.h"
#include "directorylistviewitem.h"

#include "directorytree.hpp"
#include "directorylistviewitem.hpp"


DirectoryListView::DirectoryListView (QWidget *parent) 
    : QListWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    tree = new DirectoryTree (nullptr);
    tree->createChildren();
    foreach (const QString &dir, QDir::homePath().split("/",Qt::SkipEmptyParts))
        tree = tree->cdDown(dir);
    update();
}

DirectoryListView::~DirectoryListView () {
    tree = tree->root();
    delete tree;
}

void DirectoryListView::update () {
    clear ();
    QSize size (width(), 40);

    if (!tree->isRootDir()) {
        QListWidgetItem *back = new QListWidgetItem ("Back");
        back->setSizeHint(size);
        addItem(back);
    }

    foreach (const QString &key, tree->getCurrentDirs()) {
        auto child = tree->getChild(key);
        Qt::CheckState state = (child->isSelected()         ? Qt::Checked : 
                                child->isAnyChildSelected() ? Qt::PartiallyChecked :
                                                              Qt::Unchecked);

        auto item = new QListWidgetItem (this);
        auto itemWidget = new DirectoryListViewItem (key, state);
                                            
        item->setSizeHint(itemWidget->sizeHint());
        setItemWidget(item, itemWidget);

        connect(itemWidget->getCheckBox(), &QCheckBox::stateChanged, this, &DirectoryListView::onCheckBoxClicked);
    }
    
    setCurrentRow(0);
}

void DirectoryListView::onCheckBoxClicked (int state) {
    Q_UNUSED(state);
}

void DirectoryListView::wheelEvent (QWheelEvent *event) {
    int val = verticalScrollBar()->value(), 
        max = verticalScrollBar()->maximum(), 
        min = verticalScrollBar()->minimum();
    
    if ((val == min && event->angleDelta().y() > 0) || (val == max && event->angleDelta().y() < 0))
        event->accept();
    else 
        QListWidget::wheelEvent(event);
}

void DirectoryListView::keyPressEvent (QKeyEvent *event) {
    if (count() == 0)
        return;
    int row = currentRow();
    if (event->key() == Qt::Key_Space) {
//      pressing the space bar on the "back" item
        if (row == 0 && !tree->isRootDir()) 
            return;
        auto currentItem = qobject_cast<DirectoryListViewItem*>(itemWidget(item(row)));
        currentItem->setCheckState();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (row == 0 && !tree->isRootDir())
            tree = tree->cdUp();
        else {
            auto currentItem = qobject_cast<DirectoryListViewItem*>(itemWidget(item(row)));
            tree = tree->cdDown(currentItem->getFolderName());
        }
        update();
    }
    QListWidget::keyPressEvent(event);
} 

void DirectoryListView::mouseDoubleClickEvent (QMouseEvent *event) {
    int row = currentRow();
    if (event->button() == Qt::LeftButton) {
//      pressing the space bar on the "back" item
        if (row == 0 && !tree->isRootDir())
            tree = tree->cdUp();
//      descent to a pressed directory
        else {
            auto currentItem = qobject_cast<DirectoryListViewItem*>(itemWidget(item(row)));
            tree = tree->cdDown(currentItem->getFolderName());
        }
        update();
    }
    QListWidget::mouseDoubleClickEvent(event);
}

void DirectoryListView::focusOutEvent (QFocusEvent *) {
    
}



#endif // _DIRECTORYLISTVIEW_HPP_
