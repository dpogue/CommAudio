#undef UNICODE
#include "commsocket.h"
#include "defines.h"
#include <QNetworkInterface>

CommSocket::CommSocket(QString host, int port,int protocol) : QWidget(NULL)
{
	sock = createSocket(winId(),host,(host.isEmpty() ? SERVER : CLIENT),port,protocol);
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
    			emit socketWrite();
    			break;
    		case FD_CONNECT:
    			emit socketConnected();
    			break;
    		case FD_ACCEPT:
    			emit socketAccepted();
    			break;
    		case FD_READ:
    			emit socketRead();
    			break;
    		case FD_CLOSE:
    			emit socketClose();
    			break;
        }
        // emit signals in response to events
        return true;
    }
    return false;
}


SOCKET CommSocket::createSocket(HWND hwnd,QString host,int mode,int port,int protocol)
{
	SOCKET s;
	struct sockaddr_in sin;
	long events;
	int type;
	
	type = (protocol == UDP ? SOCK_DGRAM : SOCK_STREAM);
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

