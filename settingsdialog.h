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
    
public:
    SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();
    
public slots:
    /**
     * Attempts to connect to the server. 
     *
     * @author Dean Morin
     */
//    void onConnectClicked();
};

#endif
