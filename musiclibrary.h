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

    /**
     * Returns a list of all the audio file names in all the loaded directories.
     *
     * @author Darryl Pogue
     * @return The list of song names.
     */
    QList<QString> getSongList() {
        return songs.keys();
    }

    /**
     * Returns the file path for a song.
     *
     * @author Darryl Pogue
     * @param song The name of the song.
     * @return The file path of the song.
     */
    QString getFilePath(QString song) {
        return songs[song];
    }

    /**
     * Returns the currently selected song filename.
     *
     * @author Darryl Pogue
     * @return The filename of the selected song.
     */
    QString getSelectedSong();

    /**
     * Returns the next song's filename, or an empty string if there is no next song.
     *
     * @author Darryl Pogue
     * @return The filename of the next song.
     */
    QString getNextSong();

    /**
     * Returns the previous song's filename, or an empty string if there is no previous song.
     *
     * @author Darryl Pogue
     * @return The filename of the previous song.
     */
    QString getPrevSong();

    void addFolder(QString path);
    void addSongs(QDir* directory);

    /**
     * Adds a list of songs to the music library.
     *
     * @author Darryl Pogue
     * @param songlist The list of songs to add to the library.
     */
    void addSongs(QList<QString> songlist);

signals:
    void signalSongDoubleClicked(QString songName);

public slots:
    void onItemDoubleClicked(QListWidgetItem* songListing);
};

#endif // MUSICLIBRARY_H
