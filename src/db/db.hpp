
#ifndef _DB_HPP_
#define _DB_HPP_

#include <ctime>

#include <QDebug>
#include <QListWidgetItem>
#include <sqlite3.h>

#include "db.h"
#include "src/widgets/playerTabPlaylist/playertabplaylistitem.h"


DataBase::~DataBase () {
    if (db != nullptr) 
        sqlite3_close(db);
}

void DataBase::open () {
    int rc = sqlite3_open(filePath.c_str(), &db);
    if (rc != SQLITE_OK)
        qWarning() << "[Warning]: Failed to open database";
    else
        qDebug() << "db is opened";
}

bool DataBase::createTable (const std::string &tableName) {
    char *msg = nullptr;
    std::string commandCreate = "CREATE TABLE IF NOT EXISTS " +
                                tableName +
                                " (NAME TEXT);";
    
    int rc = sqlite3_exec(db, commandCreate.c_str(),nullptr,nullptr,&msg);

    if (rc != SQLITE_OK) {
        qDebug() << msg;
        return false;
    }

    time_t currentTime = time(nullptr);
    char formattedString [30];
    strftime(formattedString, 30, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
    writeValue(tableName, formattedString,false);
    writeValue(tableName, formattedString,false);

    return true;
}

void DataBase::writeValue (const std::string &tableName,
                           const std::string &value,
                           bool skipDuplicates) {
    sqlite3_stmt *stmt = nullptr;
    std::string commandInsert;
    if (skipDuplicates) {
        commandInsert = 
            "INSERT INTO " + tableName +
            " (NAME) SELECT (?) WHERE NOT EXISTS (SELECT 1 FROM " + 
            tableName + " WHERE NAME = (?))";
        updateTime(tableName);
    }
    else
        commandInsert = 
            "INSERT INTO " + tableName +
            " (NAME) VALUES (?)";

    sqlite3_prepare_v2(db, commandInsert.c_str(), -1, &stmt, nullptr);
    
    sqlite3_bind_text(stmt,1,value.c_str(),-1, SQLITE_STATIC);
    if (skipDuplicates)
        sqlite3_bind_text(stmt,2,value.c_str(),-1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
        qDebug() << "Can't insert" << value.c_str() << "into" << tableName.c_str();

    sqlite3_finalize(stmt);
}                               

void DataBase::removeTable (const std::string &tableName) {
    std::string commandRemove = "DROP TABLE " + 
                                tableName;
    char *msg = nullptr;
    int rc = sqlite3_exec(db, commandRemove.c_str(), nullptr, nullptr, &msg);
    if (rc != SQLITE_OK)
        qWarning() << msg;
    sqlite3_free(msg);
}

void DataBase::removeValue(const std::string &tableName,
                           const std::string &value) {
    sqlite3_stmt *stmt;
    std::string commandRemove = "DELETE FROM " + tableName + 
                                " WHERE NAME = (?)";
    
    sqlite3_prepare_v2(db, commandRemove.c_str(), -1, &stmt, nullptr);
    
    sqlite3_bind_text(stmt,1,value.c_str(),-1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
        qDebug() << "Can't remove" << tableName.c_str() << value.c_str() << rc;

    sqlite3_finalize(stmt);
}

void DataBase::updateTime (const std::string &tableName) {
    sqlite3_stmt *stmt;
    std::string commandUpdate = "UPDATE " + tableName + 
                                " SET (NAME) = (?) WHERE rowid = 2";
    
    time_t currentTime = time(nullptr);
    char formattedString [30];
    strftime(formattedString, 30, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    sqlite3_prepare_v2(db, commandUpdate.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, formattedString, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
        qDebug() << "Can't update time" << rc;

}

void DataBase::setPath (const std::string &_filePath) {
    filePath = _filePath;
}

bool DataBase::tableExists (const std::string &tableName) {
    bool exists = false;
    sqlite3_stmt *stmt;
    std::string commandSearch = "SELECT name FROM sqlite_master "
                                "WHERE type='table' AND name='" +
                                tableName + "'";
    int rc = sqlite3_prepare_v2(db, commandSearch.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        sqlite3_finalize(stmt);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        exists = true;
    
    sqlite3_finalize(stmt);
    return exists;
}

QStringList DataBase::getTableNames () {
    QStringList tableNames;
    sqlite3_stmt *stmt;
    std::string commandRead =
        "SELECT name FROM sqlite_master WHERE type='table';";
    int rc = sqlite3_prepare_v2(db, commandRead.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return tableNames;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        tableNames << tableName;
    }
    sqlite3_finalize(stmt);
    return tableNames;
}

QStringList DataBase::getPlaylistTime (const std::string &tableName) {
    QStringList playlistTime;
    sqlite3_stmt *stmt;
    std::string commandRead = "SELECT * FROM " + tableName + " LIMIT 2";

    int rc = sqlite3_prepare_v2(db, commandRead.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return playlistTime;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
        playlistTime << reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));

    return playlistTime;
}

QStringList DataBase::getTableValues (const std::string &tableName) {
    QStringList values;
    sqlite3_stmt *stmt;
    std::string commandRead = "SELECT NAME FROM " + tableName;

    int rc = sqlite3_prepare_v2(db, commandRead.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return values;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto filePath = reinterpret_cast<const char *>(sqlite3_column_text(stmt,0));
        values << filePath;
    }
    sqlite3_finalize(stmt);
    return values;
}

#endif // _DB_HPP_
