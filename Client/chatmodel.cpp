#include "chatmodel.h"

ChatModel::ChatModel(QObject *parent) : QAbstractListModel{parent} {}

void ChatModel::addMessage(const ChatMessage_ &msg) {
    beginInsertRows(QModelIndex(), messages.size(), messages.size());
    messages.append(msg);
    endInsertRows();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= messages.size())
        return {};

    const ChatMessage_ &msg = messages[index.row()];

    switch(role) {
        case TextRole: return msg.message;
        case AuthorRole: return msg.author;
        case TimeRole: return msg.time;
        default: return {};
    }
}

int ChatModel::rowCount(const QModelIndex &parent) const {Q_UNUSED(parent) return messages.size(); }

QHash<int, QByteArray> ChatModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole]  = "text";
    roles[AuthorRole] = "author";
    roles[TimeRole] = "time";
    return roles;
}
