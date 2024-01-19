
#ifndef DIRECTORY_VIEW_WIDGET_H
#define DIRECTORY_VIEW_WIDGET_H

#include <QDir>
#include <QWidget>
#include <QString>

class QKeyEvent;
class QPushButton;
class QListWidget;
class QVBoxLayout;
class QListWidgetItem;
class DirectoryViewWidgetTree;

class DirectoryViewWidget : public QWidget {
    Q_OBJECT
    
public: 
    explicit DirectoryViewWidget (QListWidget*, QWidget *parent = nullptr);
    ~DirectoryViewWidget ();
    
    void setCurrentDirectory (const QString &path);

private:
    DirectoryViewWidgetTree *tree;
    
    QDir             currentPath;
    QListWidget     *listWidget;
    QVBoxLayout     *layout;
    QListWidgetItem *backItem;
    
    void updateCurrentFolders ();
    
signals:
    void newFolderSelected (const QStringList&);
    
public slots:
    void selectFolderClicked (int, const QString&);
    void nextFolderClicked   (QString);
    void backClicked         (QListWidgetItem*);
};

#endif // DIRECTORY_VIEW_WIDGET_H

