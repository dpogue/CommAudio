#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include "musiclibrary.h"
#include "commsocket.h"
#include "transport.h"
#include "connection.h"
class ConnectDialog;
class SettingsDialog;
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

	Connection* conn;

	//Connection* client;
    
    /** True if the program should multicast on clicking "Start Server". */
    bool multicastServer;

    /** Transport controls such as play, pause, and stop. */
    Transport* transport;

    /** The connect dialog window. */
    ConnectDialog* connectDialog;
    
    /** The settings dialog window. */
    SettingsDialog* settingsDialog;

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
     * @return The filename eof the next song.
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

    /**
     * Returns whether or not the chat button is in toggle mode.
     *
     * @author Dean Morin
     * @return True if the chat button is in toggle mode.
     */
    bool getStickyChat() {
        return stickyChat;
    }

    /**
     * Set whether or not the chat button is in toggle mode.
     *
     * @author Dean Morin
     * @param Whether the chat button should be in toggle mode, or held down to
     * use.
     */
    void setStickyChat(bool sticky) {
        stickyChat = sticky;
    }

public slots:
    /**
     * Adjusts the volume based on the volume slider.
     *
     * @author Dean Morin
     * @param volume The new value for volume.
     */
    void onVolumeMoved(int volume);
    //TODO: move out of slots
    /**
     * Attempts to connect to the server specified in the server group box. 
     *
     * @author Dean Morin
     */
    void connectToServer(QString host, int port);

	void disconnectFromServer();

    /**
     * Starts listening for clients. 
     *
     * @author Dean Morin.
     */
    void startServer(int port);

    /**
     * Stops the server socket from listening for incoming connections.
     *
     * @author Darryl Pogue
     */
    void stopServer();

    /**
     * This server will multicast if this is selected.
     *
     * @author Dean Morin
     * @param state The state of multicastCheckBox.
     */
    void onMulticastStateChanged(bool checked);

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
     * Opens the connect dialog.
     *
     * @author Dean Morin
     */
    void onConnectionPressed();

    /**
     * Opens the settings dialog.
     *
     * @author Dean Morin
     */
    void onSettingsPressed();

	/**
     * Tells the dialog music track finished
     *
     * @author Terence Stenvold
     */
    void playFinished();
};

#endif // MAINWINDOW_H
