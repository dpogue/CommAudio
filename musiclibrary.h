#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qdir.h>

class MusicLibrary {

private:
    QList<QDir*> directories;
    QMap<QString, QString> songs;

public:
    MusicLibrary();
    ~MusicLibrary();

    void addFolder(QString path);
    void addSongs(QDir* directory);
};

#endif // MUSICLIBRARY_H
