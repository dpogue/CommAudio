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
	fileSize = 0;
	isFileTransferInProgress = false;
}

Connection::Connection(CommAudio* owner, int prot, int port)
        : mwOwner(owner), mode(SERVER), protocol(prot) {

	ctlSock = new CommSocket("",port,protocol);
	connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
	connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
	connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
	connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
	qDebug((QString::number(port)).toAscii().data());
	fileSize = 0;
	isFileTransferInProgress = false;
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
    qDebug("Stream is %d bytes long.", buf.size());
    Stream s(buf);
    unsigned char msgType = s.readByte();
	
    if (msgType == (char)0x01) {
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
    } else if (msgType == (char)0x02) {
        qDebug("Got the list of remote files");
        int count = s.readInt();
        QList<QString> songs;
        for (int i = 0; i < count; i++) {
            int len = s.readInt();
            songs.append(QString(s.read(len)));
        }
        mwOwner->addRemoteSongs(songs);
        buf.remove(0, s.position());
		this->requestForFile("3.ogg");
    } else if(msgType == (char)0x03) {
        int len = s.readInt();
        QString songname(s.read(len));

        QString path = mwOwner->getSongFilePath(songname);
        if (path.isEmpty() || !QFile::exists(path)) {
            Stream resp;
            resp.writeByte(0x05);
            ctlSock->setWriteBuffer(resp.data());
        } else {
            sendFile(path);
        }
        buf.remove(0, s.position());
	} else if(msgType == (char)0x04) {
		fileSize = s.readInt();
		saveFile = new QFile("./test.txt");
		saveFile->open(QIODevice::WriteOnly);
	    isFileTransferInProgress = true;
        buf.remove(0, s.position());
	} else if(msgType == (char)0x05) {
		//file type requested does not exist
	} else {
        qDebug("Got something to read");
    }

	if(isFileTransferInProgress) {
		DWORD bytesWritten;
		if(saveFile->write(buf,buf.size()) < 0) {
			QFile::FileError fe = saveFile->error();
		}
	
		fileSize -= buf.size();
		if(fileSize <= 0) {
			saveFile->close();
			isFileTransferInProgress = false;
		}
	}
}

bool Connection::sendFile(QString filename) {
	HANDLE filehandle;
	Stream data;
	char buf[BUFSIZE];
	DWORD bytesRead = 0;
	DWORD filesize = 0;
	transmitFile = new QFile("./commsocket.cpp");

	data.writeByte(0x04);
	data.writeInt(transmitFile->size());
	
	while((bytesRead = transmitFile->read(buf,BUFSIZE-1)) > 0) {
		data.write(buf,bytesRead);
		qDebug(buf);
		ZeroMemory(buf,BUFSIZE);
	}
	qDebug("File Size: %d",data.size());
	ctlSock->setWriteBuffer(data.data());
	return true;
}

bool Connection::requestForFile(QString filename) {
	
	Stream buf;
	if(filename == NULL || filename.isEmpty()) {
		return false;
	}
	buf.writeByte(0x03);
	buf.writeInt(filename.size());
	buf.write(filename.toAscii().data());
	ctlSock->setWriteBuffer(buf.data());
	return true;
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
