
#pragma once


#include "directoryTree.h"

DirectoryTree::DirectoryTree (DirectoryTree *_parent, QString dir) {
    currentDir.setPath(dir);
    parent = _parent;
}
    
DirectoryTree::~DirectoryTree () {
    foreach (const QString &key, children.keys())
        delete children.value(key);
}

void DirectoryTree::setDirs () {
    foreach (const QFileInfo &fileInfo, currentDir.entryInfoList(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot)) {
        auto child        = new DirectoryTree(this, fileInfo.absoluteFilePath());
        child->isSelected = this->isSelected;
        children.insert(fileInfo.fileName(), child);
    }
    childrenSet = true;
}

QString DirectoryTree::currentPath () {
    return currentDir.absolutePath();
}

DirectoryTree* DirectoryTree::root () {
    if (parent == nullptr)
        return this;
    return parent->root();
}

DirectoryTree* DirectoryTree::cdUp () {
    if (currentDir.isRoot())
        return this;
    
    QDir parentDir = currentDir;
    parentDir.cdUp();
    
    if (parent == nullptr)
        parent = new DirectoryTree(nullptr, parentDir.absolutePath());

    if (!childrenSet) {
        setDirs();
        childrenSet = true;
    }
    return parent;
}
    
DirectoryTree* DirectoryTree::cdDown (const QString& path) {
    auto child = children.value(path);
    if (!child->childrenSet)
        child->setDirs();
        
    return children.value(path);
}
    
QStringList DirectoryTree::listDirs () {
    QStringList dirs;
    foreach (const QString &key, children.keys())
        dirs << key;
    return dirs;
}
    
QStringList DirectoryTree::listSelectedDirs () {
    QStringList dirs;
    QString currentPath = currentDir.absolutePath() + "/";
    foreach (const QString &key, children.keys()) {
        DirectoryTree *value = children.value(key);
        if (value->isSelected)
            dirs << currentPath + key;
        else if (value->isChildSelected)
            dirs.append(value->listSelectedDirs());
    }
    return dirs;
}

void DirectoryTree::setChildrenState (bool state) {
    this->isSelected      = state;
    this->isChildSelected = state;
    foreach (const QString &key, children.keys()) {
        auto child = children.value(key);
        child->setChildrenState(this->isSelected);
    }
}

void DirectoryTree::setParentState () {
    int selectedChildren = 0;
    foreach (const QString &key, listDirs()) {
        auto child = children.value(key);
        if (child->isSelected) selectedChildren++;
        isChildSelected |= child->isChildSelected;
    }
    isSelected      = (selectedChildren == children.size());
    if (!currentDir.isRoot())
        parent->setParentState();
}






