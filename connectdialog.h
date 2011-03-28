#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
class CommSocket;

namespace Ui {
    class Connect;
}

class ConnectDialog : public QDialog {
    Q_OBJECT

private:
    /** The user interface for the dialog window. */
    Ui::Connect* ui;
    
public:
    ConnectDialog(QWidget* parent = 0);
    ~ConnectDialog();
    
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
