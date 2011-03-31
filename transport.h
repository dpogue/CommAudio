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

	/** True if the order of songs played should be random. */
	bool shuffle;

	/** True if it should continue playing songs once the end of the list is 
	 *  reached. */
	int loop;

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

    void onSongDoubleClicked(QString songName);

    /**
     * Toggles shuffle mode.
     *
     * @author Dean Morin
     */
    void onShuffleClicked();

    /**
     * Toggles loop mode.
     *
     * @author Dean Morin
     */
    void onLoopClicked();
};

#endif // TRANSPORT_H
