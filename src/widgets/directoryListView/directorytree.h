
#ifndef _DIRECTORYTREE_H_
#define _DIRECTORYTREE_H_


#include <QDir>
#include <QMap>


class DirectoryTree {

public:
    explicit DirectoryTree (DirectoryTree *_parent = nullptr, const QString &path = QDir::rootPath());
    ~DirectoryTree ();

    DirectoryTree *root   ();
    DirectoryTree *cdUp   ();
    DirectoryTree *cdDown (const QString &key);

    void createChildren ();

//  getters
    QStringList getCurrentDirs ();

    DirectoryTree *getChild (const QString &key) const;

    bool isRootDir () const;

    bool isSelected () const;
    bool isAnyChildSelected () const;
private:
    QDir currentDir;

    DirectoryTree *parent = nullptr;

    QMap<QString, DirectoryTree*> children;
    
    bool childrenCreated = false,
         selected        = false;

    int childSelectedCounter = 0;
};

#endif // _DIRECTORYTREE_H_
