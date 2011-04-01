#include "connection.h"
#include "defines.h"
#include "mainwindow.h"
#include "stream.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include "manager.h"

Connection::Connection(CommAudio* owner, QString host, int prot, int port)
        : mwOwner(owner), mode(CLIENT), protocol(prot) {
    
    ctlSock = new CommSocket(host,port,protocol);
	
    connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
    connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
    connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
    connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
    connect(ctlSock,SIGNAL(socketDisconnected()),this,SLOT(onDisconnected()));

    strSock = new CommSocket(host, port, UDP);
    connect(strSock,SIGNAL(socketRead()),this,SLOT(onStrReadReady()));	
    if (isMulticast) {
		strSock->toggleMulticast();
    } else {
        timer.setInterval(23);
        connect(&timer, SIGNAL(timeout()), this, SLOT(sendAudioBuffer()));
        timer.start();
    }

    fileSize = 0;
    isFileTransferInProgress = false;
	isMulticast = false;
    sentFileList = false;
	this->port = port;
    progressBar = mwOwner->getUi()->downloadProgressBar;
}

Connection::Connection(CommAudio* owner, int prot, int port, bool multicast = false)
        : mwOwner(owner), mode(SERVER), protocol(prot) {

    ctlSock = new CommSocket("",port,protocol);
    connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
    connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
    connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
    connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
    connect(ctlSock,SIGNAL(socketDisconnected()),this,SLOT(onDisconnected()));
    qDebug((QString::number(port)).toAscii().data());
    fileSize = 0;
    isFileTransferInProgress = false;
    sentFileList = false;
	isMulticast = multicast;
    progressBar = mwOwner->getUi()->downloadProgressBar;

    if (isMulticast) {
        strSock = new CommSocket("232.21.42.1", port, UDP);
        connect(strSock,SIGNAL(socketRead()),this,SLOT(onStrReadReady()));	

        timer.setInterval(23);
        connect(&timer, SIGNAL(timeout()), this, SLOT(sendAudioBuffer()));
        timer.start();
    }
}

void Connection::closeConnection() {
    ctlSock->closeSocket();
}

void Connection::makeMulticast() {
	isMulticast = !isMulticast;
    strSock->toggleMulticast();
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
    if (buf.size() == 0) {
        return;
    }

    Stream s(buf);
    unsigned char msgType = s.readByte();
    
    if (msgType == (char)0x01 && !isFileTransferInProgress)
    {
		buf.remove(0, s.position());
        /* Received handshake */
        if (mode == SERVER) {
            Stream str;
            str.writeByte(0x01);
			str.writeByte(isMulticast);
            ctlSock->setWriteBuffer(str.data());
			if(!isMulticast) {
				sendFileList();
			}
        }
		else {
			isMulticast = s.readByte();
		}
        buf.remove(0, s.position());
        handShakeRecv = true;
    }
    else if (msgType == (char)0x02 && !isFileTransferInProgress)
    {
        /* Got the list of remote files */
        int count = s.readInt();
        QList<QString> songs;
        for (int i = 0; i < count; i++) {
            int len = s.readInt();
            songs.append(QString(s.read(len)));
        }
        mwOwner->addRemoteSongs(songs);
        buf.remove(0, s.position());

        if (mode == CLIENT && !sentFileList) {
			if(!isMulticast) {
				sendFileList();
			}
        }
    }
    else if(msgType == (char)0x03 && !isFileTransferInProgress)
    {
        /* Got a file request */
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
    }
    else if(msgType == (char)0x04 && !isFileTransferInProgress)
    {
        /* Got the file data for a transfer */
        fileSize = s.readInt();
        isFileTransferInProgress = true;
        buf.remove(0, s.position());

        progressBar->setMinimum(0);
        progressBar->setMaximum(fileSize);
        progressBar->reset();
        progressBar->show();
        progressBar->raise();
    }
    else if(msgType == (char)0x05 && !isFileTransferInProgress)
    {
        /* Requested file does not exist */
        QMessageBox(QMessageBox::Critical, QString("Error"), QString("Could not transfer the requested file"));
    } else if (!isFileTransferInProgress) {
        qDebug("Got something to read");
    }

    if(isFileTransferInProgress) {
        DWORD bytesWritten;
        if(saveFile->write(buf,buf.size()) < 0) {
            QFile::FileError fe = saveFile->error();
        }
        
        progressBar->setValue(progressBar->value() + buf.size());
        fileSize -= buf.size();
        if(fileSize <= 0) {
            saveFile->close();
            isFileTransferInProgress = false;

            QFileInfo fi(*saveFile);
            mwOwner->addSong(fi.fileName(), fi.absoluteFilePath());

            progressBar->setValue(progressBar->maximum());
            progressBar->hide();

            Stream list;
            list.writeByte(0x02);
            list.writeInt(1);
            list.writeInt(fi.fileName().size());
            list.write(fi.fileName().toUtf8());
            ctlSock->setWriteBuffer(list.data());
        }
    }
}

void Connection::sendFileList() {
    QList<QString> songs = mwOwner->getSongList();

    Stream list;
    list.writeByte(0x02);
    list.writeInt(songs.size());
    for (int i = 0; i < songs.size(); i++) {
        list.writeInt(songs[i].size());
        list.write(songs[i].toUtf8());
    }
    ctlSock->setWriteBuffer(list.data());

    sentFileList = true;
}

bool Connection::sendFile(QString filename) {
    HANDLE filehandle;
    Stream data;
    char buf[BUFSIZE];
    int bytesRead = 0;
    DWORD filesize = 0;

    transmitFile = new QFile(filename);
    transmitFile->open(QIODevice::ReadOnly);
    data.writeByte(0x04);
    data.writeInt(transmitFile->size());
    
    while((bytesRead = transmitFile->read(buf,BUFSIZE-1)) > 0) {
        data.write(buf, bytesRead);
        ZeroMemory(buf,BUFSIZE);
    }
    qDebug("File Size: %d",data.size());
    ctlSock->setWriteBuffer(data.data());
    return true;
}

void Connection::sendAudioBuffer() {
    QByteArray* packet = AudioManager::getNextNetworkQueue();

    if (packet == NULL) {
        return;
    }

    strSock->setWriteBuffer(*packet);
    delete packet;
}

void Connection::requestForFile(QString filename) {
    
    if(filename == NULL || filename.isEmpty()) {
        return;
    }

    QString defPath = QString("music/") + filename;
    QString savefilename = QFileDialog::getSaveFileName(mwOwner, QString("Save File"), defPath);
    if (savefilename.isNull() || savefilename.isEmpty()) {
        return;
    }

    saveFile = new QFile(savefilename);
    saveFile->open(QIODevice::WriteOnly);

    Stream buf;
    buf.writeByte(0x03);
    buf.writeInt(filename.size());
    buf.write(filename.toAscii().data());

    ctlSock->setWriteBuffer(buf.data());
}

void Connection::onCtlWrite() {
    qDebug("Got something to write");
}

void Connection::onCtlAccept() {
    qDebug("Accepted a socket");
	if(!isMulticast) {
		ctlSock->closeSocket();
		ctlSock = ctlSock->getLastAcceptedSocket();
	}
    
	if(isMulticast) {
		multicastClients.push_back(ctlSock->getLastAcceptedSocket());

	}
	if(!isMulticast) {
		QString host;
		unsigned short cport;
		ctlSock->getHostAndPort(&host, &cport);
		strSock = new CommSocket(host, 9500, UDP);
		
		timer.setInterval(23);
		connect(&timer, SIGNAL(timeout()), this, SLOT(sendAudioBuffer()));
		timer.start();
	}
	
    connect(ctlSock,SIGNAL(socketAccepted()),this,SLOT(onCtlAccept()));
    connect(ctlSock,SIGNAL(socketConnected()),this,SLOT(onCtlConnect()));
    connect(ctlSock,SIGNAL(socketRead()),this,SLOT(onCtlReadReady()));	
    connect(ctlSock,SIGNAL(socketWrite()),this,SLOT(onCtlWrite()));
    connect(ctlSock,SIGNAL(socketDisconnected()),this,SLOT(onDisconnected()));

    connect(strSock,SIGNAL(socketRead()),this,SLOT(onStrReadReady()));	
}

void Connection::onCtlConnect() {
    if (protocol == TCP && mode == CLIENT) {
        QByteArray buf;
        buf.append(0x01);
        ctlSock->setWriteBuffer(buf);
    }
}

void Connection::onDisconnected() {
    mwOwner->disconnected();
}

void Connection::onStrReadReady() {
    qDebug("Reading");
    QByteArray& buf = strSock->getReadBuffer();

    AudioManager::addToQueue(buf);
}
