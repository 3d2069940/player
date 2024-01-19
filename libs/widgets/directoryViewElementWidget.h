

#ifndef DIRECTORY_VIEW_ELEMENT_WIDGET_H
#define DIRECTORY_VIEW_ELEMENT_WIDGET_H

#include <QWidget>
#include <QString>

class QLabel;
class QCheckBox;
class QHBoxLayout;

class DirectoryViewElementWidget : public QWidget {
    Q_OBJECT

public:
    DirectoryViewElementWidget  (const QString&, Qt::CheckState state = Qt::Unchecked);
    ~DirectoryViewElementWidget ();
    
private:
    
    QHBoxLayout *layout;
    QCheckBox   *checkbox;
    QLabel      *filePath;
    
protected:
    void mouseDoubleClickEvent (QMouseEvent *) override;
    
private slots:
    void onCheckBoxClickedWrapper (int);
    
signals:
    void onCheckBoxClicked (int, const QString&);
    void onLabelClicked    (QString);
};

#endif // DIRECTORY_VIEW_ELEMENT_WIDGET_H

