#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include "musiclibrary.h"
#include "commsocket.h"

class CommAudio : public QMainWindow {
	Q_OBJECT

public:
	CommAudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CommAudio();

private:
    /** The program's gui. */
	Ui::CommAudioClass ui;
	CommSocket* testing;
    
    /** The state of the current song (stopped, playing, or paused). */
    int playingState;

    /** True if the program should multicast on clicking "Start Server". */
    bool multicastServer;

    /** Contains the songs that are on the user's local drive. */
    MusicLibrary userSongs;

    /** The open TCP socket either for the control channel. */
    CommSocket* ctlSock;

    /** The open UDP socket for streaming data. */
    CommSocket* strSock;

public slots:
    /**
     * Plays the song appropriate to the state of the program (selected song if
     * in regular mode, whatever's being transmitted from the peer is a
     * multicast server).
     *
     * @author Dean Morin
     */
    void onPlayClicked();

    /**
     * Stops the currently playing song. 
     *
     * @author Dean Morin
     */
    void CommAudio::onStopClicked() {
    
    /**
     * Stops
    void onStopClicked();

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

	void onCtlWrite();

    /**
     * Provides a notice that there is data available to be read on the control channel socket.
     *
     * @author Darryl Pogue
     */
    void onCtlReadReady();
};

#endif // MAINWINDOW_H
