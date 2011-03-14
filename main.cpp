#include <QtGui/QApplication>
#include "mainwindow.h"
#include "commsocket.h"
#include "defines.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WSADATA ws;
	WSAStartup(0x0202,&ws);
	CommAudio w;
	CommSocket cs("",7000,TCP);
	w.show();
	return a.exec();
}
