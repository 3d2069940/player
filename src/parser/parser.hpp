
#ifndef _PARSER_HPP_
#define _PARSER_HPP_

//*******************************************************//
// Qt5
//*******************************************************//
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qstandardpaths.h>
//*******************************************************//
// TagLib
//*******************************************************//
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <opusfile.h>
#include <mpegfile.h>
#include <flacfile.h>
#include <aifffile.h>
#include <tpropertymap.h>
#include <attachedpictureframe.h>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "parser.h"


#include "parserBindings.h"


Parser::Parser () {
    parseConfigFile();
    parsePresetFile();
}

Parser::~Parser() {

}

void Parser::getMusicFiles (QFileInfoList &musicFiles) {
    if (musicFolders.empty())
        musicFolders.append(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    foreach (const QString &path, musicFolders)
        parseMusic(path,musicFiles);
}

QStringList Parser::getExtensions () {
    return extensions;
}

QStringList Parser::getMusicFolders () {
    return musicFolders;
}

QImage Parser::getAudioCover (const std::string &filePath) {
    QImage albumCover;
    if (!_showAudioCover)
        return albumCover;

       TagLib::FileRef file (filePath.c_str());
    QByteArray imageData;
    if (TagLib::MPEG::File *mpegFile = dynamic_cast<TagLib::MPEG::File*>(file.file())) {
        if (auto mpegTag   = mpegFile->ID3v2Tag()) {
            if (auto mpegFrame = mpegTag->frameList("APIC"); mpegFrame.size() > 0) {
                auto mpegImage = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(mpegFrame.front());
                if (mpegImage)
                    imageData = QByteArray (mpegImage->picture().data(), mpegImage->picture().size());
            }
        }
  } else if (TagLib::FLAC::File *flacFile = dynamic_cast<TagLib::FLAC::File*>(file.file())) {
        if (auto flacTag = flacFile->pictureList(); flacTag.size() > 0) {
            auto flacImage = flacTag.front();
            if (flacImage)
                imageData = QByteArray (flacImage->data().data(), flacImage->data().size());
        }
  } else if (TagLib::Ogg::Opus::File *opusFile = dynamic_cast<TagLib::Ogg::Opus::File*>(file.file())) {
        auto opusTag = opusFile->tag();
        if (auto opusFrame = opusTag->pictureList(); opusFrame.size() > 0) {
            auto opusImage = opusFrame.front();
            if (opusImage)
                imageData = QByteArray (opusImage->data().data(), opusImage->data().size());
        }
    }
    if (imageData.size() > 0)
        albumCover.loadFromData(imageData, "JPEG");

    return albumCover;
}

bool Parser::flatButtons () const {
    return _flatButtons;
}

bool Parser::showAudioCover () const {
    return _showAudioCover;
}

bool Parser::playAtStartup () const {
    return _playAtStartup;
}

bool Parser::saveLastAudio () const {
    return _saveLastAudio;
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
            configYAML = YAML::Load(configData.toStdString());
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
    extractConfigInfo<bool>       (&_flatButtons,    "FlatButtons");
    extractConfigInfo<bool>       (&_showAudioCover, "ShowAudioCover");
    extractConfigInfo<bool>       (&_playAtStartup,  "PlayAtStartup");
    extractConfigInfo<bool>       (&_saveLastAudio,  "SaveLastAudio");
}

template <class T>
void Parser::extractConfigInfo (T *var, const std::string &key) {
    if (!configYAML[key].IsDefined()) {
        qWarning() << "[Warning]:" << key.c_str() << "doesn't exist. Skipped";
        return;
    }
    const YAML::Node &element = configYAML[key];
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
    configYAML = YAML::Load(configData);
}

void Parser::parsePresetFile () {
    
}

template <class T>
void Parser::extractPresetInfo (QComboBox *combobox,  const std::string &key) {
    QVariant data;
    std::string presetName;

    combobox->clear();

    if (!presetYAML[key].IsDefined()) {
        qWarning() << key.c_str() << "doesn't exists. Skipped";
        presetYAML[key] = YAML::Null;
        return;
    }
    const YAML::Node &element = presetYAML[key];
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

void Parser::parseMusic (const QString &path, QFileInfoList &musicFiles) {
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

#endif // _PARSER_HPP_
