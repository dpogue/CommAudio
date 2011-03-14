#include "mainwindow.h"
#include <WinSock2.h>

CommAudio::CommAudio(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

    connect(ui.playPushButton, SIGNAL(clicked()), 
            this, SLOT(onPlayClicked()));
    connect(ui.connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    connect(ui.chatPushButton, SIGNAL(pressed()),
            this, SLOT(onChatPressed()));
    connect(ui.chatPushButton, SIGNAL(released()),
            this, SLOT(onChatReleased()));
}

CommAudio::~CommAudio() { }

void CommAudio::onPlayClicked() {
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

    //connect(ip, port);
}

void CommAudio::onChatPressed() {
}

void CommAudio::onChatReleased() {
}

