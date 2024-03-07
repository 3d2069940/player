
#ifndef _PLAYLISTTABPLAYLISTITEM_H_
#define _PLAYLISTTABPLAYLISTITEM_H_



#include <QWidget>
#include <QVariant>
#include <QFileInfo>


class QMenu;
class QLabel;
class QAction;
class QHBoxLayout;
class QPushButton;

class PlaylistTabPlaylistItem : public QWidget {
    Q_OBJECT
public:
    PlaylistTabPlaylistItem (const QVariant &_data);
    virtual ~PlaylistTabPlaylistItem ();

    std::string filePath () const;

private:
    QVariant data;

    QLabel *label;

    QMenu *menu;

    QAction *infoAction,
            *removeAction,
            *stopAction;

    QHBoxLayout *layout;
    QPushButton *button;

private slots:
    void removeClickedWrapper (); 

signals:
    void removeClicked (const QVariant& _data);

};


#endif // _PLAYLISTTABPLAYLISTITEM_H_
