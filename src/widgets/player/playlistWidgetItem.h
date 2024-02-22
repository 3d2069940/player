
#ifndef PLAYLIST_WIDGET_ITEM
#define PLAYLIST_WIDGET_ITEM


//***********************************************************//
// Qt5
//***********************************************************//
#include <QMenu>
#include <QLabel>
#include <QAction>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QResizeEvent>


class PlaylistWidgetItem : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistWidgetItem (const QString&, const QVariant &data);
    virtual ~PlaylistWidgetItem ();
    
    std::string filePath () const;
    
    QVariant     m_data;
    
    QLabel      *label;
    
    QMenu       *contextMenu;
    QAction     *infoAction,
                *removeAction,
                *addToPlaylist,
                *stopAction;
                
    QHBoxLayout *layout;
    QPushButton *button;
    
private:
    void updateIcons ();
    
protected:
    void changeEvent (QEvent *event) override;
    
public slots:
    void contextMenuShow       ();
    void infoActionClicked ();
    
signals:
    void infoActionShow (const QString&);
    
};

#endif
