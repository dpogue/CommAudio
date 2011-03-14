#include "musiclibrary.h"

MusicLibrary::MusicLibrary() {
}

MusicLibrary::~MusicLibrary() {
    // delete list
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
    //
}
