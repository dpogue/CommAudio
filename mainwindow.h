#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

class CommAudio : public QMainWindow
{
	Q_OBJECT

public:
	CommAudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CommAudio();

private:
	Ui::CommAudioClass ui;
};

#endif // MAINWINDOW_H
