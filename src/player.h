
#ifndef PLAYER_H
#define PLAYER_H

//***********************************************************//
// STL
//***********************************************************//
#include <limits>

//***********************************************************//
// Qt5
//***********************************************************//
#include <QList>
#include <QTimer>
#include <QObject>
#include <QPixmap>
#include <QCloseEvent>
#include <QMainWindow>
#include <QFileInfoList>
#include <QSharedPointer>

//***********************************************************//
// Third-party libraries
//***********************************************************//
#include <gst/gst.h>
#include <yaml-cpp/yaml.h>

//***********************************************************//
// Autogen
//***********************************************************//
#include "ui_mainwindow.h"

class QEvent;
class Effects;
class Connector;
class QCloseEvent;
class ToggleButton;
class QListWidgetItem;
class PlaylistDelegate;
class PresetDialogWindow;
class EqualizerTabWidget;
class PlaylistDialogWindow;

class MainWindowUI : public QMainWindow {  
    Q_OBJECT
    friend Connector;
public:
    Ui::MainWindow ui;
    
    MainWindowUI ();
    virtual ~MainWindowUI ();
    
    template <class T>
    void updatePresetConfig (std::string presetName);
        
private:
    std::unique_ptr<Effects>           effects;
    QSharedPointer<PresetDialogWindow> presetDialogWindow;

    QSharedPointer<QShortcut> playPauseShortcut,
                              nextShortcut,
                              previousShortcut,
                              seekForwardShortcut,
                              seekBackwardShortcut;

    QSharedPointer<QMenu> extMenu;
    QSharedPointer<PlaylistDelegate> playlistDelegate;

    QList<QListWidgetItem*> playlistItems;

    
    QTimer audioPositionTimer,
           audioPanningTimer,
           audioVisualizingTimer;
    
    YAML::Node configYaml,
               presetYaml;
               
    QString     configPath,
                presetPath;

    QStringList extensions,
                musicFolders;
    
    QListWidgetItem *currentAudio {nullptr},
                    *stopAudio    {nullptr};
                    
    bool flatButtons    {false},
         showAudioCover {true},
         playAtStartup  {false},
         saveLastAudio  {false};

    std::string currentPresetType;
    
    QLinearGradient  gradient;
    QCPBars         *bars {nullptr};

    QImage albumCover;
    bool albumCoverSet;
    
//********Init********//
    void createWidgets       ();
    void setupWidgets        ();
        void setupIcons      ();
        void setupAnimations ();
            
//********Config********//
    void createConfigFile      ();
    void parseConfigFile       ();
        template <class T>
        void extractConfigInfo (T *var, std::string key);
    
//********Parse********//
    void parsePresetFile    ();
    void createPresetFile   ();
    void loadDefaultPresets ();
        template <class T>
        void parsePresets  (QComboBox *combobox, const std::string &key);
    void savePresets  ();
    void removePreset (std::string, QComboBox*);
    
    void parseMusic (const QString &path, QFileInfoList& musicFiles);
    void updatePlaylistWidget ();
        
//********Effects*******//    
    void serializeEqualizerParams  (YAML::Node *);
    void serializeDelayParams      (YAML::Node *);
    void serializeFilterParams     (YAML::Node *);
    void serializePitchParams      (YAML::Node *);
    void serializeCompressorParams (YAML::Node *);

    void changeEqualizerParams  (const QVariant&);
    void changeDelayParams      (const QVariant&);
    void changeFilterParams     (const QVariant&);
    void changePitchParams      (const QVariant&);
    void changeCompressorParams (const QVariant&);
        
//********Other********//
    void updateCurrentAudioCover  (const std::string &filePath);
    void setupPlayerMusicIcons    ();
    void updateAudioPositionLabel (gint64, gint64);

public slots:
//  Presets manipulation
    void addNewPreset          ();
//  Player slots
    void showTagListWidget     (const QString&);
    void closeTagListWidget    ();
    void togglePlaylistView    ();
    void previousButtonClicked ();
    void repeatButtonClicked   ();
    void pauseButtonClicked    ();
    void shuffleButtonClicked  ();
    void nextButtonClicked     ();
    void searchTextChanged     (const QString &searchText);
    void toggleSearchLineView  ();

    void playlistItemClicked   (QListWidgetItem*);
    void seekForward           ();
    void seekBackward          ();
    void updateAudioState      ();
//  Effects
    void updatePanPosition     ();    
//  Playlist
    void infoActionClicked     ();
    void setPlaylistStopAudio  (QListWidgetItem*);
    void removeFromPlaylist    (QListWidgetItem*);
//  Visualizing
    void bandsChanged            (int);
    void updateRateChanged       (int);
    void thresholdChanged        (int);
    void updateVisualizingWidget ();
//  Settings
    void themeComboBoxClicked   (const QString &theme);
    void extensionsMenuClicked  ();
    void newFolderSelected      (const QStringList&);
    void showSettingsTreeWidget ();
    void flatButtonsClicked     (int state);
    
protected:
    void changeEvent (QEvent       *event) override;
    void resizeEvent (QResizeEvent *event) override;
    void closeEvent  (QCloseEvent  *event) override;
    
};
#endif
