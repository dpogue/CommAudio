#include "musiclibrary.h"

MusicLibrary::MusicLibrary(QWidget* parent) : QListWidget(parent) {
    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
}

MusicLibrary::~MusicLibrary() {
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

QString MusicLibrary::getNextSong(bool loop) {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return getSelectedSong();
    }

    int row = this->row(sel.at(0));
    if (++row >= count()) {
        QList<QListWidgetItem*>::iterator it;
        for (it = sel.begin(); it != sel.end(); ++it) {
            setItemSelected(*it, false);
        }
        return "";
    }

    this->item(row)->setSelected(true);
    return songs[this->item(row)->text()];
}

QString MusicLibrary::getPrevSong(bool loop) {
    QList<QListWidgetItem*> sel = this->selectedItems();
    if (sel.size() == 0) {
        return getSelectedSong();
    }

    int row = this->row(sel.at(0));
    if (--row < 0) {
        QList<QListWidgetItem*>::iterator it;
        for (it = sel.begin(); it != sel.end(); ++it) {
            setItemSelected(*it, false);
        }
        return "";
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

void MusicLibrary::addSongs(QList<QString> songlist) {
    for (int i = 0; i < songlist.size(); i++) {
        songs.insert(songlist[i], songlist[i]);
        addItem(songlist[i]);
    }
}

void MusicLibrary::addSong(QString name, QString path) {
    songs.insert(name, path);
    addItem(name);
}

void MusicLibrary::onItemDoubleClicked(QListWidgetItem* songListing) {
    emit signalSongDoubleClicked(songs[songListing->text()]);
}
