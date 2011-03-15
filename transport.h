#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <qobject.h>
#include "ui_mainwindow.h"

class Transport : public QObject {
    Q_OBJECT

private:
    /** The program's gui. */
	Ui::CommAudioClass* ui;

    /** The state of the current song (stopped, playing, or paused). */
    int playingState;

public:
    Transport(Ui::CommAudioClass* gui, QWidget* parent = 0);
    ~Transport() { }
   
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
    void onStopClicked();

    /**
     * Selects the previous song in the list. 
     *
     * @author Dean Morin
     */
    void onPreviousClicked();

    /**
     * Selects the next song in the list.
     *
     * @author Dean Morin
     */
    void onNextClicked();
};

#endif // TRANSPORT_H
