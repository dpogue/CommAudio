#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <qlistwidget.h>
#include <qdir.h>
#include <qlist.h>
#include <qmap.h>
#include <qstring.h>

class MusicLibrary : public QListWidget {
    Q_OBJECT

private:
    /** A list of all of the directories to look for audio files in. */ 
    QStringList directories;

    /** All of the songs in the library. */
    QMap<QString, QString> songs;

    /** Keeps track of which songs have already been played (shuffle mode). */
    QSet<int> playedSongs;

public:
    /**
     * Constructor.
     *
     * @author Dean Morin
     * @param parent The QWidget that is the parent of this QWidget.
     */
    MusicLibrary(QWidget* parent = 0);

    /**
     * Returns the list of directories in the library.
     *
     * @author Dean Morin
     * @return The list of directories.
     */
    QStringList getDirectories() {
        return directories;
    }

    /**
     * Sets the list of directories. All songs in these directories are added
     * to the library.
     *
     * @author Dean Morin
     * @param dir The list of directories.
     */
    void setDirectories(QStringList dir);

    /**
     * Returns a list of all the audio file names in all the loaded
     * directories.
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
     * Returns the currently selected song name.
     *
     * @author Dean Morin
     * @return The name of the selected song.
     */
    QString getSelectedSongName();
    
    /**
     * Returns the next song's filename, or an empty string if there is no next
     * song.
     *
     * @author Darryl Pogue, Dean Morin
     * @param loop True if loop mode is on, and the playlist should start over
     * once it's finished.
     * @return The filename of the next song.
     */
    QString getNextSong(bool loop);

    /**
     * Returns the previous song's filename, or an empty string if there is no
     * previous song.
     *
     * @author Darryl Pogue, Dean Morin
     * @param loop True if loop mode is on, and the playlist should start over
     * once it's finished.
     * @return The filename of the previous song.
     */
    QString getPrevSong(bool loop);

    /**
     * Returns a random song's filename.
     *
     * @author Dean Morin
     * @param loop True if loop mode is on, and the playlist should start over
     * once it's finished.
     * @return The filename of the random song.
     */
    QString getRandSong(bool loop);
    
    /**
     * Clears any song selections in the list. 
     *
     * @author Dean Morin
     */
    void clearSelectedItems();

    /**
     * Add a folder to the library's folder list. This function calls
     * addSongs().
     *
     * @author Dean Morin
     * @param path The path to the folder being added.
     */
    void addFolder(QString path);

    /**
     * Add the songs in a folder to the library.
     *
     * @author Dean Morin
     * @param path The folder to add the songs from.
     */
    void addSongs(QDir* directory);

    /**
     * Adds a list of songs to the music library.
     *
     * @author Darryl Pogue
     * @param songlist The list of songs to add to the library.
     */
    void addSongs(QList<QString> songlist);

    /**
     * Adds a single song to the music library.
     *
     * @author Darryl Pogue
     * @param name The song name.
     * @param path The path to the song file.
     */
    void addSong(QString name, QString path);

signals:
    /**
     * Informs the transport object that a new song should be played.
     *
     * @author Dean Morin
     * @param songName The full path of the song.
     */
    void signalSongDoubleClicked(QString songName);

public slots:
    /**
     * Receives signals indicating that an item in the library has been
     * double-clicked on, and should be played.
     *
     * @author Dean Morin
     * @param songListing The name of the song that was clicked on.
     */
    void onItemDoubleClicked(QListWidgetItem* songListing);
};

#endif // MUSICLIBRARY_H
