#ifndef CHATMESSAGEDELEGATE_H
#define CHATMESSAGEDELEGATE_H

#include <QStyledItemDelegate>

class ChatMessageDelegate : public QStyledItemDelegate {
public:
    explicit ChatMessageDelegate(QObject *parent = nullptr);

    void setMyUsername(const QString &username);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:

    QString myUsername;
    QSizeF layoutText(const QString &text, const QFont &font, qreal maxWidth) const;
};

#endif // CHATMESSAGEDELEGATE_H
