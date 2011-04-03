#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class Settings;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

private:
    /** The user interface for the dialog window. */
    Ui::Settings* ui;

    /** True if the most recent connection settings should be remembered. */
    bool useLastConnSettings;
    
public:
    SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();
 
    void readSettings();
    void writeSettings();

public slots:
    /**
     * Attempts to connect to the server. 
     *
     * @author Dean Morin
     */
//    void onConnectClicked();
    void onOkClicked();

    void onCancelClicked();

    void onStickyChatStateChanged(int state);

    void onRememberConnectionOptionToggled(bool checked);

//    void onUseLastConnectionSetting

   // void onSettingChanged(QString setting, QVariant value);
};

#endif
