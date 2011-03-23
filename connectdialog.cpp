#include "connectdialog.h"
#include <WinSock2.h>
#include "commsocket.h"
#include "defines.h"
#include "mainwindow.h"
#include "ui_connect.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Connect())
{
    ui->setupUi(this);
    
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
/*    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
            */
    connect(ui->multicastCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onMulticastStateChanged(int)));

    multicastServer = ui->multicastCheckBox->isChecked();
}

ConnectDialog::~ConnectDialog() {
    delete ui;
    //delete ctlSock;
}

void ConnectDialog::onConnectClicked() {
    ((CommAudio*) this->parent())->onConnectClicked();
}
//    unsigned long ip = 0;
//    unsigned int port = 0;
//    bool validPort = false;
//    
//    if ((ip = inet_addr(ui->ipLineEdit->text().toAscii())) == INADDR_NONE) {
//        ui->connectErrorLabel->
//                setText("The ip address must be in the form x.x.x.x");
//        ui->ipLineEdit->selectAll();
//        return;
//    }
//
//    port = ui->serverPortLineEdit->text().toUInt(&validPort);
//    if (!validPort || port < 1024 || port > 65535) {
//        ui->connectErrorLabel->
//                setText("The port number must be between 1024 and 65535");
//        ui->serverPortLineEdit->selectAll();
//        return;
//    }
//
//    ui->connectErrorLabel->clear();
//
//    ui->connectPushButton->setDisabled(true);
//    ui->startServerPushButton->setDisabled(true);
//
//    ctlSock = new CommSocket(this->ui->ipLineEdit->text(), port, 0);
//    connect(ctlSock, SIGNAL(socketRead()), this, SLOT(onCtlReadReady()));
//	connect(ctlSock, SIGNAL(socketWrite()), this, SLOT(onCtlWrite()));
//
//    if (!ctlSock->connectToServ()) {
//        qDebug("Something went wrong trying to connect...");
//    }
//
//    ui->connectPushButton->setDisabled(false);
//    ui->connectPushButton->setText("Disconnect");
//}
//
//void ConnectDialog::onStartServerClicked() {
//    unsigned int port = 0;
//    bool validPort = false;
//
//    // TODO: disable gui->components
//
//    port = ui->serverPortLineEdit->text().toUInt(&validPort);
//    if (!validPort || port < 1024 || port > 65535) {
//        ui->connectErrorLabel->
//                setText("The port number must be between 1024 and 65535");
//        ui->serverPortLineEdit->selectAll();
//        return;
//    }
//
//    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
//                this, SLOT(onStartServerClicked()));
//
//    ui->connectPushButton->setDisabled(true);
//    ui->startServerPushButton->setText("Stop Server");
//    connect(ui->startServerPushButton, SIGNAL(clicked()),
//            this, SLOT(onStopServerClicked()));
//
//    if (ctlSock != NULL) {
//        ctlSock->closeSocket();
//        delete ctlSock;
//    }
//
//    ctlSock = new CommSocket("", port, TCP);
//    connect(ctlSock, SIGNAL(socketAccepted()), this, SLOT(onCtlAccept()));
//    if (!ctlSock->listenForConn(5)) {
//        qDebug("Something went wrong trying to listen...");
//    }
//}
//
//void ConnectDialog::onStopServerClicked() {
//    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
//                this, SLOT(onStopServerClicked()));
//
//    ui->connectPushButton->setDisabled(false);
//    ui->startServerPushButton->setText("Start Server");
//    connect(ui->startServerPushButton, SIGNAL(clicked()),
//            this, SLOT(onStartServerClicked()));
//
//    if (ctlSock == NULL) {
//        return;
//    }
//
//    ctlSock->closeSocket();
//    delete ctlSock;
//    ctlSock = NULL;
//}

void ConnectDialog::onMulticastStateChanged(int state) {
    multicastServer = ui->multicastCheckBox->isChecked();
}

//void ConnectDialog::onCtlReadReady() {
//
//    qDebug("Got something to read");
//    QByteArray data = ctlSock->getReadBuffer();
//    qDebug(data.data());
//}
//
//void ConnectDialog::onCtlWrite() {
//	qDebug("Got something to write");
//}
//
//void ConnectDialog::onCtlAccept() {
//    qDebug("Accepted a socket");
//}
