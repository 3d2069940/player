
#ifndef _PARSER_H_
#define _PARSER_H_


#include <string>


#include <yaml-cpp/yaml.h>


#include <QComboBox>
#include <QFileInfoList>

class Parser {

public:
    Parser ();
    ~Parser ();

//  getters
    void getMusicFiles   (QFileInfoList &musicFiles);
    
    QStringList getExtensions   ();
    QStringList getMusicFolders ();

    QImage getAudioCover (const std::string &filePath);

private:
    QString configFilePath,
            presetFilePath;

    YAML::Node configYAML,
               presetYAML;

    std::string databasePath;

    bool flatButtons,
         showAudioCover,
         playAtStartup,
         saveLastAudio;

    QStringList extensions;
    QStringList musicFolders;

    void parseConfigFile ();
      template<class T>
      void extractConfigInfo(T *var, const std::string &key);

      void loadDefaultConfig ();

    void parsePresetFile ();
      template<class T>
      void extractPresetInfo(QComboBox *combobox, const std::string &key);

    void parseMusic (const QString &path, QFileInfoList &musicFiles);

    void parseAudioCover (const std::string &filePath);
};

#endif // _PARSER_H_
