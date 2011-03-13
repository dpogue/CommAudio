/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Mar 13 14:30:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommAudioClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CommAudioClass)
    {
        if (CommAudioClass->objectName().isEmpty())
            CommAudioClass->setObjectName(QString::fromUtf8("CommAudioClass"));
        CommAudioClass->resize(600, 400);
        menuBar = new QMenuBar(CommAudioClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        CommAudioClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CommAudioClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        CommAudioClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(CommAudioClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        CommAudioClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CommAudioClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        CommAudioClass->setStatusBar(statusBar);

        retranslateUi(CommAudioClass);

        QMetaObject::connectSlotsByName(CommAudioClass);
    } // setupUi

    void retranslateUi(QMainWindow *CommAudioClass)
    {
        CommAudioClass->setWindowTitle(QApplication::translate("CommAudioClass", "CommAudio", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CommAudioClass: public Ui_CommAudioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
