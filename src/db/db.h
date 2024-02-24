
#ifndef DB_H
#define DB_H

#include <QListWidgetItem>
#include <sqlite3.h>
#include <string>

class MainWindowUI;

class DataBase {

public:
    DataBase  (const std::string &path, MainWindowUI *_ptr);
    ~DataBase ();

    inline void open   ();
    inline void write  (const std::string &tableName, const QList<QListWidgetItem*> &values);
    inline void remove (const std::string &tableName, const std::string &key);

    inline QStringList tables     ();
    inline QStringList readTable  (const std::string &tableName);
    inline void createNewPlaylist (const std::string &playlistName);

    inline bool tableExists (const std::string &tableName);
    inline bool exists (const std::string &value, const std::string &tableName);

//  setters
    inline void setPath (const std::string &_path);
    inline void setPtr  (MainWindowUI *_ptr);

private:
    MainWindowUI *ptr;

    std::string dbPath;
    sqlite3* db;
};

#endif // DB_H
