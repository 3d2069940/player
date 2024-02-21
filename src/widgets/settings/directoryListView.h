
#ifndef DIRECTORY_VIEW_LIST_H
#define DIRECTORY_VIEW_LIST_H

#include <QWidget>
#include <QString>
#include <QKeyEvent>
#include <QStringList>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QListWidget>
#include <qevent.h>


class DirectoryTree;
class DirectoryListWidget;

class DirectoryListView : public QListWidget {
    Q_OBJECT
    
public:
    explicit DirectoryListView (QWidget *parent = nullptr);
    virtual ~DirectoryListView ();
    
    void markSelectedDirs (const QStringList& dirs);

private:
    DirectoryTree *tree;
    void updateList       ();
    
protected:
    void wheelEvent            (QWheelEvent *event) override;
    void keyPressEvent         (QKeyEvent   *event) override;
    void mouseDoubleClickEvent (QMouseEvent *event) override;
    void focusOutEvent         (QFocusEvent *event) override;

public slots:
    void onCheckBoxClicked (const QString&);
    
signals:
    void selectedFolderUpdated (const QStringList&);

};

#endif // DIRECTORY_VIEW_LIST_H
