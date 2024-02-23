
#ifndef PARSER_CPP
#define PARSER_CPP

#include "parser.h"
#include "metaTypes.h"

#include <QDebug>
#include <QStringList>
#include <QStandardPaths>
#include <yaml-cpp/exceptions.h>

Parser::Parser () {
    
}

void Parser::parseMusic (const QString &path , QFileInfoList &musicFiles) {
    QDir musicDir (path);
    musicDir.setNameFilters(extensions);
    musicFiles += musicDir.entryInfoList(QDir::Files);

    QDir folderDir (path);
    QFileInfoList folderList;
    folderDir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    folderList = folderDir.entryInfoList();
    foreach (const QFileInfo &folder, folderList)
        parseMusic (folder.absoluteFilePath(), musicFiles);
}

void Parser::parseConfigFile () {
    configFilePath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) +
                            "/player/config.yaml";
    QFile configFile (configFilePath);
    QFileInfo configFileInfo (configFile);
    bool defaultValues = false;

    configFileInfo.absoluteDir().mkpath(".");

    if (!configFile.exists()) {
        loadDefaultConfig();
        defaultValues = true;
    }

    if (!defaultValues && !configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Warning]:" << configFile.errorString();
        loadDefaultConfig();
        defaultValues = true;
    } else if (!defaultValues && configFile.size() == 0) {
        qWarning() << "[Warning]: Config file is empty. Loading default values";
        loadDefaultConfig();
        defaultValues = true;
        configFile.close();
    }
    if (!defaultValues) {
        QString configData = configFile.readAll();
        configFile.close();
        try {
            configYaml = YAML::Load(configData.toStdString());
        } catch (YAML::ParserException &e) {
            qWarning() << "[Warning]: Bad config data. Loading default values";
            loadDefaultConfig();
        } catch (...) {
            qWarning() << "[Warning]: Unknown error while parsing config file";
            loadDefaultConfig();
        }

    }
    
    extractConfigInfo<QStringList>(&extensions, "Extensions");
    extractConfigInfo<QString>    (&presetFilePath, "PresetFilePath");
    if (presetFilePath == "")
        presetFilePath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) +
                            "/player/presets.yaml";
    extractConfigInfo<QStringList>(&musicFolders,"MusicFolders");
    extractConfigInfo<std::string>(&databasePath, "DataBasePath");
    extractConfigInfo<bool>       (&flatButtons,    "FlatButtons");
    extractConfigInfo<bool>       (&showAudioCover, "ShowAudioCover");
    extractConfigInfo<bool>       (&playAtStartup,  "PlayAtStartup");
    extractConfigInfo<bool>       (&saveLastAudio,  "SaveLastAudio");
}

template <class T>
void Parser::extractConfigInfo (T *var, const std::string &key) {
    if (!configYaml[key].IsDefined()) {
        qWarning() << "[Warning]:" << key.c_str() << "doesn't exist. Skipped";
        return;
    }
    const YAML::Node &element = configYaml[key];
    try {
        *var = element.as<T>();
    } catch (YAML::BadConversion &e) {
        qWarning() << "[Warning]:" << e.what();
    } catch (...) {
        qWarning() << "[Warning]: Unknown error. Skipped" << key.c_str();
    }
}

void Parser::loadDefaultConfig () {
    std::string configData = R"~(
PresetFilePath: ""
Extensions: ["*.mp3", "*.flac", "*.opus", "*.wav"]
ShowAudioCover: true
FlatButtons: false
PlayAtStartup: true
SaveLastAudio: true
MusicFolders: []
DataBasePath: ~
LastAudioFilePath: ~
)~";
    configYaml = YAML::Load(configData);
}

void Parser::saveConfigFile (const std::string &filePath) {
    QFile configFile (configFilePath);

    if (!configFile.open (QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning () << "[Warning]:" << "Unable to open config file. Skipping";
        return;
    }

    if (saveLastAudio && filePath != "") {
        configYaml["LastAudioFilePath"] = filePath;
  } else if (!saveLastAudio)
        configYaml["LastAudioFilePath"] = YAML::Null;

    configYaml["SaveLastAudio"] = saveLastAudio;

    YAML::Emitter emitter;
    emitter << configYaml;

    if (configFile.write (emitter.c_str()) == -1)
        qWarning() << "[Warning]:" << "Writing config data to a file failed. Skipping";

    configFile.close ();


}

void Parser::parsePresetFile () {
    QFile presetFile (presetFilePath);
    bool defaultValues = false;

    if (!presetFile.exists()) {
        loadDefaultPreset();
        defaultValues = true;
    }
    if (!defaultValues && !presetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        loadDefaultPreset();
        defaultValues = true; 
        qWarning() << "[Warning]:" << presetFile.errorString();   
    }
    if (!defaultValues && presetFile.size() == 0) {
        qWarning() << "[Warning]: Empty preset file. Loading default values";
        loadDefaultPreset();
        defaultValues = true;
        presetFile.close();
    }
    if (!defaultValues) {
        QString presetData = presetFile.readAll();
        presetFile.close();
        try {
            presetYaml = YAML::Load(presetData.toStdString().c_str());
        } catch (YAML::ParserException &e) {
            qWarning() << "[Warning]: Bad preset file data. Loading default values";
            loadDefaultPreset();
        } catch (...) {
            qWarning() << "[Warning]: Unknown error while parsing preset file";
            loadDefaultPreset();
        }
    }
}

template <class T>
void Parser::extractPresetInfo (QComboBox *combobox, const std::string &key) {
    QVariant data;
    std::string presetName;

    if (!presetYaml[key].IsDefined()) {
        qWarning() << key.c_str() << "doesn't exists. Skipped";
        presetYaml[key] = YAML::Null;
        return;
    }
    const YAML::Node &element = presetYaml[key];
    for (YAML::const_iterator it = element.begin(); it != element.end(); ++it) {
        std::string presetName = it->first.as<std::string>();
        try {
            data = QVariant::fromValue(it->second.as<T>());
        } catch (YAML::BadConversion &e) {
            qWarning() << "[Warning]:" << e.what();
            continue;
        } catch (...) {
            qWarning() << "[Warning]. Unknown error" << key.c_str();
            continue;
        }
        combobox->addItem(QString::fromStdString(presetName),data);
    }
}

void Parser::loadDefaultPreset () {
    std::string presetData = R"~(
...
Equalizer:
  Default: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
  Bass: [30, 30, 20, 10, 0, 0, 0, 0, 0, 0]
  Treble: [0, 0, 0, 0, 0, 10, 20, 30, 30, 30]
  Midrange: [0, 10, 20, 30, 30, 30, 20, 10, 0, 0]
  Soft: [-20, -10, -10, 0, 10, 10, 0, -10, -20, -20]
  Bright: [0, 0, 10, 20, 30, 30, 20, 10, 0, 0]
  Full Bass: [40, 30, 20, 10, 0, -10, -20, -30, -40, -40]
  High-pitched: [0, -10, -20, -20, -10, 10, 20, 20, 10, 0]
  Bass boost: [10, 20, 20, 10, -10, -20, -20, -10, 10, 20]
  All boost: [20, 20, 20, 20, 20, 20, 20, 20, 20, 20]
Delay: ~
Filter: ~
Pitch: ~
Compressor: ~
---)~";
    presetYaml = YAML::Load(presetData);
}

void Parser::savePresetFile () {

}

const std::string &Parser::getDBPath () {
    return this->databasePath;
}

const YAML::Node &Parser::getConfigYaml () {
    return this->configYaml;
}

const YAML::Node &Parser::getPresetYaml () {
    return this->presetYaml;
}
    
const QStringList &Parser::getExtensions () {
    return this->extensions;
}

const QStringList &Parser::getMusicFolders () {
    return this->musicFolders;
}
    
bool Parser::getFlatButtons () {
    return this->flatButtons;
}

bool Parser::getShowAudioCover () {
    return this->showAudioCover;
}

bool Parser::getPlayAtStartup () {
    return this->playAtStartup;
}

bool Parser::getSaveLastAudio () {
    return this->saveLastAudio;
}

void Parser::setMusicFolders (const QStringList &folders) {
    this->musicFolders = folders;
}

template <class T>
void Parser::setConfigYaml(const std::string &key, const T &value) {
    if (configYaml[key].IsDefined())
        configYaml[key] = value;
    else
        qDebug() << "Trying to set value that doesn't exist" << key.c_str();
}

void Parser::setFlatButtons    (bool value) {
    flatButtons = value;
}

void Parser::setShowAudioCover (bool value) {
    showAudioCover = value;
}

void Parser::setPlayAtStartup  (bool value) {
    playAtStartup = value;
}

void Parser::setSaveLastAudio  (bool value) {
    saveLastAudio = value;
}


#endif // PARSER_CPP