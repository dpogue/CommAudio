#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <list>
#include <map>
#include <qstring.h>

class MusicLibrary {

private:
    std::list<QString> directories;
    std::map<QString, QString> songs;

public:
    MusicLibrary();
    ~MusicLibrary() { }

    void addFolder(QString path);
};

#endif // MUSICLIBRARY_H
