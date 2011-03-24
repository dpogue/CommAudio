#include "settingsdialog.h"
#include "ui_settings.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Settings())
{
    ui->setupUi(this);
/*    
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));
    connect(ui->startServerPushButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(ui->multicastCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onMulticastStateChanged(int)));

    onMulticastStateChanged(0); */
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
