
#ifndef _PARSER_HPP_
#define _PARSER_HPP_

//*******************************************************//
// Qt5
//*******************************************************//
#include <QDir>
//*******************************************************//
// Homebrew classes
//*******************************************************//
#include "parser.h"


Parser::Parser () {

}

Parser::~Parser() {

}

void Parser::parseMusic (const QString &path, QFileInfoList &musicFiles) {
    QDir musicDir (path);
    QStringList extensions;
    extensions << "*.opus";
    musicDir.setNameFilters(extensions);
    musicFiles += musicDir.entryInfoList(QDir::Files);

    QDir folderDir (path);
    folderDir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList folderList = folderDir.entryInfoList();
    foreach (const QFileInfo &folder, folderList)
        parseMusic(folder.absoluteFilePath(), musicFiles);
}

#endif // _PARSER_HPP_
