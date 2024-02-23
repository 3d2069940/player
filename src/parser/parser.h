
#ifndef PARSER_H
#define PARSER_H

#include <qcombobox.h>
#include <string>

#include <QDir>
#include <QString>
#include <QComboBox>


#include <yaml-cpp/yaml.h>


class Parser {

public:
    Parser ();

    void parseMusic (const QString &path, QFileInfoList &musicFiles);

    void parseConfigFile ();
      template <class T>
      void extractConfigInfo (T* var, const std::string &key);
      void loadDefaultConfig ();

    void saveConfigFile (const std::string &filePath);

    void parsePresetFile ();
      template <class T>
      void extractPresetInfo (QComboBox *combobox, const std::string &key);
      void loadDefaultPreset ();

    template <class T>
    void addToPreset (QComboBox *combobox, YAML::Node *node, 
                      const std::string &value, const std::string &presetName);
    void removeFromPreset (QComboBox *combobox, const std::string &key);

    void savePresetFile ();

//  getters
    const QString &getConfigFilePath   ();
    const QString &getPresetFilePath   ();

    const std::string &getDBPath       ();

    const YAML::Node  &getConfigYaml   ();
    const YAML::Node  &getPresetYaml   ();
    
    const QStringList &getExtensions   ();
    const QStringList &getMusicFolders ();
    
    bool getFlatButtons    ();
    bool getShowAudioCover ();
    bool getPlayAtStartup  ();
    bool getSaveLastAudio  ();

//  setters
    void setMusicFolders (const QStringList& folders); 
    template <class T>
    void setConfigYaml (const std::string &key, const T &value);

    void setFlatButtons    (bool value);
    void setShowAudioCover (bool value);
    void setPlayAtStartup  (bool value);
    void setSaveLastAudio  (bool value);
private:
    QString configFilePath,
            presetFilePath;

    std::string databasePath;

    YAML::Node configYaml,
               presetYaml;
//  Config
    QStringList extensions,
                musicFolders;
    
    bool flatButtons,
         showAudioCover,
         playAtStartup,
         saveLastAudio;
};

#endif // PARSER_H
