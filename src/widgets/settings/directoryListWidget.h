
#ifndef DIRECTORY_LIST_ELEMENT_H
#define DIRECTORY_LIST_ELEMENT_H


#include <QWidget>

class QLabel;
class QString;
class QCheckBox;
class QHBoxLayout;
class QListWidgetItem;

class DirectoryListWidget : public QWidget {
    Q_OBJECT
public:
    DirectoryListWidget  (const QString&, Qt::CheckState);
    ~DirectoryListWidget ();
    
    void    spacePressed ();
    QString dirName   ();
private:
    QLabel      *label;
    QCheckBox   *checkbox;
    QHBoxLayout *layout;
    
public slots:
    void onCheckBoxClickedWrapper (int);
    
signals:
    void onCheckBoxClicked (const QString&);
    
};

#endif // DIRECTORY_LIST_ELEMENT_H
