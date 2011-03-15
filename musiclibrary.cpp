#include "musiclibrary.h"

MusicLibrary::MusicLibrary(QWidget* parent) : QListWidget(parent) {
}

MusicLibrary::~MusicLibrary() {
    // delete list contents
}

QString MusicLibrary::getSelectedSong() {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        if (this->count() == 0) {
            return "";
        }
        this->item(0)->setSelected(true);
        sel.append(this->item(0));
    }

    return songs[sel.at(0)->text()];
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
    QStringList filters;
    filters << "*.wav" << "*.ogg";
    QStringList newSongs = directory->entryList(filters, QDir::Readable | QDir::Files, QDir::Name);
    
    while (!newSongs.isEmpty()) {
        QString fileName = newSongs.takeFirst();
        songs.insert(fileName, directory->absoluteFilePath(fileName));
        addItem(fileName);
    }
}
