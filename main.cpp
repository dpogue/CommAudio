#include <QtGui/QApplication>
#include <qthreadpool.h>
#include "mainwindow.h"
#include "commsocket.h"
#include "defines.h"

int main(int argc, char *argv[]) {

	QApplication a(argc, argv);
	WSADATA ws;
	WSAStartup(0x0202,&ws);
	CommAudio w;
    QThreadPool::globalInstance()->setMaxThreadCount(16);
	w.show();
	return a.exec();
}
