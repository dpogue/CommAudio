#include <WinSock2.h>
#include <qdir.h>
#include <QKeyEvent>
#include "mainwindow.h"
#include "manager.h"
#include "connectdialog.h"
#include "defines.h"
#include "spacebargrabber.h"
#include "stylesheet.h"
#include "transport.h"
#include <qlayout.h>

CommAudio::CommAudio(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),server(NULL),client(NULL) {
	
	ui.setupUi(this);
    this->setStyleSheet(StyleSheet::commAudio());
    this->setFixedSize(439, 658);
    this->setFocus();
	
    transport = new Transport(&ui, this);
    spacebarGrabber = new SpacebarGrabber(&ui);
    this->installEventFilter(spacebarGrabber);
    connectDialog = new ConnectDialog(this);

    connect(ui.volumeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(onVolumeMoved(int)));
    connect(ui.chatPushButton, SIGNAL(pressed()),
            this, SLOT(onChatPressed()));
    connect(ui.chatPushButton, SIGNAL(released()),
            this, SLOT(onChatReleased()));
    connect(ui.connectionPushButton, SIGNAL(pressed()),
            this, SLOT(onConnectionPressed()));

    ui.volumeSlider->setMinimum(0);
    ui.volumeSlider->setMaximum(100);
    ui.volumeSlider->setValue(50);
    onVolumeMoved(50);
    chatting = false;
    stickyChat = false;

    //TODO: move to settings
    if(!QDir("music").exists()) {
        QDir().mkdir("music");
    }

    QBoxLayout* hl = new QBoxLayout(QBoxLayout::TopToBottom, ui.localTab);
    hl->setMargin(0);
    userSongs = new MusicLibrary();
    userSongs->addFolder("music/");
    hl->addWidget(userSongs);
    connect(userSongs, SIGNAL(signalSongDoubleClicked(QString)),
            transport, SLOT(onSongDoubleClicked(QString)));

    ui.fileTabWidget->setTabEnabled(1, false);
    ui.fileTabWidget->setTabEnabled(2, false);
    QBoxLayout* hr = new QBoxLayout(QBoxLayout::TopToBottom, ui.peerTab);
    hr->setMargin(0);
    remoteSongs = new MusicLibrary();
    hr->addWidget(remoteSongs);

	AudioManager::instance()->startCapture();
	AudioManager::instance()->playStream();
}

CommAudio::~CommAudio() { 

    AudioManager::instance()->shutdown();    
    delete spacebarGrabber;
    delete transport;
    delete connectDialog;
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
}

void CommAudio::connectToServer(QString host, int port) {
	
	client = new Connection(this, host, TCP, port);
	client->start();

    ui.fileTabWidget->setTabEnabled(1, true);
}

void CommAudio::startServer(int port) {
	
	server = new Connection(this, TCP, port);
	server->start();

    ui.fileTabWidget->setTabEnabled(1, true);
}

void CommAudio::stopServer() {
    ui.fileTabWidget->setTabEnabled(1, false);
}

void CommAudio::onMulticastStateChanged(bool checked) {
    multicastServer = checked;
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
}

void CommAudio::onChatReleased() {
    if (!stickyChat) {
        chatting = false;
        ui.chatPushButton->setIcon(QIcon(ICON_CHAT));
		AudioManager::instance()->toggleCapturePause();
	}
}

void CommAudio::onConnectionPressed() {
    connectDialog->exec();
}
