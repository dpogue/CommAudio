#include "connection.h"
#include "defines.h"

Connection::Connection(QString host,int protocol,int port) : mode(CLIENT) {
	
	ctlSock = new CommSocket("",protocol,port);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlRead()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
}

Connection::Connection(int protocol,int port) : mode(SERVER) {
	ctlSock = new CommSocket("",protocol,port);
}

void Connection::run() {
	WSABUF sendData;
	WSABUF recvData;
	ZeroMemory(&sendData,sizeof(WSABUF));
	ZeroMemory(&recvData,sizeof(WSABUF));

	if(mode == SERVER) {
		ctlSock->listenForConn(BACKLOG);
	}
	else {
		sendData.buf[0] = 1;
		sendData.len    = 1;
		ctlSock->connectToServ(&sendData,&recvData);
	}
	//handShake();
	this->exec();
}

bool Connection::handShake() {
	WSABUF sendData;

	if(mode == CLIENT) {	
	}
	else if(mode == SERVER) {
	}
	return true;
}
void Connection::onCtlReadReady() {

    qDebug("Got something to read");
    //QByteArray data = ctlSock->getReadBuffer();
   // qDebug(data.data());
}

void Connection::onCtlWrite() {
	qDebug("Got something to write");
}

void Connection::onCtlAccept() {
    qDebug("Accepted a socket");
	ctlSock->setWriteBuffer("1");
}