
#ifndef DB_HPP
#define DB_HPP

#include <iostream>

#include "db.h"
#include "player.h"
#include "playlistWidgetItem.h"

#include <QDebug>

#include <QList>
#include <QListWidgetItem>
#include <cstddef>
#include <qlistwidget.h>
#include <sqlite3.h>


DataBase::DataBase (const std::string &path, MainWindowUI* _ptr) 
    : ptr(_ptr), dbPath(path) {

}

DataBase::~DataBase () {
    sqlite3_close(db);
}

void DataBase::open () {
    int returnCode = sqlite3_open(dbPath.c_str(),&db);
    if (returnCode) {
        qWarning() << "[Warning]: failed to open database " << dbPath.c_str();
  } else {
        qDebug() << "Database opened successfully";
  }
}

void DataBase::write (const std::string &tableName, const QList<QListWidgetItem*> &values) {
    std::string commandCreate = "; CREATE TABLE IF NOT EXISTS " + tableName +
                                " (NAME TEXT);";
 
    sqlite3_exec(db, commandCreate.c_str(), nullptr,nullptr,nullptr);
    foreach (QListWidgetItem* item, values) {
        auto widget = qobject_cast<PlaylistWidgetItem*>(ptr->ui.playerPlayListWidget->itemWidget(item));
        if (!exists(widget->filePath(), tableName)) {
            std::string query = "INSERT INTO " + tableName + " (NAME) VALUES ('" + widget->filePath() + "');";
            sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
        }
    }
}

QStringList DataBase::read (const std::string &tableName) {
    QStringList result;
    sqlite3_stmt *stmt;
    std::string commandRead = "SELECT NAME FROM " + tableName + " ;";
    sqlite3_prepare_v2(db, commandRead.c_str(), -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *filePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        result << QString(filePath);
    }
    sqlite3_finalize(stmt);
    return result;
}

void DataBase::createNewPlaylist (const std::string &playlistName) {
    std::string commandCreate = "; CREATE TABLE IF NOT EXISTS " + playlistName +
                                " (NAME TEXT);";
    sqlite3_exec(db, commandCreate.c_str(), nullptr, nullptr, nullptr);

}

bool DataBase::exists (const std::string &value, const std::string &tableName) {
    sqlite3_stmt *stmt;
    std::string query = "SELECT * FROM "+tableName+" WHERE NAME=?";
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_STATIC);
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (result == SQLITE_ROW) ? true : false;
}

#endif // DB_HPP