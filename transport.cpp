#include "transport.h"
#include "defines.h"
#include "mainwindow.h"
#include "manager.h"
#include <qmessagebox.h>

Transport::Transport(Ui::CommAudioClass* gui, QWidget* parent)
    : QObject(parent), ui(gui)
{   
    connect(ui->playPushButton, SIGNAL(clicked()), 
            this, SLOT(onPlayClicked()));
    connect(ui->stopPushButton, SIGNAL(clicked()), 
            this, SLOT(onStopClicked()));
    connect(ui->previousPushButton, SIGNAL(clicked()), 
            this, SLOT(onPreviousClicked()));
    connect(ui->nextPushButton, SIGNAL(clicked()), 
            this, SLOT(onNextClicked()));
    connect(ui->shufflePushButton, SIGNAL(clicked()), 
            this, SLOT(onShuffleClicked()));
    connect(ui->loopPushButton, SIGNAL(clicked()), 
            this, SLOT(onLoopClicked()));
    
    playingState = STOPPED;
    shuffle = false;
    loop = false;
}

void Transport::onPlayClicked() {

    QString fileName = ((CommAudio*)parent())->getSelectedSong();
    if (fileName.isEmpty()) {
        QMessageBox(QMessageBox::Warning, "No Music Files",
            "There were no music files found. Try adding a folder which contains audio files.").exec();
    }
    
    switch (playingState) {

        case STOPPED:
            AudioManager::instance()->playMusic(fileName);
            ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
            playingState = PLAYING;
            break;

        case PLAYING:
            AudioManager::instance()->togglePause();
            ui->playPushButton->setIcon(QIcon(ICON_PLAY));
            ui->statusLabel->setText("Paused");
            playingState = PAUSED;
            break;

        case PAUSED:
            AudioManager::instance()->togglePause();
            ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
            QString text = ((CommAudio*) parent())->getMuted() ? "Mute" : "";
            ui->statusLabel->setText(text);
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
    QString fileName = 
            ((CommAudio*) parent())->getUserSongs()->getPrevSong(loop);
    if (fileName.isEmpty()) {
        onStopClicked();
        return;
    }

    AudioManager::instance()->playMusic(fileName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    ui->currentSongLabel->setText(fileName);
    playingState = PLAYING;
}

void Transport::onNextClicked() {
    QString fileName;

    if (shuffle) {
        fileName = ((CommAudio*) parent())->getUserSongs()->getRandSong(loop);
    } else {
        fileName = ((CommAudio*) parent())->getUserSongs()->getNextSong(loop);
    }
    if (fileName.isEmpty()) {
        onStopClicked();
        return;
    }

    AudioManager::instance()->playMusic(fileName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    playingState = PLAYING;
}

void Transport::onSongDoubleClicked(QString songName) {
    AudioManager::instance()->playMusic(songName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    playingState = PLAYING;
}

void Transport::onShuffleClicked() {
    shuffle = !shuffle;
}

void Transport::onLoopClicked() {
    loop = !loop;
}

void Transport::onSongFinished() {
    ui->playPushButton->setIcon(QIcon(ICON_PLAY));
    playingState = STOPPED;
    onNextClicked();
}
