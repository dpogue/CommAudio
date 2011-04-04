#include "settingsdialog.h"
#include <QSettings>
#include "connectdialog.h"
#include "mainwindow.h"
#include "ui_settings.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint), 
      ui(new Ui::Settings()), useLastConnSettings(true)
{
    ui->setupUi(this);
    
    connect(ui->okPushButton, SIGNAL(clicked()),
            this, SLOT(onOkClicked()));
    connect(ui->cancelPushButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    connect(ui->stickyChatCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onStickyChatStateChanged(int)));
    connect(ui->defaultSettingsRadioButton1, SIGNAL(toggled(bool)),
            this, SLOT(onRememberConnectionOptionToggled(bool)));
            
    onStickyChatStateChanged(0);
    ui->okPushButton->setDefault(true);
    
    QCoreApplication::setOrganizationName("DBDFL");
    QCoreApplication::setApplicationName("WAM");

    readSettings();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::readSettings() {
    QSettings settings;

    useLastConnSettings = settings.value("useLastConnSettings", true).toBool();
    ui->stickyChatCheckBox->setChecked(settings.value("stickyChat", 
            false).toBool());
    ui->hostIpLineEdit->setText(settings.value("hostIp", "").toString());
    ui->hostPortLineEdit->setText(settings.value("hostPort", "").toString());
    ui->serverPortLineEdit->setText(settings.value("serverPort", 
            "").toString());
    ui->multicastCheckBox->setChecked(settings.value("multicast", 
            false).toBool());
    
    // setup gui elements according to stored settings
    ui->defaultSettingsRadioButton1->setChecked(useLastConnSettings);
    ui->defaultSettingsRadioButton2->setChecked(!useLastConnSettings);
    onRememberConnectionOptionToggled(useLastConnSettings);
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("useLastConnSettings", useLastConnSettings);
    settings.setValue("hostIp", ui->hostIpLineEdit->text());
    settings.setValue("hostPort", ui->hostPortLineEdit->text());
    settings.setValue("serverPort", ui->serverPortLineEdit->text());
    settings.setValue("multicast", ui->multicastCheckBox->isChecked());
    settings.setValue("stickyChat", ui->stickyChatCheckBox->isChecked());
}

void SettingsDialog::reject() {
    readSettings();
    QDialog::reject();
}

void SettingsDialog::onOkClicked() {
    
    if (!useLastConnSettings) {
        if (!ui->hostIpLineEdit->text().isEmpty() && 
                !ConnectDialog::validateIp(ui->hostIpLineEdit, 
                                           ui->errorLabel)) {
            return;
        }
        if (!ui->hostPortLineEdit->text().isEmpty() && 
                !ConnectDialog::validatePort(ui->hostPortLineEdit, 
                                             ui->errorLabel)) {
            return;
        }
        if (!ui->serverPortLineEdit->text().isEmpty() && 
                !ConnectDialog::validatePort(ui->serverPortLineEdit, 
                                             ui->errorLabel)) {
            return;
        }
    }
    writeSettings();
    done(0);
}

void SettingsDialog::onStickyChatStateChanged(int) {
    bool checked = ui->stickyChatCheckBox->isChecked();
    ((CommAudio*) this->parent())->setStickyChat(checked);
}

void SettingsDialog::onRememberConnectionOptionToggled(bool checked) {
    
    useLastConnSettings = checked;
    ui->hostGroupBox->setDisabled(checked);
    ui->serverGroupBox->setDisabled(checked);
}
