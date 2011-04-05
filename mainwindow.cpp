#include <WinSock2.h>
#include <qdir.h>
#include <QKeyEvent>
#include "mainwindow.h"
#include "manager.h"
#include "connectdialog.h"
#include "defines.h"
#include "settingsdialog.h"
#include "spacebargrabber.h"
#include "stylesheet.h"
#include "transport.h"
#include <qlayout.h>
#include <qsettings.h>

CommAudio::CommAudio(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), conn(NULL), stickyChat(false), 
      chatting(false), muted(false) 
{	
	ui.setupUi(this);
    this->setStyleSheet(StyleSheet::commAudio());
    this->setFixedSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    this->setFocus();

    userSongs = new MusicLibrary();
    settingsDialog = new SettingsDialog(this);
    connectDialog = new ConnectDialog(this);
    transport = new Transport(&ui, this);
    spacebarGrabber = new SpacebarGrabber(&ui);
    this->installEventFilter(spacebarGrabber);

    connect(ui.volumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onVolumeMoved(int)));
    connect(ui.mutePushButton, SIGNAL(clicked()),
            this, SLOT(onMuteClicked()));
    connect(ui.chatPushButton, SIGNAL(pressed()),
            this, SLOT(onChatPressed()));
    connect(ui.chatPushButton, SIGNAL(released()),
            this, SLOT(onChatReleased()));
    connect(ui.connectionPushButton, SIGNAL(pressed()),
            this, SLOT(onConnectionPressed()));
    connect(ui.settingsPushButton, SIGNAL(pressed()),
            this, SLOT(onSettingsPressed()));
	connect(AudioManager::instance(), SIGNAL(finished()),
            this, SLOT(playFinished()));
    
    QSettings settings;

    ui.volumeSlider->setMinimum(0);
    ui.volumeSlider->setMaximum(100);
    int volume = settings.value("volume", 80).toInt();
    ui.volumeSlider->setValue(volume);
    onVolumeMoved(volume);
    ui.downloadProgressBar->hide();
    ui.stopPushButton->raise();
    ui.playPushButton->raise();

    QBoxLayout* hl = new QBoxLayout(QBoxLayout::TopToBottom, ui.localTab);
    hl->setMargin(0);
    hl->addWidget(userSongs);
    connect(userSongs, SIGNAL(signalSongDoubleClicked(QString)),
            transport, SLOT(onSongDoubleClicked(QString)));

    ui.fileTabWidget->setTabEnabled(1, false);
    QBoxLayout* hr = new QBoxLayout(QBoxLayout::TopToBottom, ui.peerTab);
    hr->setMargin(0);
    remoteSongs = new MusicLibrary();
    hr->addWidget(remoteSongs);

	AudioManager::instance()->startCapture();
	AudioManager::instance()->playStream();
}

CommAudio::~CommAudio() { 
    QSettings settings;

    AudioManager::instance()->shutdown();    
    delete spacebarGrabber;
    delete transport;
    delete connectDialog;
    delete settingsDialog;
    settings.setValue("volume", ui.volumeSlider->value());
}

void CommAudio::keyPressEvent(QKeyEvent* keyEvent) {
    if (keyEvent->isAutoRepeat()) {
        return;
    }

    switch (keyEvent->key()) {
        case Qt::Key_C:
            if (stickyChat) {
                ui.chatPushButton->animateClick();
            } else {
                ui.chatPushButton->setDown(true);
                onChatPressed();
            }
            return;
    }
    QMainWindow::keyPressEvent(keyEvent);
}

void CommAudio::keyReleaseEvent(QKeyEvent* keyEvent) {
    if (keyEvent->isAutoRepeat()) {
        return;
    }

    switch (keyEvent->key()) {
        case Qt::Key_C:
            ui.chatPushButton->setDown(false);
            onChatReleased();
            return;
        default:
            QMainWindow::keyReleaseEvent(keyEvent);
    }
}

void CommAudio::addRemoteSongs(QList<QString> songs) {
    remoteSongs->addSongs(songs);
}

void CommAudio::onVolumeMoved(int volume) {
    AudioManager::setGain(volume / 100.0);
    if (muted) {
        onMuteClicked();
    }
}

void CommAudio::onMuteClicked() {
    muted = !muted;
    if (muted) {
        AudioManager::setGain(0);
        ui.mutePushButton->setIcon(QIcon(ICON_MUTE));
        if (transport->getPlayingState() != PAUSED) {
            ui.statusLabel->setText("Mute");
        }
    } else {
        AudioManager::setGain(ui.volumeSlider->value() / 100.0);
        ui.mutePushButton->setIcon(QIcon(ICON_UNMUTE));
        if (transport->getPlayingState() != PAUSED) {
            ui.statusLabel->setText("");
        }
    }
}

void CommAudio::connectToServer(QString host, int port) {
	
	conn = new Connection(this, host, TCP, port);
	conn->start();
    connect(remoteSongs, SIGNAL(signalSongDoubleClicked(QString)),
            conn, SLOT(requestForFile(QString)));

    connect(this, SIGNAL(gotDisconnected()), this, SLOT(disconnectFromServer()));
    connect(conn, SIGNAL(joinedMulticast()), this, SLOT(onJoiningMulticastSession()));
    connect(conn, SIGNAL(leftMulticast()), this, SLOT(onQuittingMulticastSession()));

    ui.fileTabWidget->setTabEnabled(1, true);
}

void CommAudio::disconnectFromServer() {
	conn->closeConnection();
    disconnect(this, SIGNAL(gotDisconnected()), this, SLOT(disconnectFromServer()));

    connectDialog->onDisconnectClicked();

    disconnect(remoteSongs, SIGNAL(signalSongDoubleClicked(QString)),
            conn, SLOT(requestForFile(QString)));
    remoteSongs->clear();
	ui.fileTabWidget->setTabEnabled(1, false);
    ui.fileTabWidget->setCurrentIndex(0);
    delete conn;
}

void CommAudio::startServer(int port) {
	
	conn = new Connection(this, TCP, port, connectDialog->getMulticastServer());
	conn->start();
    connect(remoteSongs, SIGNAL(signalSongDoubleClicked(QString)),
            conn, SLOT(requestForFile(QString)));

    connect(this, SIGNAL(gotDisconnected()), this, SLOT(stopServer()));

    ui.fileTabWidget->setTabEnabled(1, true);
}

void CommAudio::stopServer() {
	conn->closeConnection();
    disconnect(this, SIGNAL(gotDisconnected()), this, SLOT(stopServer()));

    connectDialog->onStopServerClicked();

    disconnect(remoteSongs, SIGNAL(signalSongDoubleClicked(QString)),
            conn, SLOT(requestForFile(QString)));
    remoteSongs->clear();
    ui.fileTabWidget->setTabEnabled(1, false);
    ui.fileTabWidget->setCurrentIndex(0);
}

void CommAudio::onChatPressed() {
    if (!stickyChat) {
        chatting = true;
        ui.chatPushButton->setIcon(QIcon(ICON_CHATTING));
		AudioManager::instance()->toggleCapturePause();
        return;
    }

    chatting = !chatting;
    QString icon = (chatting) ? ICON_CHATTING : ICON_CHAT;
    ui.chatPushButton->setIcon(QIcon(icon));
    AudioManager::instance()->toggleCapturePause();
}

void CommAudio::onChatReleased() {
    if (!stickyChat) {
        chatting = false;
        ui.chatPushButton->setIcon(QIcon(ICON_CHAT));
		AudioManager::instance()->toggleCapturePause();
	}
}

void CommAudio::onConnectionPressed() {
    connectDialog->updateFields(settingsDialog->getUseLastConnSettings());
    connectDialog->exec();
}

void CommAudio::onSettingsPressed() {
    settingsDialog->exec();
}

void CommAudio::playFinished() {
    transport->onSongFinished();
}

void CommAudio::onJoiningMulticastSession() {
    ui.previousPushButton->setDisabled(true);
    ui.nextPushButton->setDisabled(true);
    ui.playPushButton->setDisabled(true);
    ui.chatPushButton->setDisabled(true);
}

void CommAudio::onQuittingMulticastSession() {
    ui.previousPushButton->setEnabled(true);
    ui.nextPushButton->setEnabled(true);
    ui.playPushButton->setEnabled(true);
    ui.chatPushButton->setEnabled(true);
}

void CommAudio::onStartingMulticastSession() {
    ui.previousPushButton->setDisabled(true);
    ui.nextPushButton->setDisabled(true);
    ui.playPushButton->setDisabled(true);
    ui.chatPushButton->setDisabled(true);
    ui.stopPushButton->setDisabled(true);
}

void CommAudio::onStoppingMulticastSession() {
    ui.previousPushButton->setEnabled(true);
    ui.nextPushButton->setEnabled(true);
    ui.playPushButton->setEnabled(true);
    ui.chatPushButton->setEnabled(true);
    ui.stopPushButton->setEnabled(true);
}

void CommAudio::changeDisplayedSong() { 
    QString sn = userSongs->getSelectedSongName();
    ui.currentSongLabel->setText(sn);
    if (conn != NULL) {
        conn->sendSongName(sn);
    }
}

void CommAudio::changeDisplayedSong(QString songName) { 
    ui.currentSongLabel->setText(songName);
}
