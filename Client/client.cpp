#include "client.h"
#include "ui_client.h"
#include "createchat.h"
#include "invite.h"
#include <QTcpSocket>
#include <QTcpSocket>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QByteArray>
#include <QMessageBox>
#include <QListWidgetItem>

Client::Client(QWidget *parent) : QMainWindow(parent), ui(new Ui::Client) {
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    // socket->connectToHost("192.168.0.14", 1234);
    socket->connectToHost("127.0.0.1", 1234);
    connect(socket, &QTcpSocket::connected, this, &Client::_connectNewUser);
    connect(socket, &QTcpSocket::readyRead, this, &Client::_readyRead);
    _display();
}

Client::~Client() { delete ui; }

void Client::_connectNewUser() {
    QString lineNewUser = "Пользователь " + username + " присоединился";
    QByteArray data = lineNewUser.toUtf8();
    socket->write(data);
}

void Client::_display() {
    splitter = new QSplitter(Qt::Horizontal, this);
    listChats = new QListWidget(this);
    chat = new QTextEdit(this);
    buttonSend = new QPushButton("Отправить", this);
    str_message = new QLineEdit(this);
    buttonCreateNewChat = new QPushButton("Создать чат", this);
    buttonJoinChat = new QPushButton("Пригласить", this);
    ////=========================================НАСТРОЙКА КОМПОНЕНТОВ(ГЛУБО ГОВОРЯ)===========================================
    chat->setReadOnly(true);
    str_message->setReadOnly(true);
    str_message->setPlaceholderText("Ввести сообщение");
    buttonSend->setFixedSize(30, 30);
    ////================================================ЛЕВАЯ ЧАСТЬ============================================================
    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(buttonCreateNewChat);
    buttonLayout->addWidget(buttonJoinChat);
    buttonWidget->setLayout(buttonLayout);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(listChats, 9);
    leftLayout->addWidget(buttonWidget, 1);

    QWidget* leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    ////================================================ПРАВАЯ ЧАСТЬ===========================================================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(chat, 9);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(str_message, 8);
    bottomLayout->addWidget(buttonSend, 2);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addLayout(bottomLayout, 1);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);
    ///=========================================================================================================================
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    setCentralWidget(splitter);
    ////=================================================КОННЕКТЫ===============================================================
    connect(buttonSend, &QPushButton::clicked, this, &Client::_sendMessage);
    connect(str_message, &QLineEdit::returnPressed, this, &Client::_sendMessage);
    connect(buttonCreateNewChat, &QPushButton::clicked, this, &Client::_createNewChat);
    connect(buttonJoinChat, &QPushButton::clicked, this, &Client::_invite_to_Chat);
    connect(listChats, &QListWidget::itemClicked, this, &Client::_setCurrentChatName);
    connect(listChats, &QListWidget::itemClicked, this, &Client::_str_message_change);
}

void Client::_sendMessage() {
    QString messageText = str_message->text();
    if (messageText.isEmpty()) return;

    QString message = username + " : " + messageText;
    QString internalMessage = "SEND_MSG:" + currentChatName + ":" + message;
    QByteArray data = internalMessage.toUtf8();

    socket->write(data);
    socket->flush();

    chat->append(message);
    chatHistory[currentChatName].append(message);
    str_message->clear();
}

void Client::set_data(const QString username_, const QString password_) { username = username_; password = password_; }

void Client::_readyRead() {
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    ////================================================РЕГУЛЯРКИ=============================================================
    static QRegularExpression re_chat_ok("^SERVER_CREATE_CHAT_OK_(.+)$");
    QRegularExpressionMatch match_ok_chat = re_chat_ok.match(message.trimmed());
    static QRegularExpression re_chat_error("^SERVER_CREATE_CHAT_NO_Такой чат уже существует$");
    QRegularExpressionMatch match_error_chat = re_chat_error.match(message.trimmed());
    static QRegularExpression re_send_msg("^SEND_MSG:(.+):(.+)$");
    QRegularExpressionMatch match_send_msg = re_send_msg.match(message.trimmed());
    ////======================================================================================================================
    if (match_ok_chat.hasMatch()) {
        QString chatName = match_ok_chat.captured(1);
        QListWidgetItem* item = new QListWidgetItem(chatName);
        listChats->addItem(item);
        return;
    }
    if (match_error_chat.hasMatch()) {
        QMessageBox::warning(this, "Ошибка создания чата", "Чат с таким именем уже существует.");
        return;
    }
    if (match_send_msg.hasMatch()) {
        QString chatName = match_send_msg.captured(1);
        QString msg = match_send_msg.captured(2);
        chatHistory[chatName].append(msg);

        if (chatName == currentChatName) {
            chat->append(msg);
        }
        return;
    }
    if (!message.isEmpty()) {
        chat->append(message);
    }
}

void Client::_createNewChat() {
    CreateChatDialog t(this);
    if (t.exec() == QDialog::Accepted) {
        QString name = t.get_chatName();
        QByteArray data = ("CREATE_CHAT:" + name).toUtf8();
        if (socket->state() == QTcpSocket::ConnectedState) {
            socket->write(data);
            socket->flush();
        }
    }
}

void Client::_invite_to_Chat() {
    Invite t(this);
    if (t.exec() == QDialog::Accepted) {
        QString invite_username = t.get_username();
        QByteArray data = ("INVITE_USER:" + invite_username).toUtf8();
        if (socket->state() == QTcpSocket::ConnectedState) {
            socket->write(data);
            socket->flush();
        }
    }
}

void Client::_setCurrentChatName() {
    QListWidgetItem* item = listChats->currentItem();
    QString selectedChatName = item->text();

    if (!currentChatName.isEmpty()) {
        chatHistory[currentChatName] = chat->toPlainText().split("\n");
    }

    currentChatName = selectedChatName;
    chat->clear();
    if (chatHistory.contains(currentChatName)) {
        for (auto& msg : chatHistory[currentChatName]) {
            if (!msg.isEmpty()) {
                chat->append(msg);
            }
        }
    } else {
        chatHistory[currentChatName] = QStringList();
    }
}

void Client::_str_message_change() {
    str_message->setReadOnly(false);
}
