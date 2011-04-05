#include "connectdialog.h"
#include <QSettings>
#include <WinSock2.h>
#include "commsocket.h"
#include "defines.h"
#include "mainwindow.h"
#include "manager.h"
#include "settingsdialog.h"
#include "ui_connect.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
      ui(new Ui::Connect()), running(false)
{
    ui->setupUi(this);
    
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
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
    QSettings settings;
    
    if (!(ip = ConnectDialog::validateIp(ui->ipLineEdit, 
            ui->connectErrorLabel))) {
        return;
    }

    if (!(port = ConnectDialog::validatePort(ui->portLineEdit, 
            ui->connectErrorLabel))) {
        return;
    }
    
    settings.setValue("lastHostIp", ui->ipLineEdit->text());
    settings.setValue("lastHostPort", ui->portLineEdit->text());

    ui->connectPushButton->setDisabled(true);
    ui->startServerPushButton->setDisabled(true);

	disconnect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    ui->connectPushButton->setDisabled(false);
    ui->connectPushButton->setText("Disconnect");
	connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onDisconnectClicked()));

    ((CommAudio*) this->parent())->connectToServer(ui->ipLineEdit->text(),
            port);
    running = true;
    done(0);
}

void ConnectDialog::onDisconnectClicked() {
	disconnect(ui->connectPushButton, SIGNAL(clicked()),
               this, SLOT(onDisconnectClicked()));

    ui->connectPushButton->setDisabled(false);
    ui->startServerPushButton->setDisabled(false);
    ui->connectPushButton->setText("Connect");
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    
    if (running) {
        running = false;
        ((CommAudio*) this->parent())->disconnectFromServer();
    }
    multicastServer = ui->multicastCheckBox->isChecked();
        
    done(0);
}

void ConnectDialog::onStartServerClicked() {
    unsigned int port = 0;
    QSettings settings;

    if (!(port = ConnectDialog::validatePort(ui->startServerPortLineEdit, 
            ui->connectErrorLabel))) {
        return;
    }

    settings.setValue("lastServerPort", ui->startServerPortLineEdit->text());
    settings.setValue("lastMulticast", ui->multicastCheckBox->isChecked());
    
    AudioManager::setMulticast(multicastServer);

    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
                this, SLOT(onStartServerClicked()));

    ui->connectPushButton->setDisabled(true);
    ui->startServerPushButton->setText("Stop Server");
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStopServerClicked()));

    ((CommAudio*) this->parent())->startServer(port);
    running = true;
    done(0);
}

void ConnectDialog::onStopServerClicked() {

    disconnect(ui->startServerPushButton, SIGNAL(clicked()),
               this, SLOT(onStopServerClicked()));

    ui->connectPushButton->setDisabled(false);
    ui->startServerPushButton->setDisabled(false);
    ui->startServerPushButton->setText("Start Server");
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    
    ((CommAudio*) this->parent())->stopServer();
    running = false;
    multicastServer = ui->multicastCheckBox->isChecked();

    done(0);
}

void ConnectDialog::onMulticastStateChanged(int state) {
    if (!running) {
        multicastServer = ui->multicastCheckBox->isChecked();
    }
}

unsigned long ConnectDialog::validateIp(QLineEdit* ipLineEdit, 
                                        QLabel* errorLabel) {
    unsigned long ip = 0;
    
    if ((ip = inet_addr(ipLineEdit->text().toAscii())) == INADDR_NONE) {
        errorLabel->setText("The ip address must be in the form x.x.x.x");
        ipLineEdit->setFocus();
        ipLineEdit->selectAll();
        return 0;
    }
    errorLabel->clear();
    return ip;
}

unsigned int ConnectDialog::validatePort(QLineEdit* portLineEdit, 
                                          QLabel* errorLabel) {
    unsigned int port = 0;
    bool validPort = false;
    
    port = portLineEdit->text().toUInt(&validPort);
    if (!validPort || port < 1024 || port > 65535) {
        errorLabel->setText("The port number must be between 1024 and 65535");
        portLineEdit->setFocus();
        portLineEdit->selectAll();
        return 0;
    }
    errorLabel->clear();
    return port;
}

void ConnectDialog::updateFields(bool useMostRecentConnectionSettings) {
    QSettings settings;

    if (running) {
        return;
    }

    if (useMostRecentConnectionSettings) {
        ui->ipLineEdit->setText(settings.value("lastHostIp", "").toString());
        ui->portLineEdit->setText(settings.value("lastHostPort", 
                "").toString());
        ui->startServerPortLineEdit->setText(settings.value("lastServerPort", 
                "").toString());
        ui->multicastCheckBox->setChecked(settings.value("lastMulticast", 
                false).toBool());
    } else {
        ui->ipLineEdit->setText(settings.value("hostIp", "").toString());
        ui->portLineEdit->setText(settings.value("hostPort", "").toString());
        ui->startServerPortLineEdit->setText(settings.value("serverPort", 
                "").toString());
        ui->multicastCheckBox->setChecked(settings.value("multicast", 
                false).toBool());
    }
}
