#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include "musiclibrary.h"
#include "commsocket.h"
#include "transport.h"
#include "connection.h"
class ConnectDialog;
class SpacebarGrabber;
class Transport;

class CommAudio : public QMainWindow {
	Q_OBJECT

public:
	CommAudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CommAudio();

private:
    /** The program's gui. */
	Ui::CommAudioClass ui;

	Connection* server;

	Connection* client;
    /** Transport controls such as play, pause, and stop. */
    Transport* transport;

    /** Encapsulates all of the connection info. */
    ConnectDialog* connectDialog;
    
    /** True if the program should multicast on clicking "Start Server". */
    bool multicastServer;

    /** Contains the songs that are on the user's local drive. */
    MusicLibrary* userSongs;

    /** Contains the songs that are on the remote drive. */
    MusicLibrary* remoteSongs;

    /** True if the chat button should toggle chat. */
    bool stickyChat;

    /** True if currently chatting. */
    bool chatting;

    /** Prevents buttons other that play from stealing the spacebar. */
    SpacebarGrabber* spacebarGrabber;

protected:
    /**
     * Handles keyboard presses.
     *
     * @author Dean Morin
     * @param keyEvent The triggering key event.
     */
    virtual void keyPressEvent(QKeyEvent* keyEvent);
    
    /**
     * Handles keyboard releases.
     *
     * @author Dean Morin
     * @param keyEvent The triggering key event.
     */
    virtual void keyReleaseEvent(QKeyEvent* keyEvent);

public:
    /**
     * Returns the currently selected song filename.
     *
     * @author Darryl Pogue
     * @return The filename of the selected song.
     */
    QString getSelectedSong() {
        return userSongs->getSelectedSong();
    }

    /**
     * Returns the next song's filename, or an empty string if there is no next song.
     *
     * @author Darryl Pogue
     * @return The filename of the next song.
     */
    QString getNextSong() {
        return userSongs->getNextSong();
    }

    /**
     * Returns the previous song's filename, or an empty string if there is no previous song.
     *
     * @author Darryl Pogue
     * @return The filename of the previous song.
     */
    QString getPrevSong() {
        return userSongs->getPrevSong();
    }

    /**
     * Returns the list of songs in the local library.
     *
     * @author Darryl Pogue
     * @return The list of songs.
     */
    QList<QString> getSongList() {
        return userSongs->getSongList();
    }

    QString getSongFilePath(QString song) {
        return userSongs->getFilePath(song);
    }

    void addRemoteSongs(QList<QString> songs);

public slots:
    /**
     * Adjusts the volume based on the volume slider.
     *
     * @author Dean Morin
     * @param volume The new value for volume.
     */
    void onVolumeMoved(int volume);

    /**
     * Attempts to connect to the server specified in the server group box. 
     *
     * @author Dean Morin
     */
    void onConnectClicked();

    /**
     * Starts listening for clients. 
     *
     * @author Dean Morin.
     */
    void onStartServerClicked();

    /**
     * Stops the server socket from listening for incoming connections.
     *
     * @author Darryl Pogue
     */
    void onStopServerClicked();

    /**
     * Starts transmitting voice data, or toggles the transmission on/off if
     * "sticky" mode is enabledd.
     *
     * @author Dean Morin
     */
    void onChatPressed();

    /**
     * Stops transmitting voice data (ignored if "sticky" mode is enabled).
     *
     * @author Dean Morin
     */
    void onChatReleased();

    /**
     * This server will multicast if this is selected.
     *
     * @author Dean Morin
     * @param state The state of multicastCheckBox.
     */
    void onMulticastStateChanged(int state);
};

#endif // MAINWINDOW_H
