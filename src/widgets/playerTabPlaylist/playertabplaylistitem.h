
#ifndef _PLAYERTABPLAYLISTTAB_H_
#define _PLAYERTABPLAYLISTTAB_H_


#include <QMenu>
#include <QLabel>
#include <QVariant>
#include <QHBoxLayout>
#include <QPushButton>


class PlayerTabPlaylistItem : public QWidget {
    Q_OBJECT
public:
    explicit PlayerTabPlaylistItem (const QString&, const QVariant &data);
    virtual ~PlayerTabPlaylistItem ();
    
    std::string filePath () const;
    
    QVariant     data;
    
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


#endif // _PLAYERTABPLAYLISTTAB_H_
