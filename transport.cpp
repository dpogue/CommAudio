#include "transport.h"
#include "defines.h"
#include "manager.h"

Transport::Transport(Ui::CommAudioClass* gui)
    : ui(gui)
{   
    connect(ui->playPushButton, SIGNAL(clicked()), 
            this, SLOT(onPlayClicked()));
    connect(ui->stopPushButton, SIGNAL(clicked()), 
            this, SLOT(onStopClicked()));
    connect(ui->previousPushButton, SIGNAL(clicked()), 
            this, SLOT(onPreviousClicked()));
    connect(ui->nextPushButton, SIGNAL(clicked()), 
            this, SLOT(onNextClicked()));
    
    playingState = STOPPED;
}

void Transport::onPlayClicked() {

    QString fileName = "music/3.ogg";
    
    switch (playingState) {

        case STOPPED:
            AudioManager::instance()->playMusic(fileName);
            ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
            playingState = PLAYING;
            break;

        case PLAYING:
            AudioManager::instance()->togglePause();
            ui->playPushButton->setIcon(QIcon(ICON_PLAY));
            playingState = PAUSED;
            break;

        case PAUSED:
            AudioManager::instance()->togglePause();
            ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
            playingState = PLAYING;
            break;
    }
}

void Transport::onStopClicked() {
    
    if (playingState == PLAYING) {
        AudioManager::instance()->togglePause();
        ui->playPushButton->setIcon(QIcon(ICON_PLAY));
    }
    playingState = STOPPED;
}

void Transport::onPreviousClicked() {
    qDebug("Transport::onPreviousClicked()");
}

void Transport::onNextClicked() {
    qDebug("Transport::onNextClicked()");
}
