#include "connection.h"
#include "defines.h"
#include "mainwindow.h"
#include "stream.h"

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
    Stream s(buf);

    if (s.readByte() == (char)0x01) {
        qDebug("Received handshake");
        if (mode == SERVER) {
            Stream str;
            str.writeByte(0x01);
            ctlSock->setWriteBuffer(str.data());

            QList<QString> songs = mwOwner->getSongList();
            Stream list;
            list.writeByte(0x02);
            list.writeInt(songs.size());
            for (int i = 0; i < songs.size(); i++) {
                list.writeInt(songs[i].size());
                list.write(songs[i].toUtf8());
            }
            ctlSock->setWriteBuffer(list.data());
        }
        buf.remove(0, s.position());
        handShakeRecv = true;
    } else if (s.readByte() == (char)0x02) {
        qDebug("Got the list of remote files");
        int count = s.readInt();
        QList<QString> songs;
        for (int i = 0; i < count; i++) {
            int len = s.readInt();
            songs.append(QString(s.read(len)));
        }
        mwOwner->addRemoteSongs(songs);

        buf.remove(0, s.position());
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
