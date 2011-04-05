#ifndef COMMSOCKET_H
#define COMMSOCKET_H
#include <QtGui/QMainWindow>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

class CommSocket : public QWidget {
	
	Q_OBJECT
private:
	/**
    * Creates a new SOCKET.
    *
    * @author Kelvin Lui
    * @param host The host ip.
	* @param mode either SERVER or CLIENT.
	* @param port the port number to use.
	* @return the new SOCKET
    */
	SOCKET createSocket(QString host,int mode,int port);
	
	/**
    * Reads data from the SOCKET sock.
    *
    * @author Kelvin Lui
	* @return true if the operation worked successfully
    */
	bool read();

	/**
    * Writes the data in writebuffer to the SOCKET sock.
    *
    * @author Kelvin Lui
	* @return true if write operation was successful
    */
	bool write();

	/**
    * Copies a QByteArray from src to dest.
    *
    * @author Kelvin Lui
	* @param dest the string to receive the data
	* @param the source string to copy from
	* @param size the size of the src string
	* @return true if write operation was successful
    */
	bool qBinCpy(QByteArray& dest,QByteArray& src,int size);

	/**
    * Accepts a new connection.
    *
    * @author Kelvin Lui
	* @return true if WSAAccept completes successfully
    */
	bool acceptConn();

	
	/** The SOCKET to write and recv from */
    SOCKET sock;

	/** The last accepted SOCKET. Only valid for servers*/
	SOCKET lastAccepted;

	/** The SOCKET to write and recv from */
	sockaddr_in server;

	/** The recipient for data read from the SOCKET sock. */
	QByteArray readBuffer;

	/** Contains the data to be written to the SOCKET sock*/
	QByteArray writeBuffer;

	/** the protocol for this CommSocket. Either TCP or UDP*/
	int prot;

	/** set to true if in multicasting mode*/
    bool multicasting;
	
public:
	/**
    * Creates a new CommSocket
    *
    * @author Kelvin Lui
	* @param host the host to connect to.host should be empty(not NULL) if the caller wants the socket to listen for connections
    */
    CommSocket(QString host, int port,int protocol);
	
	/**
    * Creates a new CommSocket
    *
    * @author Kelvin Lui
	* @param socket the socket to create a new CommSocket from
    */
	CommSocket(SOCKET socket);
    
	/**
    * Destructor for CommSocket
    *
    * @author Kelvin Lui
    */
	virtual ~CommSocket();
	
	/**
    * getLastAcceptedSocket
    *
    * @author Kelvin Lui
	* @return the newest accepted Connection as a CommSocket
    */
	CommSocket* getLastAcceptedSocket();
	
	/**
    * sets the WriteBuffer if it is empty. Data in the write buffer will then be automatically sent
    *
    * @author Kelvin Lui
	* @param data th data to be sent
	* @return returns true if buffer is set successfully
    */
	bool setWriteBuffer(QByteArray data);
    
	/**
    * returns the read buffer
    *
    * @author Kelvin Lui
	* @return the read buffer which contains data read from the SOCKET sock
    */
	QByteArray& getReadBuffer();
	
	/**
    * attempts to establish a connection with the server by calling WSAConnect
    *
    * @author Kelvin Lui
	* @return true if WSAConnect was successful
    */
	bool connectToServ();
	
	/**
    * begins listening for connections
    *
    * @author Kelvin Lui
	* @param backlog the maximum number of connections
	* @return true is listen was successful
    */
	bool listenForConn(int backlog);
    
	/**
    * closes the connection on SOCKET sock
    *
    * @author Kelvin Lui
    */
	void closeSocket();

    /**
     * Toggles this socket to join or leave a multicast group.
     *
     * @author Darryl Pogue
     */
    bool toggleMulticast();

    /**
     * Fills a string and int with the hostname and port for the socket.
     *
     * @author Darryl Pogue
     * @param host The string to contain the hostname.
     * @param port The integer to contain the port.
     */
    void getHostAndPort(QString* host, unsigned short* port);

protected:
	/**
    * win32 message loop
    *
    * @author Kelvin Lui
	* @param message win32 message
	* @param result
	* @return true is message was handled
    */
    virtual bool winEvent(MSG* message, long* result);

signals:
    /**
     * Emitted when the socket has connected.
     *
     * @author Darryl Pogue
     */
    void socketConnected();

    /**
     * Emitted when the remote socket is disconnected.
     *
     * @author Darryl Pogue
     */
    void socketDisconnected();

    /**
     * Emitted when a socket is accepted.
     *
     * @author Darryl Pogue
     */
    void socketAccepted();

    /**
     * Emitted when there is data to be read on the socket.
     *
     * @author Darryl Pogue
     */
    void socketRead();

    /**
     * Emitted when the socket is able to write data.
     *
     * @author Darryl Pogue
     */
    void socketWrite();

    /**
     * Emitted when the socket is closed gracefully.
     *
     * @author Darryl Pogue
     */
    void socketClose();
};
#endif

