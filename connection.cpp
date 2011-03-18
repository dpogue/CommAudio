#include "connection.h"
#include "defines.h"

Connection::Connection(QString host,int protocol,int port) : mode(CLIENT) {
	
	ctlSock = new CommSocket("",protocol,port);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
}

Connection::Connection(int protocol,int port) : mode(SERVER) {

	ctlSock = new CommSocket("",protocol,port);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
	qDebug((QString::number(port)).toAscii().data());
}

void Connection::run() {
	
	if(mode == SERVER) {
		ctlSock->listenForConn(BACKLOG);
		qDebug("listening");
	}
	else {
		//sendData.buf[0] = 1;
		//sendData.len    = 1;
		ctlSock->connectToServ();
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

void Connection::onCtlConnect() {
	//To be implemented
}