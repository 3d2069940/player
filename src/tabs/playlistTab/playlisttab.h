
#ifndef _PLAYLISTTAB_H_
#define _PLAYLISTTAB_H_


#include "ui_playlisttab.h"


class PlaylistTab : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistTab (QWidget *parent=nullptr);
    virtual ~PlaylistTab ();

private:
    Ui::PlaylistTabWidget ui;
};

#endif // _PLAYLISTTAB_H_
