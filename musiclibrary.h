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
};

#endif // MUSICLIBRARY_H
