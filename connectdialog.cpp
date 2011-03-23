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

    onMulticastStateChanged(0);
}

ConnectDialog::~ConnectDialog() {
    delete ui;
}

void ConnectDialog::onConnectClicked() {
    unsigned long ip = 0;
    unsigned int port = 0;
    bool validPort = false;
    
    if ((ip = inet_addr(ui->ipLineEdit->text().toAscii())) == INADDR_NONE) {
        ui->connectErrorLabel->
                setText("The ip address must be in the form x.x.x.x");
        ui->ipLineEdit->selectAll();
        return;
    }

    port = ui->portLineEdit->text().toUInt(&validPort);
    if (!validPort || port < 1024 || port > 65535) {
        ui->connectErrorLabel->
                setText("The port number must be between 1024 and 65535");
        ui->portLineEdit->selectAll();
        return;
    }

    ui->connectErrorLabel->clear();

    ui->connectPushButton->setDisabled(true);
    ui->startServerPushButton->setDisabled(true);

    ui->connectPushButton->setDisabled(false);
    ui->connectPushButton->setText("Disconnect");

    ((CommAudio*) this->parent())->connectToServer(ui->ipLineEdit->text(), port);
}

void ConnectDialog::onStartServerClicked() {
    unsigned int port = 0;
    bool validPort = false;

    // TODO: disable gui components

    port = ui->portLineEdit->text().toUInt(&validPort);
    if (!validPort || port < 1024 || port > 65535) {
        ui->connectErrorLabel->
                setText("The port number must be between 1024 and 65535");
        ui->portLineEdit->selectAll();
        return;
    }

    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
                this, SLOT(onStartServerClicked()));

    ui->connectPushButton->setDisabled(true);
    ui->startServerPushButton->setText("Stop Server");
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStopServerClicked()));

    ((CommAudio*) this->parent())->startServer(port);
}

void ConnectDialog::onStopServerClicked() {

    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
               this, SLOT(onStopServerClicked()));

    ui->connectPushButton->setDisabled(false);
    ui->startServerPushButton->setText("Start Server");
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    
    ((CommAudio*) this->parent())->stopServer();
}

void ConnectDialog::onMulticastStateChanged(int state) {
    bool checked = ui->multicastCheckBox->isChecked();
    ((CommAudio*) this->parent())->onMulticastStateChanged(checked);
}
