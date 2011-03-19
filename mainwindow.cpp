#include <WinSock2.h>
#include <qdir.h>
#include <QKeyEvent>
#include "mainwindow.h"
#include "manager.h"
#include "defines.h"
#include "spacebargrabber.h"
#include "stylesheet.h"
#include "transport.h"
#include <qlayout.h>

CommAudio::CommAudio(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),server(NULL),client(NULL) {
	
	ui.setupUi(this);
    this->setStyleSheet(StyleSheet::commAudio());
	
    transport = new Transport(&ui, this);
    spacebarGrabber = new SpacebarGrabber(&ui);

    connect(ui.volumeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(onVolumeMoved(int)));
    connect(ui.connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    connect(ui.startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(ui.chatPushButton, SIGNAL(pressed()),
            this, SLOT(onChatPressed()));
    connect(ui.chatPushButton, SIGNAL(released()),
            this, SLOT(onChatReleased()));
    connect(ui.multicastCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onMulticastStateChanged(int)));

    multicastServer = ui.multicastCheckBox->isChecked();
    ui.volumeSlider->setMinimum(0);
    ui.volumeSlider->setMaximum(100);
    ui.volumeSlider->setValue(50);
    onVolumeMoved(50);
    chatting = false;
    stickyChat = true;

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
}

CommAudio::~CommAudio() { 

    AudioManager::instance()->shutdown();    
    delete spacebarGrabber;
    delete transport;
}

void CommAudio::keyPressEvent(QKeyEvent* keyEvent) {
    switch (keyEvent->key()) {
        case Qt::Key_Space:    
            ui.playPushButton->animateClick();
            return;
        case Qt::Key_C:
            onChatPressed();
            return;
    }
    QMainWindow::keyPressEvent(keyEvent);
}

void CommAudio::keyReleaseEvent(QKeyEvent* keyEvent) {
    switch (keyEvent->key()) {
        case Qt::Key_C:
            onChatReleased();
            return;
        default:
            QMainWindow::keyReleaseEvent(keyEvent);
    }
}

void CommAudio::onVolumeMoved(int volume) {
    AudioManager::setGain(volume / 100.0);
}

void CommAudio::onConnectClicked() {
    unsigned long ip = 0;
    unsigned int port = 0;
    bool validPort = false;
    
    if ((ip = inet_addr(ui.ipLineEdit->text().toAscii())) == INADDR_NONE) {
        ui.connectErrorLabel->
                setText("The ip address must be in the form x.x.x.x");
        ui.ipLineEdit->selectAll();
        return;
    }

    port = ui.portLineEdit->text().toUInt(&validPort);
    if (!validPort || port < 1024 || port > 65535) {
        ui.connectErrorLabel->
                setText("The port number must be between 1024 and 65535");
        ui.portLineEdit->selectAll();
        return;
    }

    ui.connectErrorLabel->clear();

    ui.connectPushButton->setDisabled(true);
    ui.startServerPushButton->setDisabled(true);

    ui.connectPushButton->setDisabled(false);
    ui.connectPushButton->setText("Disconnect");
	
	client = new Connection(ui.ipLineEdit->text(),TCP,port);
	client->start();
}

void CommAudio::onStartServerClicked() {
    unsigned int port = 0;
    bool validPort = false;

    // TODO: disable gui components

    port = ui.portLineEdit->text().toUInt(&validPort);
    if (!validPort || port < 1024 || port > 65535) {
        ui.connectErrorLabel->
                setText("The port number must be between 1024 and 65535");
        ui.portLineEdit->selectAll();
        return;
    }

    disconnect(ui.startServerPushButton, SIGNAL(clicked()),
                this, SLOT(onStartServerClicked()));

    ui.connectPushButton->setDisabled(true);
    ui.startServerPushButton->setText("Stop Server");
    connect(ui.startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStopServerClicked()));
	
	server = new Connection(TCP,port);
	server->start();
}

void CommAudio::onStopServerClicked() {
    disconnect(ui.startServerPushButton, SIGNAL(clicked()),
                this, SLOT(onStopServerClicked()));

    ui.connectPushButton->setDisabled(false);
    ui.startServerPushButton->setText("Start Server");
    connect(ui.startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
}

void CommAudio::onChatPressed() {
    if (!stickyChat) {
        chatting = true;
        ui.chatPushButton->setIcon(QIcon(ICON_CHATTING));
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
    }
 }

void CommAudio::onMulticastStateChanged(int state) {
    multicastServer = ui.multicastCheckBox->isChecked();
}
