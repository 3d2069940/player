
#ifndef DIRECTORY_VIEW_WIDGET_TREE_H
#define DIRECTORY_VIEW_WIDGET_TREE_H

#include <map>

#include <QDir>
#include <QString>
#include <QStringList>

class DirectoryViewWidgetTree {
    
public:
    DirectoryViewWidgetTree (DirectoryViewWidgetTree *_parent = nullptr, QString dir = QDir::rootPath());
    ~DirectoryViewWidgetTree ();

    void setDirs ();
    
    DirectoryViewWidgetTree *root ();
    DirectoryViewWidgetTree *cdUp ();
    DirectoryViewWidgetTree *cdDown (const QString&);
    
    QStringList listDirs ();
    QStringList listSelectedDirs ();
    
    void setSelectionState      (bool selected = true, bool direction = true);
    void setChildSelectionState ();
    
    bool childrenSet     = false,
         isSelected      = false, 
         isChildSelected = false;
         
    DirectoryViewWidgetTree *parent;
    std::unordered_map<QString, DirectoryViewWidgetTree*> children;
    
    QDir currentDir;
};

#endif // DIRECTORY_VIEW_WIDGET_TREE_H

