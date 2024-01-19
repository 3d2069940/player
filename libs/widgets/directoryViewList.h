
#ifndef DIRECTORY_VIEW_LIST_H
#define DIRECTORY_VIEW_LIST_H

#include <QListWidget>


class QWidget;
class DirectoryViewList : public QListWidget {
    Q_OBJECT
    
public:
    explicit DirectoryViewList (QWidget *parent = nullptr);
    virtual ~DirectoryViewList ();
    
protected:
    void wheelEvent (QWheelEvent *) override;
    void keyPressEvent (QKeyEvent *) override;
    
public slots:
    
signals:

    
};

#endif // DIRECTORY_VIEW_LIST_H
