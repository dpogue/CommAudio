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

    bool getUseLastConnSettings() {
        return useLastConnSettings;
    }

public slots:
    /**
     * Reverts any changes to the settings.
     *
     * @author Dean Morin
     */
    virtual void reject();

    /**
     * Saves any changes to the settings.
     *
     * @author Dean Morin
     */
    void onOkClicked();

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
     * @param checked True if "Remember last used" is selected.
     */
    void onRememberConnectionOptionToggled(bool checked);
};

#endif
