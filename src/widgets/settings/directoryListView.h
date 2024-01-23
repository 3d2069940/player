
#ifndef DIRECTORY_VIEW_LIST_H
#define DIRECTORY_VIEW_LIST_H

#include <QWidget>
#include <QString>
#include <QKeyEvent>
#include <QStringList>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QListWidget>


class DirectoryTree;
class DirectoryListWidget;

class DirectoryListView : public QListWidget {
    Q_OBJECT
    
public:
    explicit DirectoryListView (QWidget *parent = nullptr);
    ~DirectoryListView ();
    
private:
    DirectoryTree *tree;
    
    void updateList ();
    
protected:
    void wheelEvent            (QWheelEvent*) override;
    void keyPressEvent         (QKeyEvent*)   override;
    void mouseDoubleClickEvent (QMouseEvent*) override;
        
public slots:
    void onCheckBoxClicked (const QString&);
    
signals:
    void selectedFolderUpdated (const QStringList&);
    
};

#endif // DIRECTORY_VIEW_LIST_H
