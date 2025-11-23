#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QRegularExpression>
class QTcpSocket;
class QListWidget;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QSplitter;
class QListWidgetItem;

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
    QListWidgetItem* _findItemByChatName(QListWidget* listWidget, const QString& chatName);

    QTcpSocket* socket;

    QString username;
    QString password;

    QString currentChatName;
    QString buffer;
    QMap<QString, QStringList> chatHistory;

    QListWidget* listChats;
    QTextEdit* chat;
    QLineEdit* str_message;
    QPushButton* buttonSend;
    QPushButton* buttonCreateNewChat;
    QPushButton* buttonJoinChat;
    QSplitter* splitter;

    Ui::Client *ui;
};

#endif // CLIENT_H
