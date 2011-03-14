#include "mainwindow.h"
#include <WinSock2.h>
#include <qdir.h>
#include "manager.h"
#include "defines.h"
#include "stylesheet.h"

CommAudio::CommAudio(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
    this->setStyleSheet(StyleSheet::commAudio());

    connect(ui.playPushButton, SIGNAL(clicked()), 
            this, SLOT(onPlayClicked()));
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
    playing = false;

    //TODO: move to settings
    if(!QDir("music").exists()) {
        QDir().mkdir("music");
    }
    userSongs.addFolder("music/");
}

CommAudio::~CommAudio() { 
    AudioManager::instance()->shutdown();
}

void CommAudio::onPlayClicked() {

    AudioManager::instance()->pause();
    if (!playing) {
        playing = true;
        ui.playPushButton->setIcon(QIcon(ICON_PAUSE));
        QString fileName = "music/3.ogg";
        AudioManager::instance()->playMusic(fileName);
  } else {
        playing = false;
        ui.playPushButton->setIcon(QIcon(ICON_PLAY));
        //terrysGainFunction(0);
    }
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

    ctlSock = new CommSocket(this->ui.ipLineEdit->text(), port, 0);
    connect(ctlSock, SIGNAL(socketRead()), this, SLOT(onCtlReadReady()));
	connect(ctlSock, SIGNAL(socketWrite()), this, SLOT(onCtlWrite()));

    if (!ctlSock->connectToServ()) {
        qDebug("Something went wrong trying to connect...");
    }
}

void CommAudio::onStartServerClicked() {
    qDebug("onStartServer()");
    // startlisteningforconnections(multicastServer)
    // TODO: disable gui components
}

void CommAudio::onChatPressed() {
}

void CommAudio::onChatReleased() {
}

void CommAudio::onMulticastStateChanged(int state) {
    multicastServer = ui.multicastCheckBox->isChecked();
}

void CommAudio::onCtlReadReady() {
    qDebug("Got something to read");
	
	qDebug(ctlSock->getReadBuffer().toAscii().data());
}

void CommAudio::onCtlWrite(){
	qDebug("Got something to write");
	ctlSock->setWriteBuffer("udp");
}
