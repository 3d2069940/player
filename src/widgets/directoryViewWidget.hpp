
#pragma once


#include <QStringList>

#include "directoryViewWidget.h"

#include "directoryViewWidgetTree.hpp"
#include "directoryViewElementWidget.hpp"


DirectoryViewWidget::DirectoryViewWidget (QListWidget *container, QWidget *parent) : QWidget(parent) {
    listWidget = container;
    tree       = new DirectoryViewWidgetTree (nullptr, QDir::rootPath());
    connect(listWidget, &QListWidget::itemClicked, this, &DirectoryViewWidget::backClicked);
}

DirectoryViewWidget::~DirectoryViewWidget () {
    tree = tree->root();
    delete tree;
}

void DirectoryViewWidget::setCurrentDirectory (const QString &path) {
    currentPath.setPath(path);
    tree->setDirs();
    foreach (const QString &dir, currentPath.absolutePath().split("/", Qt::SkipEmptyParts)) {
        tree = tree->cdDown(dir);
        tree->setDirs();
    }
    updateCurrentFolders ();
}

void DirectoryViewWidget::updateCurrentFolders () {
    listWidget->clear();
    
    QSize size = QSize(listWidget->width(), 40);
    
    if (!currentPath.isRoot()) {
        backItem = new QListWidgetItem ("Back");
        backItem->setSizeHint(size);
        listWidget->addItem(backItem);
    }
    for (const auto &[name, child] : tree->children) {
        Qt::CheckState state = child->isSelected ? Qt::Checked : 
                                child->isChildSelected ? Qt::PartiallyChecked : Qt::Unchecked;
                                
        QListWidgetItem *item              = new QListWidgetItem (listWidget);
        DirectoryViewElementWidget *widget = new DirectoryViewElementWidget (name, state);
        
        item->setSizeHint(size);
        listWidget->setItemWidget(item, widget);
        connect(widget, &DirectoryViewElementWidget::onCheckBoxClicked, this, &DirectoryViewWidget::selectFolderClicked);
        connect(widget, &DirectoryViewElementWidget::onLabelClicked,    this, &DirectoryViewWidget::nextFolderClicked);
    }
}

void DirectoryViewWidget::selectFolderClicked (int state, const QString& childName) {
    auto child = tree->children.at(childName);
    switch (state) {
        case 0:
            child->setSelectionState(false);
            break;
        case 1:
            break;
        case 2:
            child->setSelectionState(true);
            break;
    }
    QStringList selected = tree->root()->listSelectedDirs();
    emit newFolderSelected (selected);
}

void DirectoryViewWidget::nextFolderClicked (QString path) {
    if (currentPath.cd(path)) {
        tree = tree->cdDown(path);
        updateCurrentFolders();
    }
}

void DirectoryViewWidget::backClicked (QListWidgetItem *item) {
    if (listWidget->row(item) == 0 && currentPath.cdUp()) {
        tree = tree->cdUp();
        updateCurrentFolders();
  } else {
      
    } 
}




