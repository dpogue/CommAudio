#include "mainwindow.h"

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
    qDebug("play selected song");
}

void CommAudio::onConnectClicked() {
    qDebug("connect to server");
}

void CommAudio::onChatPressed() {
    qDebug("chat pressed");
}

void CommAudio::onChatReleased() {
    qDebug("chat released");
}

