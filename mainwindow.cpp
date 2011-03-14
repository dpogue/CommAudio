#include "mainwindow.h"
#include <WinSock2.h>
#include "stylesheet.h"
#include "defines.h"
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

    //TODO: move to settings
    
}

CommAudio::~CommAudio() { }

void CommAudio::onPlayClicked() {
    //QString fileName = "hard code file name here for now"
    //terrysPlayFunction(fileName);
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

    testing = new CommSocket(this->ui.ipLineEdit->text(), port, UDP);
    connect(testing, SIGNAL(socketRead()), this, SLOT(onCtlRead()));
	connect(testing, SIGNAL(socketWrite()), this, SLOT(onCtlWrite()));

    bool b = testing->connectToServ();
    if (b) {
        qDebug("Connected");
    }

}

void CommAudio::onStartServerClicked() {
    qDebug("onStartServer()");
    // startlisteningforconnections(multicastServer)
}

void CommAudio::onChatPressed() {
}

void CommAudio::onChatReleased() {
}

void CommAudio::onMulticastStateChanged(int state) {
    multicastServer = ui.multicastCheckBox->isChecked();
}

void CommAudio::onCtlRead() {
    qDebug("Got something to read");
	
	qDebug(testing->getReadBuffer().toAscii().data());
}

void CommAudio::onCtlWrite(){
	qDebug("Got something to write");
	testing->setWriteBuffer("udp");
}
