#include "transport.h"
#include "defines.h"
#include "mainwindow.h"
#include "manager.h"
#include <qmessagebox.h>
#include <qsettings.h>

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
    
    QSettings settings;
    qDebug("what gives");

    loop = !settings.value("loop", false).toBool();
    qDebug() << "entering: " << !loop;
    onLoopClicked();
    shuffle = !settings.value("shuffle", false).toBool();
    onShuffleClicked();
    playingState = STOPPED;
}
    
Transport::~Transport() {
    QSettings settings;

    settings.setValue("loop", loop);
    settings.setValue("shuffle", shuffle);
    qDebug() << "exiting: " << loop;
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
            ui->currentSongLabel->setText(((CommAudio*) parent())->
                    getUserSongs()->getSelectedSongName());
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
    ui->currentSongLabel->setText("");
    playingState = STOPPED;
}

void Transport::onPreviousClicked() {
    QString fileName = 
            ((CommAudio*) parent())->getUserSongs()->getPrevSong(loop);
    if (fileName.isEmpty()) {
        return;
    }

    AudioManager::instance()->playMusic(fileName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    ui->currentSongLabel->setText(
            ((CommAudio*) parent())->getUserSongs()->getSelectedSongName());
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
        if (playingState == STOPPED) {
            ui->currentSongLabel->setText("");
        }
        return;
    }

    AudioManager::instance()->playMusic(fileName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    ui->currentSongLabel->setText(
            ((CommAudio*) parent())->getUserSongs()->getSelectedSongName());
    playingState = PLAYING;
}

void Transport::onSongDoubleClicked(QString songName) {
    AudioManager::instance()->playMusic(songName);
    ui->playPushButton->setIcon(QIcon(ICON_PAUSE));
    ui->currentSongLabel->setText(
            ((CommAudio*) parent())->getUserSongs()->getSelectedSongName());
    playingState = PLAYING;
}

void Transport::onShuffleClicked() {
    shuffle = !shuffle;
    QString icon = (shuffle) ? ICON_SHUFFLE_ON : ICON_SHUFFLE_OFF;
    ui->shufflePushButton->setIcon(QIcon(icon));
}

void Transport::onLoopClicked() {
    loop = !loop;
    QString icon = (loop) ? ICON_LOOP_ON : ICON_LOOP_OFF;
    ui->loopPushButton->setIcon(QIcon(icon));
}

void Transport::onSongFinished() {
    ui->playPushButton->setIcon(QIcon(ICON_PLAY));
    playingState = STOPPED;
    onNextClicked();
    if (!loop) {
        ((CommAudio*) parent())->getUserSongs()->clearSelectedItems();
    }
}
