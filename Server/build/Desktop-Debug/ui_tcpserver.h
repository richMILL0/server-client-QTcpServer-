/********************************************************************************
** Form generated from reading UI file 'tcpserver.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPSERVER_H
#define UI_TCPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpServer
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *TcpServer)
    {
        if (TcpServer->objectName().isEmpty())
            TcpServer->setObjectName("TcpServer");
        TcpServer->resize(800, 600);
        centralwidget = new QWidget(TcpServer);
        centralwidget->setObjectName("centralwidget");
        TcpServer->setCentralWidget(centralwidget);
        menubar = new QMenuBar(TcpServer);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 43));
        TcpServer->setMenuBar(menubar);
        statusbar = new QStatusBar(TcpServer);
        statusbar->setObjectName("statusbar");
        TcpServer->setStatusBar(statusbar);

        retranslateUi(TcpServer);

        QMetaObject::connectSlotsByName(TcpServer);
    } // setupUi

    void retranslateUi(QMainWindow *TcpServer)
    {
        TcpServer->setWindowTitle(QCoreApplication::translate("TcpServer", "TcpServer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpServer: public Ui_TcpServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPSERVER_H
