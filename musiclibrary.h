#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qdir.h>
#include <qlistwidget.h>

class MusicLibrary : public QListWidget {
    Q_OBJECT

private:
    QList<QDir*> directories;
    QMap<QString, QString> songs;

public:
    MusicLibrary(QWidget* parent = 0);
    ~MusicLibrary();

    void addFolder(QString path);
    void addSongs(QDir* directory);
};

#endif // MUSICLIBRARY_H
