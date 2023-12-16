
#ifndef PLAYER_H
#define PLAYER_H

#include <QTimer>
#include <QFileInfoList>

#include <gst/gst.h>


#include "ui_mainwindow.h"


class Effects;
class PresetDialogWindow;
class PlaylistWidgetItem;

class MainWindowUI : public Ui::MainWindow {  

public:
    MainWindowUI ();
    ~MainWindowUI ();
    
    QMainWindow *mainwindow ();
    void updatePresetConfig (std::string presetType, std::string presetName);
private:
//  Effects
    std::unique_ptr<Effects> effects;
    
    QTimer         audioPositionTimer,
                   audioPanningTimer;
    
    QMainWindow    mainwindow_;
    
    std::unique_ptr<PresetDialogWindow> dialogWindow;
    
    YAML::Node     config;
    QString        musicFolderPath;
    
    QListWidgetItem *currentAudio;
    int              currentAudioId;
    
    std::string currentPresetType;
    
    
    void parseMusic (QFileInfoList&);

//********Config********//
    void parseConfigFile  ();    
    void createConfigFile (QString &path);
    
    template <class T>
    void parsePresets           (std::string, QComboBox *);
    
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
    void connectEqualizerTabWidgets ();
    
    void connectEqualizerWidgets  ();
    void connectDelayWidgets      ();
    void connectFilterWidgets     ();
    void connectPitchWidgets      ();
    void connectCompressorWidgets ();
    void connectPanoramaWidgets   ();
    
//********Other********//
    
    void updateAudioState         ();
    void updateAudioPositionLabel (gint64, gint64);
    
    void updateVisualizingWidget  ();
    void updatePanPosition        ();

    void lockWidgetFor (QWidget*, quint64);
    
};
#endif
