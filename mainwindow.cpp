#include "mainwindow.h"
#include "commsocket.h"
#include <QLayout>

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

    QString host = this->ui.ipLineEdit->text();
    int port = this->ui.portLineEdit->text().toInt();

    CommSocket* s = new CommSocket(host, port, 0);
    connect(s, SIGNAL(socketRead()), this, SLOT(onCtlRead()));

    bool b = s->connectToServ();
    if (b) {
        qDebug("Connected");
    }
}

void CommAudio::onChatPressed() {
    qDebug("chat pressed");
}

void CommAudio::onChatReleased() {
    qDebug("chat released");
}

void CommAudio::onCtlRead() {
    qDebug("Got something to read");
}

