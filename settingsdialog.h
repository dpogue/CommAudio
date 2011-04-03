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
     * Saves any changes to the settings.
     *
     * @author Dean Morin
     */
    void onOkClicked();

    /**
     * Reverts any changes to the settings.
     *
     * @author Dean Morin
     */
    void onCancelClicked();

    /**
     * Toggles sticky chat. When activated, the chat button is a toggle button.
     *
     * @author Dean Morin
     */
    void onStickyChatStateChanged(int);

    /**
     * Sets whether or not the most recently used connection details should be
     * remembered.
     *
     * @author Dean Morin
     * @param checked This slot is called by both radio buttons when a change
     * is made. This parameter ensures that the desired result only happens
     * once.
     */
    void onRememberConnectionOptionToggled(bool checked);

   // void onSettingChanged(QString setting, QVariant value);
};

#endif
