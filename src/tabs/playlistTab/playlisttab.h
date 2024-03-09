
#ifndef _PLAYLISTTAB_H_
#define _PLAYLISTTAB_H_

#include "../tab.h"
#include "../playlistitemdelegate.h"

#include "ui_playlisttab.h"


#include <QEvent>



class Effects;
class DataBase;
class QPushButton;
class PlaylistTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit PlaylistTab (DataBase *_db, QWidget *parent=nullptr);
    virtual ~PlaylistTab ();

//  setters
    void setEffects (Effects *_effects);
private:
    Ui::PlaylistTabWidget ui;

    Effects  *effects = nullptr;
    DataBase *db      = nullptr;

    QPushButton *currentItem = nullptr;

    PlaylistItemDelegate delegate;

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
    void onPlaylistItemClicked   (QListWidgetItem *item);

    void onAcceptDeleteButtonBoxClicked ();
    void onRejectDeleteButtonBoxClicked ();
    void onPlaylistRenameClicked ();

    void onRemoveItemClicked (const QVariant &data);
};

#endif // _PLAYLISTTAB_H_
