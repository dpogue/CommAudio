#ifndef COMMSOCKET_H
#define COMMSOCKET_H
#include <QtGui/QMainWindow>
#include <WinSock2.h>
#include <Windows.h>

class CommSocket : public QWidget
{
	Q_OBJECT
private:
	SOCKET createSocket(HWND hwnd,int mode,int port,int protocol);
    SOCKET sock;
    
public:
    CommSocket(QString host, int port,int protocol);
       
    
protected:
    virtual bool winEvent(MSG* message, long* result);


signals:
    void connected();
    void socketAccepted();
    void socketRead();
    void socketWrite();
    void socketClose();
};
#endif

