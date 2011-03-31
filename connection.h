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
	//for client
	Connection(CommAudio* owner, QString host,int protocol,int port);
	//for server
	Connection(CommAudio* owner, int protocol,int port);

    void makeMulticast();

	void closeConnection();

private:
	bool handShake();

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

	/** sends the file to the client*/
	bool sendFile(QString filename);


	/** saves the incoming file */
	//bool saveFile();

	bool isFileTransferInProgress;
	int fileSize;
	HANDLE saveFileHandle;

	/** The download progress bar. */
	QProgressBar* progressBar;	

    QTimer timer;

protected:
	virtual void run();

public slots:
	void onCtlReadReady();
	void onCtlWrite();
	void onCtlAccept();
	void onCtlConnect();

	void onStrReadReady();

    void sendAudioBuffer();

    void onDisconnected();

	void requestForFile(QString filename);
};

#endif