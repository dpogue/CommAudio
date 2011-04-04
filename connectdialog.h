#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
class CommSocket;

namespace Ui {
    class Connect;
}

/**
 * A dialog window which allows the user to connect to a host, or to start a
 * server.
 */
class ConnectDialog : public QDialog {
    Q_OBJECT

private:
    /** The user interface for the dialog window. */
    Ui::Connect* ui;

    /** True if the program is connected to a host or running a server. */
    bool running;
    
public:
    /**
     * Initializes the dialog window.
     *
     * @author Dean Morin
     * @param parent The QWidget that is the parent of this QWidget.
     */
    ConnectDialog(QWidget* parent = 0);

    /** 
     * Deletes dynamically created objects.
     *
     * @author Dean Morin
     */
    ~ConnectDialog();

    /**
     * Ensures the ip that was entered is valid.
     *
     * @author Dean Morin
     * @param ipLineEdit The ip address as entered.
     * @param errorLabel The label that will display any errors.
     * @return 0 if there was an error, otherwise returns the ip address.
     */
    static unsigned long validateIp(QLineEdit* ipLineEdit, QLabel* errorLabel);

    /**
     * Ensures the port that was entered is valid.
     *
     * @author Dean Morin
     * @param portLineEdit The port as entered.
     * @param errorLabel The label that will display any errors.
     * @return 0 if there was an error, otherwise returns the port.
     */
    static unsigned int validatePort(QLineEdit* portLineEdit, 
                                     QLabel* errorLabel);

    /**
     * Fills in the connection details with either the last (succesful)
     * connection particulars, or the defaults specified in the settings
     * menu, depending on the value of the argument passed in.
     *
     * @author Dean Morin
     * @param useMostRecentConnectionSettings True if the most recent 
     * connection's settings should be remember (ip, port, etc.)
     */
    void updateFields(bool useMostRecentConnectionSettings);

public slots:
    /**
     * Attempts to connect to the server. 
     *
     * @author Dean Morin
     */
    void onConnectClicked();

	/**
	 * Closes the open connection.
	 *
	 * @author Darryl Pogue
	 */
	void onDisconnectClicked();
    
    /**
     * Starts listening for clients. 
     *
     * @author Dean Morin.
     */
    void onStartServerClicked();

    /**
     * Stops the server socket from listening for incoming connections.
     *
     * @author Darryl Pogue
     */
    void onStopServerClicked();

    /**
     * This server will multicast if this is selected.
     *
     * @author Dean Morin
     * @param state The state of multicastCheckBox.
     */
    void onMulticastStateChanged(int state);
};

#endif // CONNECTDIALOG_H
