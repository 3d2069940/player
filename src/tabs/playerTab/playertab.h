
#ifndef _PLAYERTAB_H_
#define _PLAYERTAB_H_

//*******************************************************//
// Qt5
//*******************************************************//
#include <QTimer>
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


class PlayerTab : public QWidget, public Tab {
    Q_OBJECT
public:
    explicit PlayerTab (QWidget *parent=nullptr);
    virtual ~PlayerTab ();

//  setters
    void setEffects      (Effects *_effects);
    void setMusicFolders (const QStringList &_musicFolders);
    void setExtensions   (const QStringList &_extensions);

private:
    Ui::PlayerTabWidget ui;

    Effects *effects;

    QSharedPointer<PlayerTabItemDelegate> delegate;

    QTimer audioTimer;

    QStringList musicFolders,
                extensions;

    void createWidgets () override;
    void setupTab      () override;
    void connectTab    () override;

    void updatePlayList ();
    void parseMusic     (const QString &path, QFileInfoList &musicFiles);

    void updateIcons ();

protected:
    void changeEvent (QEvent *event) override;

private slots:
    void togglePlaylistView   ();
    void toggleSearchLineView ();

    void onPauseButtonClicked ();
    void onPlayerPlaylistItemClicked (QListWidgetItem *item);

    void updateAudioInfo ();
};

#endif // PLAYERTAB_H_
