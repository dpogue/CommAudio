#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <qobject.h>
#include "ui_mainwindow.h"

/**
 * This class contains the player's transport controls. These include play, 
 * stop, previous, next, shuffle, and loop.
 */
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
	bool loop;

public:
    /**
     * Sets up the transport controls. The state of loop and shuffle are read
     * from the settings file.
     * 
     * @author Dean Morin
     * @param gui A pointer to the main window's gui, which contains all of the
     * transport gui items.
     * @param parent The parent of this QObject.
     */
    Transport(Ui::CommAudioClass* gui, QWidget* parent = 0);
    
    /**
     * Saves the current state of loop and shuffle to the settings file.
     *
     * @author Dean Morin
     */
    ~Transport();

    /**
     * Gets the current state of the player (playing, paused, or stopped).
     *
     * @author Dean Morin
     * @return The current player state.
     */
    int getPlayingState() {
        return playingState;
    }

signals:
    /**
     * Emitted whenever a button is clicked that changes the song.
     *
     * @author Dean Morin
     */
    void songChanged();

    /**
     * Emitted whenever the stop button is clicked.
     *
     * @author Darryl Pogue
     */
    void songStopped();
   
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

    /**
     * Responds to a song title being double-clicked by playing that song.
     *
     * @author Dean Morin
     * @param songName The name of the song that was double-clicked.
     */
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

    /**
     * Sets the state when a song finishes playing.
     *
     * @author Darryl Pogue
     */
    void onSongFinished();
};

#endif // TRANSPORT_H
