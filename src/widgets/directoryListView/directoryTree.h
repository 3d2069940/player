
#ifndef _DIRECTORYTREE_H_
#define _DIRECTORYTREE_H_


#include <QDir>
#include <QMap>
#include <QString>


class DirectoryTree {

public:
    DirectoryTree  (DirectoryTree *_parent=nullptr, const QString &dir = QDir::rootPath());
    ~DirectoryTree ();

private:
    DirectoryTree *parent;
    QMap<QString, DirectoryTree*> children;

    size_t childrenSet = 0;

    bool isSelected = false;

    QDir currentDir;

    void setChildren ();

    DirectoryTree *root();
    DirectoryTree *cdUp();
    DirectoryTree *cdDown (const QString &folder);
};

#endif // _DIRECTORYTREE_H_
