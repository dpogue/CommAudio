#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

class CommAudio : public QMainWindow {
	Q_OBJECT

public:
	CommAudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CommAudio();

private:
    /** The program's gui. */
	Ui::CommAudioClass ui;

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
     * Attempts to connect to the server specified in the server group box. 
     *
     * @author Dean Morin
     */
    void onConnectClicked();

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

    void onCtlRead();
};

#endif // MAINWINDOW_H
