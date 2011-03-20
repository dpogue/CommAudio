#ifndef CONNECTION_H
#define CONNECTION_H
#include <qthread.h>
#include "commsocket.h"

class CommAudio;

class Connection : public QThread {
	
	Q_OBJECT

public:
	//for client
	Connection(CommAudio* owner, QString host,int protocol,int port);
	//for server
	Connection(CommAudio* owner, int protocol,int port);
private:
	bool handShake();

    CommAudio* mwOwner;

	/** The open TCP socket either for the control channel. */
    CommSocket* ctlSock;

	/** The open UDP socket for streaming data. */
    CommSocket* strSock;

	/** Retains memory regarding the type of connection */
	int protocol;

	/** Specifies whether the connection is client or server */
	int mode;

	/** Boolean for determing whether or not the handshake has been received*/
	bool handShakeRecv;

protected:
	virtual void run();
public slots:
	void onCtlReadReady();
	void onCtlWrite();
	void onCtlAccept();
	void onCtlConnect();
};

#endif