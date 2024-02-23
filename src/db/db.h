
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

    void open  ();
    void write (const std::string &tableName, const QList<QListWidgetItem*> &values);
    QStringList read  (const std::string &tableName);
    void createNewPlaylist (const std::string &playlistName);

    bool exists (const std::string &value, const std::string &tableName);


private:
    MainWindowUI *ptr;

    std::string dbPath;
    sqlite3* db;
};

#endif // DB_H
