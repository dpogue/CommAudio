#ifndef CONNECTION_H
#define CONNECTION_H
#include <qthread.h>
#include <qfile.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include "commsocket.h"

class CommAudio;

class Connection : public QThread {
	
	Q_OBJECT

public:
	/**
    * Creates a new Connection.
    *
    * @author Kelvin Lui
	* @param owner a pointer to the owner of this object
    * @param host The host ip.
	* @param protocol the protocol to use.
	* @param port the port number to use.
    */
	Connection(CommAudio* owner, QString host,int protocol,int port);

	/**
    * Creates a new Connection
    *
    * @author Kelvin Lui
    * @param owner a pointer to the owner of this object
	* @param protocol the protocol to use.
	* @param port the port number to use.
	* @param multicast true is multicast is to be set
    */
	Connection(CommAudio* owner, int protocol,int port,bool multicast);

	
	void closeConnection();

	/**
    * sends a message to all connected multicast clients.
    *
    * @author Kelvin Lui
    * @param msgType defines the type of message. To be prepended at the beginning of the message
	* @param msg the message to be sent
    */
	void notifyMulticastClients(char msgType,char* msg);

private:
	/** A pointer to the owner of this Connection object*/
    CommAudio* mwOwner;

	/** The open TCP socket either for the control channel. */
    CommSocket* ctlSock;

	/** The open UDP socket for streaming data. */
    CommSocket* strSock;

	/** file to be saved */
	QFile* saveFile;

	/**file to be transmitted */
	QFile* transmitFile;

	/** Retains memory regarding the type of connection */
	int protocol;

	/** Retains memory regarding the host ip */
	QString host;

	/** Retains the port number */
	int port;

	/** Specifies whether the connection is client or server */
	int mode;

	/** Boolean for determing whether or not the handshake has been received*/
	bool handShakeRecv;

    /** Whether the list of files has been transferred or not. */
    bool sentFileList;

    /**
     * Sends the list of local files to the other client.
     *
     * @author Darryl Pogue
     */
    void sendFileList();

	/**
    * sends the specified file to the client
    *
    * @author Kelvin Lui
    * @param filename the file to be sent
	* @return true if file is sucessfully sent
    */
	bool sendFile(QString filename);


	/** Set to true if a file transfer is currently in progress*/
	bool isFileTransferInProgress;

	/** set to true if the current connection is multicast*/
	bool isMulticast;

	/** size of the last requested file*/
	int fileSize;

	/** A handle to the file being saved*/
	HANDLE saveFileHandle;

	/** The download progress bar. */
	QProgressBar* progressBar;	

    QTimer timer;
	
	/** A list of all connected multicast clients */
	QList<CommSocket*>multicastClients;

protected:
	/**
    * entry point for QThread
    *
    * @author Kelvin Lui
    */
	virtual void run();

public slots:
	/**
    * Called when the ctlSock has data to be read
    *
    * @author Kelvin Lui
    */
	void onCtlReadReady();

	/**
    * Called when the CommSocket is ready for writing
    *
    * @author Kelvin Lui
    */
	void onCtlWrite();
	
	/**
    * Called when the CommSocket has accepted a new connection
    *
    * @author Kelvin Lui
    */
	void onCtlAccept();

	/**
    * Called when the CommSocket has accepted a new connection
    *
    * @author Kelvin Lui
    */
	void onCtlConnect();

	/**
    * Called when the strSock has data to be read
    *
    * @author Kelvin Lui
    */
	void onStrReadReady();

    void sendAudioBuffer();

	/**
    * Called when a CommSocket has been disconnected
    *
    * @author Kelvin Lui
    */
    void onDisconnected();

	/**
    * Request for file from server
	* @param filename the file to be sent
    * @author Kelvin Lui
    */
	void requestForFile(QString filename);
};

#endif