#include "musiclibrary.h"

MusicLibrary::MusicLibrary() {
}

MusicLibrary::~MusicLibrary() {
    // delete list contents
}

void MusicLibrary::addFolder(QString path) {
    QDir* directory = new QDir(path);
    if (!directory->makeAbsolute()) {
        qDebug("MusicLibrary::addFolder(); could not make path absolute");
    }
    directories.push_back(directory);
    addSongs(directory);
}

void MusicLibrary::addSongs(QDir* directory) {
    QStringList newSongs = directory->entryList();
    
    newSongs.sort();
    while (!newSongs.isEmpty()) {
        QString fileName = newSongs.takeFirst();
        songs.insert(fileName, fileName);
    }
}
