#include "settingsdialog.h"
#include <QSettings>
#include <QtDebug>
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
            this, SLOT(onCancelClicked()));
    connect(ui->stickyChatCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onStickyChatStateChanged(int)));
    connect(ui->defaultSettingsRadioButton1, SIGNAL(toggled(bool)),
            this, SLOT(onRememberConnectionOptionToggled(bool)));
    connect(ui->defaultSettingsRadioButton2, SIGNAL(toggled(bool)),
            this, SLOT(onRememberConnectionOptionToggled(bool)));

    onStickyChatStateChanged(0);
    ui->okPushButton->setDefault(true);
    
    QCoreApplication::setOrganizationName("DBDFL");
    QCoreApplication::setApplicationName("WAM");

    readSettings();

    ui->defaultSettingsRadioButton1->setChecked(useLastConnSettings);
    ui->defaultSettingsRadioButton2->setChecked(!useLastConnSettings);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::readSettings() {
    QSettings settings;

    ui->stickyChatCheckBox->setChecked(settings.value("stickyChat").toBool());
    useLastConnSettings = settings.value("useLastConnSettings").toBool();
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("stickyChat", ui->stickyChatCheckBox->isChecked());
    settings.setValue("useLastConnSettings", useLastConnSettings);
}

void SettingsDialog::onOkClicked() {
    writeSettings();
    done(0);
}

void SettingsDialog::onCancelClicked() {
    readSettings();
    reject();
}

void SettingsDialog::onStickyChatStateChanged(int state) {
    bool checked = ui->stickyChatCheckBox->isChecked();
    ((CommAudio*) this->parent())->setStickyChat(checked);
}

void SettingsDialog::onRememberConnectionOptionToggled(bool checked) {
    if (checked) {
        useLastConnSettings = ui->defaultSettingsRadioButton1->isChecked();
    }
}
   /* 
void SettingsDialog::onSettingChanged(QString setting, QVariant value) {
    QSettings settings;
    bool useDefaultIpAndPort = false;

    if (setting == "defaultIp" && useDefaultIpAndPort) {
        return;
    }
    settings.setValue(setting, value);
}
*/