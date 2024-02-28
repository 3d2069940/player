
#ifndef _DIRECTORYLISTVIEWITEM_H_
#define _DIRECTORYLISTVIEWITEM_H_


#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <qcheckbox.h>

class DirectoryTree;

class DirectoryListViewItem : public QWidget {
    Q_OBJECT
public:
    explicit DirectoryListViewItem (const QString &folderName, Qt::CheckState state=Qt::Unchecked);
    virtual ~DirectoryListViewItem ();

//  setters
    void setCheckState ();
//  getters
    QString    getFolderName () const;
    QCheckBox *getCheckBox   ();
private:
    QLabel label;
    QCheckBox checkbox;
    QHBoxLayout layout;

};

#endif // _DIRECTORYLISTVIEWITEM_H_
