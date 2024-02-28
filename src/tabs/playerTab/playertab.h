
#ifndef _PLAYERTAB_H_
#define _PLAYERTAB_H_

//*******************************************************//
// Qt5
//*******************************************************//
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <QFileInfo>
#include <QListWidget>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "../tab.h"
#include "playertabitemdelegation.h"
//*******************************************************//
// Autogen
//*******************************************************//
#include "ui_playertab.h"
//*******************************************************//
// Forward Declaration
//*******************************************************//
class Effects;
class Parser;

class PlayerTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit PlayerTab (QWidget *parent=nullptr);
    virtual ~PlayerTab ();

//  setters
    void setEffects      (Effects *_effects);
    void setParser       (Parser  *_parser);

    void setMusicFolders (const QStringList &_musicFolders);
    void setExtensions   (const QStringList &_extensions);

private:
    Ui::PlayerTabWidget ui;

    Effects *effects = nullptr;
    Parser  *parser  = nullptr;

    QSharedPointer<PlayerTabItemDelegate> delegate;

    QTimer audioTimer;

    QStringList musicFolders,
                extensions;

    QVector<QListWidgetItem*> playlistItems;
    QListWidgetItem *currentAudio = nullptr;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

    void updatePlayList ();

    void updateIcons ();

protected:
    void changeEvent (QEvent *event) override;

private slots:
    void togglePlaylistView   ();
//  search
    void onLineEditTextChanged (const QString &newText);
    void toggleSearchLineView  ();
//  control buttons
    void onPrevButtonClicked  ();
    void onPauseButtonClicked ();
    void onNextButtonClicked  ();

    void onPlayerPlaylistItemClicked (QListWidgetItem *item);

    void updateAudioInfo ();
};

#endif // PLAYERTAB_H_
