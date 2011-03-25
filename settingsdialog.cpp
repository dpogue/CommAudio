#include "settingsdialog.h"
#include <QtDebug>
#include "mainwindow.h"
#include "ui_settings.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint), 
      ui(new Ui::Settings())
{
    ui->setupUi(this);
    
    connect(ui->okPushButton, SIGNAL(clicked()),
            this, SLOT(onOkClicked()));
    connect(ui->cancelPushButton, SIGNAL(clicked()),
            this, SLOT(onCancelClicked()));
    connect(ui->stickyChatCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onStickyChatChanged(int)));

    onStickyChatStateChanged(0);
    ui->okPushButton->setDefault(true);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::onOkClicked() {
    //save to settings
    done(0);
}

void SettingsDialog::onCancelClicked() {
    reject();
}

void SettingsDialog::onStickyChatStateChanged(int state) {
    bool checked = ui->stickyChatCheckBox->isChecked();
    qDebug() << "checked";
    ((CommAudio*) this->parent())->setStickyChat(checked);
}