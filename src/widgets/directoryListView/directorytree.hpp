
#ifndef _DIRECTORYTREE_HPP_
#define _DIRECTORYTREE_HPP_


#include "directorytree.h"


DirectoryTree::DirectoryTree (DirectoryTree *_parent, const QString &path)
    : currentDir(path), parent(_parent) {
    
}

DirectoryTree::~DirectoryTree() {
    foreach (const QString &key, children.keys())
        delete children.value(key);
}

DirectoryTree *DirectoryTree::root () {
    auto node = this;
    while (node->parent != nullptr)
        node = node->parent;
    return node;
}

DirectoryTree *DirectoryTree::cdUp () {
    if (parent == nullptr)
        return this;
    return parent;
}
/**

*/
DirectoryTree *DirectoryTree::cdDown (const QString &key) {
    auto node = children.value(key);
    if (!node->childrenCreated)
        node->createChildren();
    return node;
}

void DirectoryTree::createChildren () {
    QDir::Filters filter = QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot;
    foreach (const QFileInfo &fileInfo, currentDir.entryInfoList(filter)) {
        auto child        = new DirectoryTree(this, fileInfo.absoluteFilePath());
        children.insert(fileInfo.fileName(), child);
    }
    childrenCreated = true;
}

QStringList DirectoryTree::getCurrentDirs () {
    QStringList dirs;
    foreach (const QString &key, children.keys())
        dirs << key;
    return dirs;
}

DirectoryTree *DirectoryTree::getChild (const QString &key) const {
    return children.value(key);
}

bool DirectoryTree::isRootDir () const {
    return currentDir.isRoot();
}

bool DirectoryTree::isSelected () const {
    return selected;
}

bool DirectoryTree::isAnyChildSelected () const {
    return childSelectedCounter > 0;
}

#endif // _DIRECTORYTREE_HPP_
