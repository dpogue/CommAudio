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
    
    /** True if the program should multicast on clicking "Start Server". */
    bool multicastServer;
//
//    /** The open TCP socket either for the control channel. */
//    CommSocket* ctlSock;
//
//    /** The open UDP socket for streaming data. */
//    CommSocket* strSock;
//
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
    
//    /**
//     * Starts listening for clients. 
//     *
//     * @author Dean Morin.
//     */
//    void onStartServerClicked();
//
//    /**
//     * Stops the server socket from listening for incoming connections.
//     *
//     * @author Darryl Pogue
//     */
//    void onStopServerClicked();

    /**
     * This server will multicast if this is selected.
     *
     * @author Dean Morin
     * @param state The state of multicastCheckBox.
     */
    void onMulticastStateChanged(int state);

//	void onCtlWrite();
//    void onCtlAccept();
//
//    /**
//     * Provides a notice that there is data available to be read on the control channel socket.
//     *
//     * @author Darryl Pogue
//     */
//    void onCtlReadReady();
};

#endif // CONNECTDIALOG_H
