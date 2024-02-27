
#ifndef _DIRECTORYLISTVIEW_H_
#define _DIRECTORYLISTVIEW_H_


#include <QListWidget>

class DirectoryTree;

class DirectoryListView : public QListWidget {
    Q_OBJECT

public:
    explicit DirectoryListView (QWidget *parent = nullptr);
    virtual ~DirectoryListView ();

    void markSelectedDirectories (const QStringList &directories);

private:
    DirectoryTree *tree;

    void update ();

protected:
    void wheelEvent            (QWheelEvent *event) override;
    void keyPressEvent         (QKeyEvent   *event) override;
    void mouseDoubleClickEvent (QMouseEvent *event) override;
    void focusOutEvent         (QFocusEvent *event) override;
};

#endif // _DIRECTORYLISTVIEW_H_
