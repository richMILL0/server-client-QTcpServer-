#ifndef CLIENT_H
#define CLIENT_H

#include "chatmodel.h"
#include "chatmessagedelegate.h"
#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QRegularExpression>
#include <QDateTime>
class QTcpSocket;
class QListWidget;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QSplitter;
class QListWidgetItem;
class QListView;

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE














class Client : public QMainWindow {
    Q_OBJECT

public:


    explicit Client(QWidget *parent = nullptr);
    ~Client();
    void set_data(const QString, const QString);

private:

    void _connectToServer(const QString&,  quint16);
    void _display();
    void _sendMessage();
    void _readyRead();
    void _connectNewUser();
    void _createNewChat();
    void _invite_to_Chat();
    void _setCurrentChatName();
    void _str_message_change();
    void _sendFile();

    QListWidgetItem* _findItemByChatName(QListWidget*, const QString&);

    QTcpSocket* socket;

    QString username;
    QString password;

    QString currentChatName;
    QHash<QString, ChatModel*> chatModels;

    ChatMessageDelegate* chatMessageDelegate;
    QListWidget* listChats;
    QListView* chat;
    QLineEdit* str_message;
    QPushButton* buttonSend;
    QPushButton* buttonCreateNewChat;
    QPushButton* buttonJoinChat;
    QPushButton* buttonSelectFile;
    QPushButton* buttonEmoji;
    QSplitter* splitter;

    Ui::Client *ui;
};


#endif // CLIENT_H





