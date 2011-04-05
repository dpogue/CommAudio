#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QBoxLayout>
#include <QListWidget>

namespace Ui {
    class Settings;
}

/**
 * A dialog window that allows the user to specify their program preferences.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

private:
    /** The user interface for the dialog window. */
    Ui::Settings* ui;

    /** True if the most recent connection settings should be remembered. */
    bool useLastConnSettings;

    /** The box layout used to show the music folders. */
    QBoxLayout* fileLayout;
    
    /** All of the music folders that are in use. */
    QListWidget* dirList;

public:
    /**
     * Initializes the dialog window. The global settings for the persistent
     * settings file are specified here.
     *
     * @author Dean Morin
     * @param parent The QWidget that is the parent of this QWidget.
     */
    SettingsDialog(QWidget* parent = 0);

    /**
     * Deletes dynamically created objects.
     *
     * @author Dean Morin
     */
    ~SettingsDialog();
    
    /**
     * Saves all of the settings specified in the dialog to a persistent file.
     * The exact type of file depends on the OS.
     *
     * @author Dean Morin
     */
    void readSettings();

    /**
     * Retrieves the user's settings from a persistent file. The exact type of
     * file depends on the OS.
     *
     * @author Dean Morin
     */
    void writeSettings();

    /** 
     * Returns true if the most recently used connection settings should be
     * remembered.
     *
     * @author Dean Morin
     * @return True if the most recently used connection settings should be
     * remembered.
     */
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

    /**
     * Prompts the user for a folder to add to the library.
     *
     * @author Dean Morin
     */
    void onAddFolderClicked();
    
    /**
     * Prompts the user for a folder to remove from the library.
     *
     * @author Dean Morin
     */
    void onRemoveFolderClicked();
};

#endif
