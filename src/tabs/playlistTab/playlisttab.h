
#ifndef _PLAYLISTTAB_H_
#define _PLAYLISTTAB_H_

#include "../tab.h"
#include "ui_playlisttab.h"


class PlaylistTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit PlaylistTab (QWidget *parent=nullptr);
    virtual ~PlaylistTab ();

private:
    Ui::PlaylistTabWidget ui;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

private slots:
    void onCreateButtonClicked ();

    void onAcceptButtonBoxClicked ();
    void onRejectButtonBoxClicked ();
};

#endif // _PLAYLISTTAB_H_
