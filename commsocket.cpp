#undef UNICODE
#include "commsocket.h"
#include "defines.h"
#include <QNetworkInterface>

CommSocket::CommSocket(QString host, int port,int protocol) : QWidget(NULL)
{
	prot = protocol;
	sock = createSocket(winId(),host,(host.isEmpty() ? SERVER : CLIENT),port);
	
}

bool CommSocket::listenForConn()
{
	if(listen(sock,5) == SOCKET_ERROR)
	{
		int s;
		s = WSAGetLastError();
		return false;
	}
	return true;
}

bool CommSocket::connectToServ()
{
	
	if(WSAConnect(sock,(sockaddr*)&server,sizeof(server),NULL,NULL,NULL,NULL) == SOCKET_ERROR)
	{
		int s;
		if((s = WSAGetLastError()) != WSAEWOULDBLOCK)
		{
			return false;
		}
	}
	return true;
}

bool CommSocket::winEvent(MSG* message, long* result)
{
	switch(message->message) 
	{
    case WM_SOCKET:
        switch(WSAGETSELECTEVENT(message->lParam)) {
    		case FD_WRITE:
				if(writeBuffer.isEmpty()){
    				emit socketWrite();
				}
				else{
					write();
				}
    			break;
    		case FD_CONNECT:
    			emit socketConnected();
    			break;
    		case FD_ACCEPT:
    			emit socketAccepted();
    			break;
    		case FD_READ:
				if(read())
				{
    				emit socketRead();
				}
    			break;
    		case FD_CLOSE:
    			emit socketClose();
    			break;
        }
        
        return true;
    }
    return false;
}


SOCKET CommSocket::createSocket(HWND hwnd,QString host,int mode,int port)
{
	SOCKET s;
	struct sockaddr_in sin;
	long events;
	int type;
	
	type = (prot == UDP ? SOCK_DGRAM : SOCK_STREAM);
	events = FD_CONNECT | FD_WRITE | FD_ACCEPT | FD_READ | FD_CLOSE; 
	
	if ((s = socket(PF_INET, type, 0)) < 0)
		exit(1);

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,"1",1);

	if(mode == SERVER)
	{
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		if(bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1)
		{
			int s = WSAGetLastError();
			exit(1);
		}
	}
	else
	{
		memcpy(&server,&sin,sizeof(sockaddr_in));
		server.sin_addr.s_addr = inet_addr(host.toAscii().data());
	}
	int i = WSAAsyncSelect(s, hwnd, WM_SOCKET, events);
	return s;
}

bool CommSocket::read()
{
	int bytesRead = 0;
	int bytesToRead = BUFSIZE;
	char buffer[BUFSIZE];
	ZeroMemory(buffer,BUFSIZE);
	int senderAddrSize = sizeof(server);
	while(bytesToRead > 0)
	{
		if(prot == TCP) {
			bytesRead = recv(sock,buffer,bytesToRead,0);
		}
		else{
			bytesRead = recvfrom(sock,buffer,bytesToRead,0,(SOCKADDR *)&server, 
				&senderAddrSize);
		}
		if(bytesRead == SOCKET_ERROR)
		{
			int s = WSAGetLastError();
			if(s == WSAEWOULDBLOCK)
			{
				readBuffer = readBuffer.fromAscii(buffer);
				return true;
			}
			else
			{
				return false;
			}
		}
		bytesToRead -= bytesRead;
	}
	readBuffer = readBuffer.fromAscii(buffer);
	return true;
}
bool CommSocket::write()
{
	QString buffer;
	int bytesSent = 0;
	qStrCpy(buffer,writeBuffer,BUFSIZE);
	
	while(!writeBuffer.isEmpty())
	{
		if(prot == TCP){
			bytesSent = send(sock,buffer.toAscii().data(),buffer.size(),0);
		}
		else{
			bytesSent = sendto(sock,buffer.toAscii().data(),buffer.size(),0,(SOCKADDR *)&server,
				sizeof(server));
		}

		if(bytesSent == SOCKET_ERROR)
		{
			int s = WSAGetLastError();
			if(s != WSAEWOULDBLOCK){
				return false;
			}

			return true;
		}
		writeBuffer = writeBuffer.right(writeBuffer.size()-buffer.size());
		qStrCpy(buffer,writeBuffer,BUFSIZE);
	}
	emit socketWrite();
	return true;
}
bool CommSocket::setWriteBuffer(QString data)
{
	if(data == NULL)
		return true;
	if(writeBuffer.isEmpty()){
		writeBuffer = data;
		write();
		return true;
	}
	else{
		return false;
	}
}
bool CommSocket::qStrCpy(QString& dest,QString& src,int size)
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
QString CommSocket::getReadBuffer()
{
	return readBuffer;
}