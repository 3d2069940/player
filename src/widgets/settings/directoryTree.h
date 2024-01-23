
#ifndef DIRECTORY_TREE_H
#define DIRECTORY_TREE_H

#include <QMap>
#include <QDir>
#include <QString>
#include <QStringList>

class DirectoryTree {
    
public:
    DirectoryTree (DirectoryTree *_parent = nullptr, QString dir = QDir::rootPath());
    ~DirectoryTree ();

    void setDirs ();
    
    QString currentPath ();
    
    DirectoryTree *root ();
    DirectoryTree *cdUp ();
    DirectoryTree *cdDown (const QString&);
    
    QStringList listDirs ();
    QStringList listSelectedDirs ();
    
    void setChildrenState (bool state = true);
    void setParentState   ();
    
    bool childrenSet     = false,
         isSelected      = false, 
         isChildSelected = false;
         
    DirectoryTree *parent;
    QMap<QString, DirectoryTree*> children;
    
    QDir currentDir;
};

#endif // DIRECTORY_TREE_H

