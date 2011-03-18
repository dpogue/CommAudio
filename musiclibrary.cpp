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

QString MusicLibrary::getNextSong() {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return getSelectedSong();
    }

    int row = this->row(sel.at(0));
    if (++row >= count()) {
        return "";
    }

    this->item(row)->setSelected(true);
    return songs[this->item(row)->text()];
}

QString MusicLibrary::getPrevSong() {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return getSelectedSong();
    }

    int row = this->row(sel.at(0));
    if (--row < 0) {
        return "";
    }

    this->item(row)->setSelected(true);
    return songs[this->item(row)->text()];
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
    filters << "*.wav" << "*.ogg" << "*.oga";
    QStringList newSongs = directory->entryList(filters, QDir::Readable | QDir::Files, QDir::Name);
    
    while (!newSongs.isEmpty()) {
        QString fileName = newSongs.takeFirst();
        songs.insert(fileName, directory->absoluteFilePath(fileName));
        addItem(fileName);
    }
}

void MusicLibrary::onSongDoubleClicked(QListWidgetItem* songListing) {
    emit signalSongDoubleClicked(songListing->text());
}
