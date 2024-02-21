
#pragma once

#include <QScrollBar>
#include <qevent.h>

#include "directoryListView.h"

#include "directoryTree.hpp"
#include "directoryListWidget.hpp"



DirectoryListView::DirectoryListView (QWidget *parent) : QListWidget(parent) {
        
    setFocusPolicy(Qt::StrongFocus);
        
    tree = new DirectoryTree (nullptr, QDir::rootPath());
    tree->setDirs();
    
    foreach (const QString &dir, QDir::homePath().split("/", Qt::SkipEmptyParts))
        tree = tree->cdDown(dir);
    updateList ();
}

DirectoryListView::~DirectoryListView () {
    tree = tree->root();
    delete tree;
}

void DirectoryListView::markSelectedDirs (const QStringList& paths) {
    foreach (const QString &path, paths) {
        auto tree_ptr = tree->root();
        foreach (const QString &dir, path.split("/", Qt::SkipEmptyParts)) {
            tree_ptr->isChildSelected = true;
            tree_ptr = tree_ptr->cdDown(dir);
        }
        tree_ptr->isSelected = true;
    }
    updateList();
}

void DirectoryListView::updateList () {
    clear();
    
    QSize size = QSize (width(), 40);
    
    if (tree->currentPath() != QDir::rootPath()) {
        QListWidgetItem *back = new QListWidgetItem ("Back");
        back->setSizeHint(size);
        addItem(back);
    }
    foreach (const QString &key, tree->listDirs()) {
        const DirectoryTree *value = tree->children.value(key);
        Qt::CheckState state = value->isSelected ? Qt::Checked :
                                value->isChildSelected ? Qt::PartiallyChecked :
                                    Qt::Unchecked;
        QListWidgetItem     *item   = new QListWidgetItem ();
        DirectoryListWidget *widget = new DirectoryListWidget (key, state);
        
        item->setSizeHint(widget->sizeHint());
        
        addItem(item);
        setItemWidget(item, widget);
        
        connect(widget, &DirectoryListWidget::onCheckBoxClicked, this, &DirectoryListView::onCheckBoxClicked);
    }
    setCurrentRow(0);
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
    DirectoryListWidget *widget;
    DirectoryTree *child;
    bool selected;
    switch (event->key()) {
        case Qt::Key_Space:
            if (row == 0 && tree->currentPath() != QDir::rootPath())
                return; 
            widget = qobject_cast<DirectoryListWidget*>(itemWidget(item(row)));
            widget->spacePressed();
            child = tree->children.value(widget->dirName());
            child->setChildrenState(!child->isSelected);
            tree->setParentState   ();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (row == 0 && tree->currentPath() != QDir::rootPath())
                tree = tree->cdUp();
            else {
                widget           = qobject_cast<DirectoryListWidget*>
                                                (itemWidget(item(row)));
                selected         = tree->isSelected;
                tree             = tree->cdDown(widget->dirName());
                tree->isSelected = selected;
            }
            updateList();
            setCurrentRow(0);
            break;
        default:
            break;
    }
    QListWidget::keyPressEvent(event);
}

void DirectoryListView::mouseDoubleClickEvent (QMouseEvent *event) {
    int row = currentRow();
    DirectoryListWidget *widget;
    if (event->button() == Qt::LeftButton) {
        if (row == 0 && tree->currentPath() != QDir::rootPath())
            tree = tree->cdUp();
        else {
            widget = qobject_cast<DirectoryListWidget*>(itemWidget(item(row)));
            tree   = tree->cdDown(widget->dirName());
        }
        updateList();
    }
    QListWidget::mouseDoubleClickEvent(event);
}

void DirectoryListView::focusOutEvent (QFocusEvent *event) {
    emit selectedFolderUpdated(tree->root()->listSelectedDirs());
    QListWidget::focusOutEvent(event);
}

void DirectoryListView::onCheckBoxClicked (const QString& dirName) {
    auto child = tree->children.value(dirName);
    child->setChildrenState(!child->isSelected);
    tree->setParentState();
}
