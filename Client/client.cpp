#include "client.h"
#include "ui_client.h"
#include "createchat.h"
#include "invite.h"
#include "chatmodel.h"
#include "chatmessagedelegate.h"
#include <QTcpSocket>
#include <QFile>
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
#include <QListView>
#include <QDateTime>

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
    chat = new QListView(this);
    chatMessageDelegate = new ChatMessageDelegate(chat);
    buttonSend = new QPushButton("", this);
    str_message = new QLineEdit(this);
    buttonCreateNewChat = new QPushButton("Создать чат", this);
    buttonJoinChat = new QPushButton("Пригласить", this);
    buttonSelectFile = new QPushButton("", this);
    buttonEmoji = new QPushButton("", this);
    ////=========================================НАСТРОЙКА КОМПОНЕНТОВ(ГЛУБО ГОВОРЯ)===========================================
    chat->setItemDelegate(chatMessageDelegate);
    chat->setResizeMode(QListView::Adjust);
    chat->setUniformItemSizes(false);
    chat->setWordWrap(true);
    chat->setSpacing(12);
    chat->scrollToBottom();

    str_message->setEnabled(false);
    str_message->setPlaceholderText("Ввести сообщение");

    buttonSelectFile->setFixedSize(30, 30);
    buttonEmoji->setFixedSize(30, 30);
    buttonSend->setFixedSize(30, 30);
    buttonSelectFile->setObjectName("ButtonSelectFile");
    buttonEmoji->setObjectName("ButtonEmoji");
    buttonSend->setObjectName("ButtonSend");
    buttonCreateNewChat->setObjectName("buttonCreateNewChat");
    buttonJoinChat->setObjectName("buttonJoinChat");
    ////================================================ЛЕВАЯ ЧАСТЬ============================================================
    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 7, 20);
    buttonLayout->addWidget(buttonCreateNewChat);
    buttonLayout->addWidget(buttonJoinChat);
    buttonWidget->setLayout(buttonLayout);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(7, 4, 0, 4);
    leftLayout->addWidget(listChats, 9);
    leftLayout->addWidget(buttonWidget, 0);

    QWidget* leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    ////================================================ПРАВАЯ ЧАСТЬ===========================================================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(chat, 9);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(buttonSelectFile);
    str_message->setContentsMargins(0, 20, 0, 0);
    bottomLayout->addWidget(str_message);
    bottomLayout->addWidget(buttonEmoji);
    bottomLayout->addWidget(buttonSend);
    bottomLayout->setContentsMargins(4, 0, 0, 0);
    rightLayout->setContentsMargins(0, 4, 13, 10);
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
    connect(buttonSelectFile, &QPushButton::clicked, this, &Client::_sendFile);
}

void Client::_sendMessage() {
    QString messageText = str_message->text().trimmed();
    if (messageText.isEmpty()) return;

    QString fullMessage = QString("SEND_MSG:%1:%2:%3").arg(currentChatName, username, messageText);
    socket->write(fullMessage.toUtf8());
    socket->flush();

    chatModels[currentChatName]->addMessage(ChatMessage_{username, messageText, QDateTime::currentDateTime()});
    str_message->clear();
}


void Client::set_data(const QString username_, const QString password_) {
    username = username_;
    password = password_;

    if (chatMessageDelegate)
        chatMessageDelegate->setMyUsername(username);
}

void Client::_readyRead() {
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    ////======================================================РЕГУЛЯРКИ=========================================================
    static QRegularExpression re_chat_ok("SERVER_CREATE_CHAT_OK_(.+)");
    QRegularExpressionMatch match_ok_chat = re_chat_ok.match(message);

    static QRegularExpression re_chat_error("^SERVER_CREATE_CHAT_NO_Такой чат уже существует$");
    QRegularExpressionMatch match_error_chat = re_chat_error.match(message.trimmed());

    static QRegularExpression re_send_msg("^SEND_MSG:([^:]+):([^:]+):([\\s\\S]+)$");
    QRegularExpressionMatch match_send_msg = re_send_msg.match(message);

    static QRegularExpression re_history("^SERVER_CHAT_HISTORY:([^\\n]+)\\n([\\s\\S]*)$");
    QRegularExpressionMatch match_history = re_history.match(message);

    //===========================================Одобрение создания чата======================================================
    if (match_ok_chat.hasMatch()) {
        QString chatName = match_ok_chat.captured(1).trimmed();
        if (!chatModels.contains(chatName)) {
            chatModels[chatName] = new ChatModel(this);
            connect(chatModels[chatName], &QAbstractItemModel::rowsInserted, chat, &QAbstractItemView::scrollToBottom);
        }
        bool exist = false;
        for (int i = 0; i < listChats->count(); ++i)
            if (listChats->item(i)->text() == chatName) { exist = true; break; }
        if (!exist)
            listChats->addItem(new QListWidgetItem(chatName));
        return;
    }
    //===========================================Отказ создания чата===========================================================
    if (match_error_chat.hasMatch()) {
        QMessageBox::warning(this, "Ошибка создания чата", "Чат с таким именем уже существует.");
        return;
    }
    //===========================================Отправка сообщения============================================================
    if (match_send_msg.hasMatch()) {
        QString chatName = match_send_msg.captured(1).trimmed();
        QString author = match_send_msg.captured(2).trimmed();
        QString text = match_send_msg.captured(3);

        if (!chatModels.contains(chatName)) {
            chatModels[chatName] = new ChatModel(this);
            connect(chatModels[chatName], &QAbstractItemModel::rowsInserted, chat, &QAbstractItemView::scrollToBottom);
        }

        chatModels[chatName]->addMessage(ChatMessage_{author, text, QDateTime::currentDateTime()});

        bool exist = false;
        for (int i = 0; i < listChats->count(); ++i)
            if (listChats->item(i)->text() == chatName) { exist = true; break; }
        if (!exist)
            listChats->addItem(new QListWidgetItem(chatName));
        return;
    }
    //===========================================Подгрузка истории============================================================
    if (match_history.hasMatch()) {
        QString chatName = match_history.captured(1).trimmed();
        QStringList hist = match_history.captured(2).split("\n", Qt::SkipEmptyParts);

        if (!chatModels.contains(chatName)) {
            chatModels[chatName] = new ChatModel(this);
            connect(chatModels[chatName], &QAbstractItemModel::rowsInserted, chat, &QAbstractItemView::scrollToBottom);
        }

        for (auto &line : hist) {
            QStringList parts = line.split(":", Qt::KeepEmptyParts);
            QString author = parts.value(0);
            QString text = parts.mid(1).join(":");
            chatModels[chatName]->addMessage(ChatMessage_{author, text, QDateTime::currentDateTime()});
        }

        bool exist = false;
        for (int i = 0; i < listChats->count(); ++i)
            if (listChats->item(i)->text() == chatName) { exist = true; break; }
        if (!exist)
            listChats->addItem(new QListWidgetItem(chatName));

        if (currentChatName == chatName)
            chat->setModel(chatModels[chatName]);

        return;
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

        if (currentChatName.isEmpty()) {
            QMessageBox::warning(this, "Приглашение", "Сначала выберите чат, в который хотите пригласить пользователя.");
            return;
        }

        QByteArray data = ("INVITE_USER:" + invite_username + ":" + currentChatName).toUtf8();
        if (socket->state() == QTcpSocket::ConnectedState) {
            socket->write(data);
            socket->flush();
        }
    }
}

void Client::_setCurrentChatName() {
    QListWidgetItem* item = listChats->currentItem();
    if (!item)
        return;
    QString selectedChatName = item->text();

    if (!chatModels.contains(selectedChatName)) {
        chatModels[selectedChatName] = new ChatModel(this);
        connect(chatModels[selectedChatName], &QAbstractItemModel::rowsInserted, chat, &QAbstractItemView::scrollToBottom);
    }

    chat->setModel(chatModels[selectedChatName]);
    currentChatName = selectedChatName;

    str_message->setEnabled(true);
}

void Client::_sendFile() {

}

void Client::_str_message_change() { str_message->setEnabled(true); }
