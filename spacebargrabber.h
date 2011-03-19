#ifndef SPACEBARGRABBER_H
#define SPACEBARGRABBER_H

#include <QObject>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include "ui_mainwindow.h"

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
     * @param ui The main window's gui
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
