#undef UNICODE
#include "commsocket.h"
#include "defines.h"
#include <QNetworkInterface>

CommSocket::CommSocket(QString host, int port,int protocol) : QWidget(NULL) {
	prot = protocol;
    multicasting = false;
	sock = createSocket(host,(host.isEmpty() ? SERVER : CLIENT),port);
}

CommSocket::CommSocket(SOCKET socket) {
	long events = FD_CONNECT | FD_WRITE | FD_ACCEPT | FD_READ | FD_CLOSE;
	prot = TCP;
	sock = socket;
    multicasting = false;
	WSAAsyncSelect(sock, winId(), WM_SOCKET, events);
}

CommSocket::~CommSocket() {
    if (sock != 0) {
        closesocket(sock);
    }
}

bool CommSocket::toggleMulticast() {
    if (prot == TCP) {
        return false;
    }

    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl(MULTICAST);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (!multicasting) {
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) {
            perror("setsockopt");
        }
        multicasting = !multicasting;
        return true;
    } else {
        if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) {
            perror("setsockopt");
        }
        multicasting = !multicasting;
        return false;
    }
}

void CommSocket::getHostAndPort(QString* host, unsigned short* port) {
    sockaddr_in addr;
    int size = sizeof(addr);
    int ret = 0;

    if ((ret = getpeername(sock, (sockaddr*)&addr, (socklen_t*)&size)) == SOCKET_ERROR) {
        int e = WSAGetLastError();
        qDebug("%d", e);
    }

    hostent* hp = gethostbyaddr((char*)&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
    if (hp != NULL) {
        *host = QString(hp->h_name);
    } else {
        qDebug("Something went wrong getting the hostname!");
        *host = QString();
    }

    *port = ntohs(addr.sin_port);
}

bool CommSocket::listenForConn(int backlog) {
	
	if(listen(sock,backlog) == SOCKET_ERROR) {
		int s;
		s = WSAGetLastError();
		return false;
	}
	return true;
}

bool CommSocket::winEvent(MSG* message, long* result) {
	
	
	switch(message->message) {
	case WM_SOCKET:

		if(WSAGETSELECTERROR(message->lParam)) {
				qDebug((QString::number(WSAGETSELECTERROR(message->lParam))).toAscii().data());
                emit socketDisconnected();
		}
        switch(WSAGETSELECTEVENT(message->lParam)) {
			
    		case FD_WRITE:
				if(writeBuffer.isEmpty()) {
    				emit socketWrite();
				}
				else {
					write();
				}
    			break;
    		case FD_CONNECT:
    			emit socketConnected();
    			break;
    		case FD_ACCEPT:
				acceptConn();
    			emit socketAccepted();
    			break;
    		case FD_READ:
				if(read()) {
    				emit socketRead();
				}
    			break;
    		case FD_CLOSE:
				closesocket(sock);
    			emit socketClose();
    			break;
		}
        return true;
    }
    return false;
}

SOCKET CommSocket::createSocket(QString host,int mode,int port) {
	
	SOCKET s;
	struct sockaddr_in sin;
	long events;
	int type;
	
	type = (prot == UDP ? SOCK_DGRAM : SOCK_STREAM);
	events = FD_CONNECT | FD_WRITE | FD_ACCEPT | FD_READ | FD_CLOSE; 
	
	if ((s = socket(PF_INET, type, 0)) < 0) {
		exit(1);
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,"1",1);

	if(mode == SERVER) {
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		qDebug("binding");
		if(bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			int s = WSAGetLastError();
			exit(1);
		}
	}
	else {
		memcpy(&server,&sin,sizeof(sockaddr_in));
		server.sin_addr.s_addr = inet_addr(host.toAscii().data());
	}

	if(WSAAsyncSelect(s, winId(), WM_SOCKET, events)) {
		int s = WSAGetLastError();
		return false;
	}
	return s;
}

bool CommSocket::read() {
	int bytesRead = 0;
	int bytesToRead = BUFSIZE;
	char buffer[BUFSIZE];
	
	int senderAddrSize = sizeof(server);
	readBuffer.clear();
	ZeroMemory(buffer,BUFSIZE);
	while(bytesToRead > 0) {
		if(prot == TCP) {
			bytesRead = recv(sock,buffer,bytesToRead,0);
		}
		else {
			bytesRead = recvfrom(sock,buffer,bytesToRead,0,(SOCKADDR *)&server, 
				&senderAddrSize);
		}
		if(bytesRead == SOCKET_ERROR) {
			int s = WSAGetLastError();
			if(s == WSAEWOULDBLOCK) {
                readBuffer.append(buffer, BUFSIZE - bytesToRead);
				return true;
			}
			else {
				return false;
			}
		} else if (bytesRead == 0) {
            /* Disconnected! */
            emit socketDisconnected();
            return false;
        }
		bytesToRead -= bytesRead;
	}
    readBuffer.append(buffer, BUFSIZE - bytesToRead);
	return true;
}

bool CommSocket::write() {
	int bytesSent = 0;

    if (prot == UDP) {
        bytesSent = sendto(sock, writeBuffer.data(), writeBuffer.size(), 0, (SOCKADDR *)&server,
				sizeof(server));
        if (bytesSent == SOCKET_ERROR) {
			int s = WSAGetLastError();
			if(s != WSAEWOULDBLOCK){
				return false;
			}
			return true;
        }
        writeBuffer.clear();
        emit socketWrite();
        return true;
    }

	QByteArray buffer;
	qBinCpy(buffer,writeBuffer,BUFSIZE);
	
	while(!writeBuffer.isEmpty()) {
        bytesSent = send(sock,buffer.data(),buffer.size(),0);
		if(bytesSent == SOCKET_ERROR) {
			int s = WSAGetLastError();
			if(s != WSAEWOULDBLOCK){
				return false;
			}
			return true;
		}
		writeBuffer = writeBuffer.right(writeBuffer.size()-buffer.size());
		buffer.clear();
		qBinCpy(buffer,writeBuffer,BUFSIZE);
	}
	emit socketWrite();
	return true;
}

bool CommSocket::setWriteBuffer(QByteArray data)
{
	if(writeBuffer.isEmpty()){
		writeBuffer = data;
		write();
		return true;
	} else {
		return false;
	}
}

bool CommSocket::qBinCpy(QByteArray& dest,QByteArray& src,int size)
{
	if(src.isNull()){
		return false;
	}
	for(int i = 0; i < size && i < src.size(); i++)
	{
		dest[i] = src[i];
	}
	return true;
}

QByteArray& CommSocket::getReadBuffer() {
	return readBuffer;
}

void CommSocket::closeSocket() {
    
	closesocket(sock);
    sock = 0;
}

bool CommSocket::connectToServ() {
	
	if(WSAConnect(sock,(sockaddr*)&server,sizeof(server),NULL,NULL,NULL,NULL) == SOCKET_ERROR)
	{
		int s;
		if((s = WSAGetLastError()) != WSAEWOULDBLOCK) {
			return false;
		}
	}
	return true;
}

bool CommSocket::acceptConn() {

	long events = FD_CONNECT | FD_WRITE | FD_ACCEPT | FD_READ | FD_CLOSE; 
	lastAccepted = WSAAccept(sock,NULL,NULL,NULL,NULL);
	
	if(lastAccepted == INVALID_SOCKET) {
		return false;
	}
	WSAAsyncSelect(lastAccepted,winId(),WM_SOCKET,0);
	return true;
}

CommSocket* CommSocket::getLastAcceptedSocket() {
	
	return new CommSocket(lastAccepted);
}