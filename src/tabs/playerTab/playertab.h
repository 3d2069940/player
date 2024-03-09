
#ifndef _PLAYERTAB_H_
#define _PLAYERTAB_H_

//*******************************************************//
// Qt5
//*******************************************************//
#include <QEvent>
#include <QTimer>
#include <QDialog>
#include <QVector>
#include <QWidget>
#include <QFileInfo>
#include <QListWidget>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "../tab.h"
#include "src/db/db.h"
#include "src/tabs/playlistitemdelegate.h"
//*******************************************************//
// Autogen
//*******************************************************//
#include "ui_dialog.h"
#include "ui_playertab.h"
//*******************************************************//
// Forward Declaration
//*******************************************************//
class Effects;
class Parser;
class DataBase;

class PlayerTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit PlayerTab (DataBase *_db, QWidget *parent=nullptr);
    virtual ~PlayerTab ();

//  setters
    void setEffects  (Effects  *_effects);
    void setParser   (Parser   *_parser);

    void setMusicFolders (const QStringList &_musicFolders);
    void setExtensions   (const QStringList &_extensions);

private:
    Ui::PlayerTabWidget ui;
    Ui::Dialog          dialogUi;

    Effects  *effects = nullptr;
    Parser   *parser  = nullptr;
    DataBase *db      = nullptr;

    PlaylistItemDelegate delegate;
    QSharedPointer<QDialog> dialog;

    QTimer audioTimer;

    QStringList musicFolders,
                extensions;

    QImage albumCover;

    QString musicFile;

    QVector<QListWidgetItem*> playlistItems;
    QListWidgetItem *currentAudio = nullptr;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

    void updatePlayList ();

    void updateIcons ();

    void updateAlbumCover ();

protected:
    void changeEvent (QEvent       *event) override;
    void resizeEvent (QResizeEvent *event) override;
    void showEvent   (QShowEvent   *event) override;

private slots:
    void togglePlaylistView   ();
//  search
    void onLineEditTextChanged (const QString &newText);
    void toggleSearchLineView  ();
//  control buttons
    void onPrevButtonClicked  ();
    void onPauseButtonClicked ();
    void onNextButtonClicked  ();
//  slider
    void onSliderValueChanged (int value);
    void onSliderReleased     ();
    void onSliderPressed      ();
//  playlist
    void onPlayerPlaylistItemClicked (QListWidgetItem *item);

    void addToPlaylistClicked (const QString &_musicFile);
    void addToPlaylist ();
//  timer
    void updateAudioInfo ();
};

#endif // PLAYERTAB_H_
