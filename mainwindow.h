#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include "musiclibrary.h"
#include "commsocket.h"
#include "transport.h"

class CommAudio : public QMainWindow {
	Q_OBJECT

public:
	CommAudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CommAudio();

private:
    /** The program's gui. */
	Ui::CommAudioClass ui;

    /** Transport controls such as play, pause, and stop. */
    Transport* transport;

	CommSocket* testing;
    
    /** True if the program should multicast on clicking "Start Server". */
    bool multicastServer;

    /** Contains the songs that are on the user's local drive. */
    MusicLibrary* userSongs;

    /** True if the chat button should toggle chat. */
    bool stickyChat;

    /** True if currently chatting. */
    bool chatting;

    /** The open TCP socket either for the control channel. */
    CommSocket* ctlSock;

    /** The open UDP socket for streaming data. */
    CommSocket* strSock;

public:
    /**
     * Returns the currently selected song filename.
     *
     * @author Darryl Pogue
     * @return The filename of the selected song.
     */
    QString getSelectedSong();

public slots:
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
    void onCtlAccept();

    /**
     * Provides a notice that there is data available to be read on the control channel socket.
     *
     * @author Darryl Pogue
     */
    void onCtlReadReady();
};

#endif // MAINWINDOW_H
