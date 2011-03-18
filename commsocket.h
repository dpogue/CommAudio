#ifndef COMMSOCKET_H
#define COMMSOCKET_H
#include <QtGui/QMainWindow>
#include <WinSock2.h>
#include <Windows.h>

class CommSocket : public QWidget {
	
	Q_OBJECT
private:
	SOCKET createSocket(QString host,int mode,int port);
	bool read();
	bool write();
	bool qBinCpy(QByteArray& dest,QByteArray& src,int size);
	bool acceptConn();
    SOCKET sock;
	SOCKET lastAccepted;
	//Don't ask me, talk to microsoft
	static int CALLBACK acceptCondition(LPWSABUF lpCallerId,LPWSABUF lpCallerData,LPQOS lpSQOS,LPQOS lpGQOS,
		LPWSABUF lpCalleeId,LPWSABUF lpCalleeData,GROUP *g,DWORD* dwCallbackData);
	sockaddr_in server;
	QByteArray readBuffer;
	QByteArray writeBuffer;
	int prot;
	
public:
    CommSocket(QString host, int port,int protocol);
	CommSocket(SOCKET socket);
    virtual ~CommSocket();
	CommSocket* getLastAcceptedSocket();
	bool setWriteBuffer(QByteArray data);
    QByteArray getReadBuffer();
	bool connectToServ();
	bool listenForConn(int backlog);
    void closeSocket();

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

