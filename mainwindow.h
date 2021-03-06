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

/**
 * The main window of the program. Contains all of the gui component logic
 * except for the transport controls.
 */
class CommAudio : public QMainWindow {
	Q_OBJECT

public:
	/**
     * Constructor.
     *
     * @author Dean Morin, Darryl Pogue
     * @param parent The QWidget that is the parent of this window.
     * @param flags Options for creating the window.
     */
    CommAudio(QWidget* parent = 0, Qt::WFlags flags = 0);

    /**
     * Destructor.
     *
     * @author Dean Morin
     */
	~CommAudio();

private:
    /** The program's gui. */
	Ui::CommAudioClass ui;

	Connection* conn;

	//Connection* client;
    
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

    /** True if the audio should be muted. */
    bool muted;

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
     * Returns the collection of local songs.
     *
     * @author Dean Morin
     * @return The collection of songs on the client's computer.
     */
    MusicLibrary* getUserSongs() {
        return userSongs;
    }
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
     * Returns the list of songs in the local library.
     *
     * @author Darryl Pogue
     * @return The list of songs.
     */
    QList<QString> getSongList() {
        return userSongs->getSongList();
    }

    /**
     * Returns the path for the given song name.
     *
     * @author Darryl Pogue
     * @param song The song name.
     * @return The path to the file.
     */
    QString getSongFilePath(QString song) {
        return userSongs->getFilePath(song);
    }

    /**
     * Adds a list of songs to the remote songs tab.
     *
     * @author Darryl Pogue
     * @param songs The list of remote songs.
     */
    void addRemoteSongs(QList<QString> songs);

    /**
     * Adds a song to the music library.
     *
     * @author Darryl Pogue
     * @param song The name of the song.
     * @param path The file path to the song.
     */
    void addSong(QString song, QString path) {
        userSongs->addSong(song, path);
    }

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

    /**
     * Informs the application that a socket connection has been disconnected in
     * an abnormal way.
     *
     * @author Darryl Pogue
     */
    void disconnected() {
        emit gotDisconnected();
    }

    /**
     * Gets a reference to the main window ui.
     *
     * @author Dean Morin
     * @return The main window ui.
     */
	Ui::CommAudioClass* getUi() {
		return &ui;
	}

    /**
     * Finds out if the audio is currently muted. 
     *
     * @author Dean Morin
     * @return True if the audio is currently muted.
     */
    bool getMuted() {
        return muted;
    }

signals:
    /**
     * Emitted when the connection is broken.
     *
     * @author Darryl Pogue
     */
    void gotDisconnected();

public slots:
    /**
     * Adjusts the volume based on the volume slider.
     *
     * @author Dean Morin
     * @param volume The new value for volume.
     */
    void onVolumeMoved(int volume);

    /**
     * Mutes the audio.
     *
     * @author Dean Morin
     */
    void onMuteClicked();

    /**
     * Attempts to connect to the server specified in the server group box. 
     *
     * @author Dean Morin
     */
    void connectToServer(QString host, int port);

    /**
     * Disconnects from a server and resets the application state.
     *
     * @author Darryl Pogue
     */
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

    /**
     * Disables elements of the gui that aren't needed when a multicast session
     * is joined.
     *
     * @author Dean Morin
     */
    void onJoiningMulticastSession();

    /**
     * Enables elements of the gui that were disabled when a multicast server
     * was joined.
     *
     * @author Dean Morin
     */
    void onQuittingMulticastSession();

    /**
     * Disables elements of the gui that aren't needed when a multicast server
     * is started.
     *
     * @author Dean Morin
     */
    void onStartingMulticastSession();

    /**
     * Enables elements of the gui that were disabled when the multicast server
     * is started.
     *
     * @author Dean Morin
     */
    void onStoppingMulticastSession();
    
    /**
     * Sets the song displayed in the top frame to the current song.
     *
     * @author Dean Morin
     */
    void changeDisplayedSong();
    
    /**
     * Sets the song displayed in the top frame to the current song.
     *
     * @author Dean Morin
     * @param songName The song to display.
     */
    void changeDisplayedSong(QString songName);

    /**
     * Clears the song displayed in the top frame.
     *
     * @author Darryl Pogue
     */
    void clearDisplayedSong();
};

#endif // MAINWINDOW_H
