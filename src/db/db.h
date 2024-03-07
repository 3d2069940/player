
#ifndef _DB_H_
#define _DB_H_


#include <string>

#include <QList>
#include <QStringList>
#include <QStandardPaths>

#include <sqlite3.h>


class DataBase {

public:
    DataBase  () = default;
    ~DataBase ();

    void open        ();
    bool createTable (const std::string &tableName);
    void writeValue  (const std::string &tableName, 
                      const std::string &value,
                      bool skipDuplicates = true);

    void removeTable (const std::string &tableName);
    void removeValue (const std::string &tableName, 
                      const std::string &value);

    void updateTime (const std::string &tableName);

//  setters
    void setPath (const std::string &_filePath);
//  getters
    bool tableExists (const std::string &tableName);

    QStringList getTableNames   ();
    QStringList getPlaylistTime (const std::string &tableName);
    QStringList getTableValues  (const std::string &tableName);
private:
    std::string filePath;
    sqlite3 *db = nullptr;

};

#endif // _DB_H_
