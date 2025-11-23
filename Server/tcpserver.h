#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QHash>
class QObject;
class QTcpServer;
class QTcpSocket;
class QPushButton;
class QListWidget;
class QTextEdit;

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpServer;
}
QT_END_NAMESPACE

class TcpServer : public QMainWindow {
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

private:
    void _startServer();
    void _stopServer();
    void _disconnectAllUsers();
    void _disconnectOneUser();
    void _newConnection();
    void _readyRead();
    void _clientDisconnect();
    void _display();
    QTcpSocket* _findSocketByUsername(const QString&);

    QTcpServer *server;
    QList<QTcpSocket*> clients;
    QTcpSocket* socket;

    QString username;

    QMap<QString, QTcpSocket*> chats;

    QTextEdit* chat;
    QPushButton* buttonOnServer;
    QPushButton* buttonOffServer;
    QPushButton* buttonDisconnectUser;
    QListWidget* lw_chatExisting;
    QListWidget* lw_users;

    Ui::TcpServer *ui;
};

#endif // TCPSERVER_H
