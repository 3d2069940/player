
#ifndef _PLAYLISTTAB_H_
#define _PLAYLISTTAB_H_

#include "../tab.h"
#include "ui_playlisttab.h"
#include <qevent.h>



class DataBase;
class QPushButton;
class PlaylistTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit PlaylistTab (DataBase *_db, QWidget *parent=nullptr);
    virtual ~PlaylistTab ();

private:
    Ui::PlaylistTabWidget ui;

    DataBase *db = nullptr;

    QPushButton *currentItem = nullptr;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

protected:
    void showEvent (QShowEvent *event) override;

private slots:
    void onCreateButtonClicked ();

    void onAcceptCreationButtonBoxClicked ();
    void onRejectCreationButtonBoxClicked ();

    void onPlaylistButtonClicked ();
    
    void onAcceptDeleteButtonBoxClicked ();
    void onRejectDeleteButtonBoxClicked ();
    void onPlaylistRenameClicked ();

    void onRemoveItemClicked (const QVariant &data);
};

#endif // _PLAYLISTTAB_H_
