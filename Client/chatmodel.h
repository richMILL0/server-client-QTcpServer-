#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
class QVariant;

struct ChatMessage_ {
    QString author;
    QString message;
    QDateTime time;
};

enum role {
    TextRole        = 1,
    AuthorRole      = 2,
    TimeRole        = 3
};

class ChatModel : public QAbstractListModel {
public:
    explicit ChatModel(QObject *parent = nullptr);
    void addMessage(const ChatMessage_&);
private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex&, int) const override;

    QVector<ChatMessage_> messages;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // CHATMODEL_H
