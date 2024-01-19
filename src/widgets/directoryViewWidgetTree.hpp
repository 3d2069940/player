
#pragma once

#include <algorithm>

#include "directoryViewWidgetTree.h"


DirectoryViewWidgetTree::DirectoryViewWidgetTree (DirectoryViewWidgetTree *_parent, QString dir) {
    currentDir.setPath(dir);
    parent = _parent;
}
    
DirectoryViewWidgetTree::~DirectoryViewWidgetTree () {
    for (auto it = children.begin(); it != children.end(); ++it)
        delete it->second;
}

void DirectoryViewWidgetTree::setDirs () {
    foreach (const QFileInfo &fileInfo, currentDir.entryInfoList(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot)) {
        auto child        = new DirectoryViewWidgetTree(this, fileInfo.absoluteFilePath());
        child->isSelected = this->isSelected;
        children.insert({fileInfo.fileName(), child});
    }
}

DirectoryViewWidgetTree* DirectoryViewWidgetTree::root () {
    if (parent == nullptr)
        return this;
    return parent->root();
}

DirectoryViewWidgetTree* DirectoryViewWidgetTree::cdUp () {
    if (currentDir.isRoot()) {
        return this;
    }
    
    QDir parentDir = currentDir;
    parentDir.cdUp();
    
    if (parent == nullptr)
        parent = new DirectoryViewWidgetTree(nullptr, parentDir.absolutePath());

    if (!childrenSet) {
        setDirs();
        childrenSet = true;
    }
    return parent;
}
    
DirectoryViewWidgetTree* DirectoryViewWidgetTree::cdDown (const QString& path) {
    DirectoryViewWidgetTree* child = children.at(path);
    if (!child->childrenSet) {
        child->setDirs();
        child->childrenSet = true;
    }
    return child;
}
    
QStringList DirectoryViewWidgetTree::listDirs () {
    QStringList dirs;
    for (const auto &[key, value] : children)
        dirs << key;
    return dirs;
}
    
QStringList DirectoryViewWidgetTree::listSelectedDirs () {
    QStringList dirs;
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (it->second->isSelected)
            dirs << it->second->currentDir.absolutePath();
        else if (it->second->isChildSelected)
            dirs.append(it->second->listSelectedDirs());
    }
    return dirs;
}

/**
 * Marks the descendants of the node as selected, and the ancestors as that some descendant was selected 
 * @param selected Set state for the node
 * @param direction Set traversal direction true - to ancestors false - to descendants
 */
void DirectoryViewWidgetTree::setSelectionState (bool selected, bool direction) {
    isSelected = isChildSelected = selected;
    for (auto &[key, child] : children)
        child->setSelectionState(selected, false);

    if (parent != nullptr && direction)
        parent->setChildSelectionState();
}

void DirectoryViewWidgetTree::setChildSelectionState () {
    size_t checkedChildren = 0, 
           partiallyCheckedChildren = 0;
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (it->second->isSelected) checkedChildren++;
        if (it->second->isChildSelected) partiallyCheckedChildren++;
    }

    isChildSelected = (checkedChildren != 0 || partiallyCheckedChildren != 0);
    isSelected      = (checkedChildren == children.size());
    
    if (parent != nullptr)
        parent->setChildSelectionState();
}
