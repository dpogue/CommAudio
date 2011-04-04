#ifndef SPACEBARGRABBER_H
#define SPACEBARGRABBER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QPushButton>
#include "ui_mainwindow.h"

/**
 * Changes the default behaviour of the spacebar, in order to meet our twisted
 * and nefarious purposes.
 */
class SpacebarGrabber : public QObject {
    Q_OBJECT

private:
    /** The button to redirect all spacebar presses to. */
    QPushButton* playButton;

public:
    /**
     * Constructor.
     *
     * @author Dean Morin
     * @param ui The main window's gui.
     */
    SpacebarGrabber(Ui::CommAudioClass* ui);

    /**
     * Destructor.
     *
     * @author Dean Morin
     */
    ~SpacebarGrabber();

protected:
    /**
     * Intercepts the spacebar, so that it can be passed to the spaceButton.
     *
     * @author Dean Morin
     * @param obj The intended recipient of the event.
     * @param event The event that occured.
     * @return True if the event was filtered.
     */
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // SPACEBARGRABBER_H
