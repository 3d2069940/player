
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
class DirectoryListView;
class PresetDialogWindow;
class EqualizerTabWidget;
class PlaylistDialogWindow;

class MainWindowUI : public QMainWindow {  
    Q_OBJECT
    
    friend Connector;
public:
    Ui::MainWindow ui;
    
    MainWindowUI ();
    ~MainWindowUI ();
    
    template <class T>
    void updatePresetConfig (std::string presetName);
        
private:

    std::unique_ptr<Effects>           effects;
    QSharedPointer<PresetDialogWindow> presetDialogWindow;
        
    QList<QListWidgetItem*> playlistItems;
    
    QTimer audioPositionTimer,
           audioPanningTimer,
           audioVisualizingTime;
    
    YAML::Node configYaml,
               presetYaml;
               
    QString     configPath,
                presetPath;
    QStringList musicFolders;
    
    QListWidgetItem *currentAudio = nullptr,
                    *stopAudio    = nullptr;
                    
    std::string currentPresetType;
    
    QLinearGradient         gradient;
    QSharedPointer<QBrush>  brush;
    QSharedPointer<QCPBars> bars;
    
//********Init********//
    void createWidgets  ();
    void setupWidgets   ();
        void setupIcons ();
//  Connecting All Widgets
    void connectWidgets ();
//      Connect Player Tab Widgets
        void connectPlayerTabWidgets ();
//      Connect Effects Tab Widgets
        void connectEffectsTabWidgets ();
            void connectEqualizerWidgets  ();
            void connectDelayWidgets      ();
            void connectFilterWidgets     ();
            void connectPitchWidgets      ();
            void connectCompressorWidgets ();
            void connectPanoramaWidgets   ();
//      Connect Visualizing Tab Widgets
        void connectVisualizingTabWidgets ();
//      Connect Settings Tab Widgets
        void connectSettingsTabWidgets    ();
            
//********Config********//
    void createConfigFile      ();
    void parseConfigFile       ();
        template <class T>
        void extractConfigInfo (std::string);
    
//********Parse********//
    void createPresetFile ();
    void parsePresetFile  ();
        template <class T>
        void parsePresets (std::string);
    void savePresets  ();
    void removePreset (std::string, QComboBox*);
    
    void parseMusic (QFileInfoList&);
    void updatePlaylistWidget ();
        
//********Effects*******//    
    void serializeEqualizerParams  (YAML::Node *);
    void serializeDelayParams      (YAML::Node *);
    void serializeFilterParams     (YAML::Node *);
    void serializePitchParams      (YAML::Node *);
    void serializeCompressorParams (YAML::Node *);

    void changeEqualizerParams  (QVariant);
    void changeDelayParams      (QVariant);
    void changeFilterParams     (QVariant);
    void changePitchParams      (QVariant);
    void changeCompressorParams (QVariant);
        
//********Other********//
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
    
    void playlistItemClicked   (QListWidgetItem*);
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
    void newFolderSelected      (const QStringList&);
    void showSettingsTreeWidget ();
    
protected:
    void changeEvent (QEvent       *event) override;
    void resizeEvent (QResizeEvent *event) override;
    void closeEvent  (QCloseEvent  *event) override;
    
};
#endif
