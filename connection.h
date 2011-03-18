#ifndef CONNECTION_H
#define CONNECTION_H
#include <qthread.h>
#include "commsocket.h"
class Connection : public QThread {
	
	Q_OBJECT

public:
	//for client
	Connection(QString host,int protocol,int port);
	//for server
	Connection(int protocol,int port);
private:
	bool handShake();
	/** The open TCP socket either for the control channel. */
    CommSocket* ctlSock;

	/** The open UDP socket for streaming data. */
    CommSocket* strSock;

	int mode;

protected:
	virtual void run();
public slots:
	void onCtlReadReady();
	void onCtlWrite();
	void onCtlAccept();
	void onCtlConnect();
};

#endif