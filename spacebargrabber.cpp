#include "spacebargrabber.h"
#include "mainwindow.h"

SpacebarGrabber::SpacebarGrabber(Ui::CommAudioClass* ui) 
    : playButton(ui->playPushButton) 
{ 
    ui->previousPushButton->installEventFilter(this);
    ui->nextPushButton->installEventFilter(this);
    ui->settingsPushButton->installEventFilter(this);
    ui->stopPushButton->installEventFilter(this);
    ui->connectionPushButton->installEventFilter(this);
    ui->chatPushButton->installEventFilter(this);
}

SpacebarGrabber::~SpacebarGrabber() { 
    delete playButton; 
}

bool SpacebarGrabber::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Space) {
            if (!keyEvent->isAutoRepeat()) {
                playButton->animateClick();
            }
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
