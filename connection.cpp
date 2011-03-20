#include "connection.h"
#include "defines.h"
#include "mainwindow.h"

Connection::Connection(CommAudio* owner, QString host, int prot, int port)
        : mwOwner(owner), mode(CLIENT), protocol(prot) {
	
	ctlSock = new CommSocket(host,port,protocol);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
}

Connection::Connection(CommAudio* owner, int prot, int port)
        : mwOwner(owner), mode(SERVER), protocol(prot) {

	ctlSock = new CommSocket("",port,protocol);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
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
		ctlSock->connectToServ();
	}

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
    QByteArray& buf = ctlSock->getReadBuffer();

    if (buf[0] == (char)0x01) {
        qDebug("Received handshake");
        if (mode == SERVER) {
            QByteArray buf2;
            buf2.append(0x01);
            ctlSock->setWriteBuffer(buf2);

            QByteArray buf3;
            QList<QString> list = mwOwner->getSongList();
            buf3.append(0x02);
            buf3.append(list.size());
            for (int i = 0; i < list.size(); i++) {
                buf3.append(list[i].length());
                buf3.append(list[i]);
            }
            ctlSock->setWriteBuffer(buf3);
        }
        handShakeRecv = true;
    } else {
        qDebug("Got something to read");
    }
}

void Connection::onCtlWrite() {
	qDebug("Got something to write");
}

void Connection::onCtlAccept() {
    qDebug("Accepted a socket");
	ctlSock->closeSocket();
	ctlSock = ctlSock->getLastAcceptedSocket();
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));

}

void Connection::onCtlConnect() {
    if (protocol == TCP && mode == CLIENT) {
        QByteArray buf;
        buf.append(0x01);
        ctlSock->setWriteBuffer(buf);
    }
}
