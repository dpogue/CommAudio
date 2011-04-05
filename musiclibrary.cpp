#include "musiclibrary.h"
#include <QMessageBox>

MusicLibrary::MusicLibrary(QWidget* parent) : QListWidget(parent) {
    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
}

void MusicLibrary::setDirectories(QStringList dir) {
    directories = dir;
    QStringList::iterator it;
    
    // remove all songs, before adding the current list
    this->clear();

    for (it = dir.begin(); it != dir.end(); ++it) {
        addSongs(&QDir(*it));
    }
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

QString MusicLibrary::getSelectedSongName() {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        if (this->count() == 0) {
            return "";
        }
        this->item(0)->setSelected(true);
        sel.append(this->item(0));
    }

    return sel.at(0)->text();
}

QString MusicLibrary::getNextSong(bool loop) {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return getSelectedSong();
    }

    int row = this->row(sel.at(0));
    if (++row >= count()) {
        if (loop) {
            row = 0;
        } else {
            return "";
        }
    }
    this->item(row)->setSelected(true);
    return songs[this->item(row)->text()];
}

QString MusicLibrary::getPrevSong(bool loop) {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return "";
    }

    int row = this->row(sel.at(0));
    if (--row < 0) {
        if (loop) {
            row = this->count() - 1;
        } else {
            return "";
        }
    }
    this->item(row)->setSelected(true);
    return songs[this->item(row)->text()];
}

QString MusicLibrary::getRandSong(bool loop) {

    // check if all songs have been played
    if (playedSongs.size() == this->count()) {
        playedSongs.clear();
        if (!loop) {
            return "";
        }
    }
 
    int nextSong = qrand() % this->count();
    while (playedSongs.contains(nextSong)) {
        nextSong = qrand() % this->count();
    }
    playedSongs.insert(nextSong);

    this->item(nextSong)->setSelected(true);
    return songs[this->item(nextSong)->text()];
}

void MusicLibrary::clearSelectedItems() {
    QList<QListWidgetItem*> sel = this->selectedItems();
    QList<QListWidgetItem*>::iterator it;
    
    for (it = sel.begin(); it != sel.end(); ++it) {
        setItemSelected(*it, false);
    }
}

void MusicLibrary::addFolder(QString path) {
    QDir* directory = new QDir(path);
    if (!directory->makeAbsolute()) {
        qDebug("MusicLibrary::addFolder(); could not make path absolute");
    }
    if (directories.contains(directory->absolutePath())) {
        QMessageBox(QMessageBox::Warning, "Duplicate Folder",
                "This folder has already been added to the library.").exec();
        return;
    }
    directories.push_back(directory->absolutePath());
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

void MusicLibrary::addSongs(QList<QString> songlist) {
    for (int i = 0; i < songlist.size(); i++) {
        songs.insert(songlist[i], songlist[i]);
        addItem(songlist[i]);
    }
}

void MusicLibrary::addSong(QString name, QString path) {
    if (songs.contains(name)) {
        return;
    }
    songs.insert(name, path);
    addItem(name);
}

void MusicLibrary::onItemDoubleClicked(QListWidgetItem* songListing) {
    emit signalSongDoubleClicked(songs[songListing->text()]);
}
