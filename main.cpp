#include "mainwindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CommAudio w;
	w.show();
	return a.exec();
}
