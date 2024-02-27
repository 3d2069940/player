
#ifndef _PARSER_H_
#define _PARSER_H_


#include <QFileInfoList>

class Parser {

public:
    Parser ();
    ~Parser ();

    void parseMusic (const QString &path, QFileInfoList &musicFiles);

private:
    QStringList extensions;
    QStringList musicFolders;
};

#endif // _PARSER_H_
