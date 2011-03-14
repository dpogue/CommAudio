#include "musiclibrary.h"

MusicLibrary::MusicLibrary() {
}

void MusicLibrary::addFolder(QString path) {
    directories.push_back(path);
}
