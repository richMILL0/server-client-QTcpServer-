#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QTimer>
#include <QListWidgetItem>
#include <QVariant>

TcpServer::TcpServer(QWidget *parent) : QMainWindow(parent) , ui(new Ui::TcpServer) {
    ui->setupUi(this);
    _display();
}

TcpServer::~TcpServer() { delete ui; }


void TcpServer::_display(){
    chat = new QTextEdit(this);
    chat->setReadOnly(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(new QWidget());
    splitter->addWidget(new QWidget());

    buttonOnServer = new QPushButton("Вкл сервер", this);
    buttonOffServer = new QPushButton("Выкл сервер", this);
    buttonDisconnectUser = new QPushButton("Отключить пользователя", this);
    lw_chatExisting = new QListWidget(this);
    lw_users = new QListWidget(this);
    ////=================================================ЛЕВАЯ ЧАСТЬ=================================================
    QHBoxLayout *buttonsnServerLayout = new QHBoxLayout;
    buttonsnServerLayout->addWidget(buttonOnServer);
    buttonsnServerLayout->addWidget(buttonOffServer);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(chat, 1);
    leftLayout->addLayout(buttonsnServerLayout);

    QWidget *leftWidget = splitter->widget(0);
    leftWidget->setLayout(leftLayout);
    ////=================================================ПРАВАЯ ЧАСТЬ=================================================
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(lw_chatExisting, 1);
    rightLayout->addWidget(lw_users, 2);
    rightLayout->addWidget(buttonDisconnectUser);

    QWidget *rightWidget = splitter->widget(1);
    rightWidget->setLayout(rightLayout);

    setCentralWidget(splitter);
    ////=================================================УБРАТЬ ПОТОМ=================================================
    // lw_chatSelection->addItem("Чат 1");
    // lw_chatSelection->addItem("Чат 2");
    ////=================================================КОННЕКТЫ=====================================================
    connect(buttonOnServer, &QPushButton::clicked, this, &TcpServer::_startServer);
    connect(buttonOffServer, &QPushButton::clicked, this, &TcpServer::_stopServer);
    connect(buttonDisconnectUser, &QPushButton::clicked, this, &TcpServer::_disconnectOneUser);
}

void TcpServer::_startServer() {
    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Сервер не запустился" << server->errorString();
        return;
    } else
        qDebug() << "Сервер запущен...";

    server->setMaxPendingConnections(100);
    connect(server, &QTcpServer::newConnection, this, &TcpServer::_newConnection);
}

void TcpServer::_stopServer() {
    if (!server) return;
    QByteArray msg = "STOP_SERVER: Сервер будет выключен через 5 секунд...";
    foreach(QTcpSocket* client, clients) {
        client->write(msg);
        client->flush();
    }

    server->close();
    qDebug() << "Сервер будет выключен через 5 секунд...";

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TcpServer::_disconnectAllUsers);
    timer->start(5000);
}

void TcpServer::_disconnectAllUsers() {
    foreach (QTcpSocket* client, clients) {
        if (client->state() == QTcpSocket::ConnectedState) {
            client->write("SERVER: Вы были отключены от сервера.");
            client->flush();
            client->disconnectFromHost();
            client->close();
            client->deleteLater();
        }
    }
    clients.clear();
    server->deleteLater();
    qDebug() << "Сервер выключен";
}

void TcpServer::_disconnectOneUser() {
    QListWidgetItem* item = lw_users->currentItem();
    if (!item) return;

    QPair<QString, QTcpSocket*> userData = item->data(Qt::UserRole).value<QPair<QString, QTcpSocket*>>();
    QTcpSocket* client = userData.second;

    if (!client) {
        delete item;
        return;
    }

    if (client->state() != QTcpSocket::ConnectedState) {
        delete item;
        return;
    }

    int indx = clients.indexOf(client);
    if (indx != -1) {
        clients.removeAt(indx);
        qDebug() << "Пользователь " << client->peerAddress().toString() << " " << client->peerPort() << " отключен";
    } else {
        qDebug() << "Пользователь не найден в списке clients";
        delete item;
        return;
    }

    client->write("SERVER : Вы были отключены от сервера.");
    client->flush();
    client->disconnectFromHost();
    client->close();
    client->deleteLater();

    delete item;
}

void TcpServer::_newConnection() {
    socket = server->nextPendingConnection();
    clients.append(socket);

    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::_readyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpServer::_clientDisconnect);

    socket->write("Вы успешно подключились");
}

void TcpServer::_readyRead() {
    QTcpSocket* client_sender = qobject_cast<QTcpSocket*>(sender());
    if (!client_sender) return;

    QByteArray data = client_sender->readAll();
    QString message = QString::fromUtf8(data);
    ////================================================РЕГУЛЯРКИ=============================================================
    static QRegularExpression re("^Пользователь\\s+(.+)\\s+(подключился|присоединился)$");
    QRegularExpressionMatch match_connect = re.match(message);

    static QRegularExpression re_create_chat("^CREATE_CHAT:(.+)$");
    QRegularExpressionMatch match_create_chat = re_create_chat.match(message);

    static QRegularExpression re_send_msg("^SEND_MSG:(.+):(.+)$");
    QRegularExpressionMatch match_send_msg = re_send_msg.match(message);

    static QRegularExpression re_invite_chat("^INVITE_CHAT:(.+)$");
    QRegularExpressionMatch match_invite_chat = re_invite_chat.match(message);
    ////======================================================================================================================
    if (match_connect.hasMatch()) {
        QString username = match_connect.captured(1).trimmed();

        QListWidgetItem* item = new QListWidgetItem(username);
        item->setData(Qt::UserRole, QVariant::fromValue(QPair<QString, QTcpSocket*>(username, client_sender)));
        lw_users->addItem(item);

        foreach (QTcpSocket* client, clients) {
            if (client != client_sender && client->state() == QTcpSocket::ConnectedState)
                client->write(message.toUtf8());
        }

        return;
    }

    if (match_create_chat.hasMatch()) {
        QString chatName =match_create_chat.captured(1).trimmed();
        if (chats.find(chatName) == chats.end()) {
            QListWidgetItem* item = new QListWidgetItem(chatName);
            lw_chatExisting->addItem(item);
            chats.insert(chatName, client_sender);
            QByteArray data = ("SERVER_CREATE_CHAT_OK_" + chatName).toUtf8();
            client_sender->write(data);
            client_sender->flush();
        } else {
            QByteArray data = QString("SERVER_CREATE_CHAT_NO_Такой чат уже существует").toUtf8();
            client_sender->write(data);
            client_sender->flush();
        }
        return;
    }

    if (match_send_msg.hasMatch()) {
        QString chatName = match_send_msg.captured(1);
        QString msgContent = match_send_msg.captured(2);
        chatName = chatName.replace(":", " : ");
        chat->append(chatName + " : " + msgContent);
        if (chats.contains(chatName)) {
            foreach (QTcpSocket* client, clients) {
                if (client != client_sender && client->state() == QTcpSocket::ConnectedState) {
                    client->write(message.toUtf8());
                    client->flush();
                }
            }
        }
        return;
    }

    if (match_invite_chat.hasMatch()) {
        QString invite_username = match_invite_chat.captured(1);
        QString chatName = match_invite_chat.captured(2);

        return;
    }
    ////======================================если ни один блок не сработал==================================================
    foreach (QTcpSocket* client, clients) {
        if (client != client_sender && client->state() == QTcpSocket::ConnectedState)
            client->write(message.toUtf8());
    }
    chat->append(message);
}

void TcpServer::_clientDisconnect() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    clients.removeOne(client);
    client->deleteLater();
    qDebug() << "Клиент отключился:" << client->peerAddress().toString();
}

QTcpSocket* TcpServer::_findSocketByUsername(const QString& username) {
    for (int i = 0; i < lw_users->count(); ++i) {
        QListWidgetItem* item = lw_users->item(i);
        QPair<QString, QTcpSocket*> data = item->data(Qt::UserRole).value<QPair<QString, QTcpSocket*>>();
        if (data.first == username) {
            return data.second;
        }
    }
    return nullptr;
}
