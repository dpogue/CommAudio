#ifndef COMMSOCKET_H
#define COMMSOCKET_H
#include <QtGui/QMainWindow>
#include <WinSock2.h>
#include <Windows.h>

class CommSocket : public QWidget
{
	Q_OBJECT
private:
	SOCKET createSocket(HWND hwnd,QString host,int mode,int port,int protocol);
    SOCKET sock;
	sockaddr_in server;

    
public:
    CommSocket(QString host, int port,int protocol);
	bool connectToServ();
	bool listenForConn();
    
protected:
    virtual bool winEvent(MSG* message, long* result);

signals:
    void socketConnected();
    void socketAccepted();
    void socketRead();
    void socketWrite();
    void socketClose();
};
#endif

