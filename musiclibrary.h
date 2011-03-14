#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <list>
#include <map>
#include <qstring.h>
#include <qdir.h>

class MusicLibrary {

private:
    std::list<QDir*> directories;
    std::map<QString, QString> songs;

public:
    MusicLibrary();
    ~MusicLibrary();

    void addFolder(QString path);
    void addSongs(QDir* directory);
};

#endif // MUSICLIBRARY_H
