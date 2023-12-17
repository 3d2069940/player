
#ifndef PLAYER_H
#define PLAYER_H


//***********************************************************//
// Qt5
//***********************************************************//
#include <QTimer>
#include <QObject>
#include <QPixmap>
#include <QCloseEvent>
#include <QMainWindow>
#include <QFileInfoList>

//***********************************************************//
// Third-party libraries
//***********************************************************//
#include <gst/gst.h>
#include <yaml-cpp/yaml.h>

//***********************************************************//
// Autogen
//***********************************************************//
#include "ui_mainwindow.h"


class Effects;
class PresetDialogWindow;
class PlaylistWidgetItem;
class EqualizerTabWidget;

class MainWindowUI : public QMainWindow {  
    Q_OBJECT
    
public:
    Ui::MainWindow ui;
    
    MainWindowUI ();
    ~MainWindowUI ();
    
    QMainWindow *mainwindow ();
    
    template <class T>
    void updatePresetConfig (std::string presetName);
        
private:
//  Effects
    std::unique_ptr<Effects> effects;
    
    QTimer         audioPositionTimer,
                   audioPanningTimer;
    
    std::unique_ptr<PresetDialogWindow> dialogWindow;
    
    YAML::Node     config;
    QString        musicFolderPath,
                   configPath;
    
    QListWidgetItem *currentAudio;
    int              currentAudioId = -1;
    
    std::string currentPresetType;
    
    void parseMusic (QFileInfoList&);

//********Config********//
    void parseConfigFile  ();    
    void createConfigFile ();
    
    template <class T>
    void parsePresets (std::string);
    void savePresets  ();
    void removePreset (std::string, QComboBox*);
    
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
    
    void setupWidgets ();
    void connectWidgets ();
    
//********Player********//
    void connectPlayerTabWidgets ();
    
//********Effects*******//
    void connectEffectsTabWidgets ();
     
    void connectEqualizerWidgets  ();
    void connectDelayWidgets      ();
    void connectFilterWidgets     ();
    void connectPitchWidgets      ();
    void connectCompressorWidgets ();
    void connectPanoramaWidgets   ();
    
//********Other********//

    void updateAudioPositionLabel (gint64, gint64);
    
    void updateVisualizingWidget  ();

    void lockWidgetFor (QWidget*, quint64);

public slots:
//  Player Items
    void togglePlaylistView    ();
    void previousButtonClicked ();
    void pauseButtonClicked    ();
    void nextButtonClicked     ();
    void playlistItemClicked   (QListWidgetItem*);
    void updateAudioState      ();
//  Presets
    void addNewPreset ();
//  Effects
    
    void updatePanPosition ();    

protected:
    void closeEvent (QCloseEvent *event) override;
    
};
#endif
