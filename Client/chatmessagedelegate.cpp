#include "chatmessagedelegate.h"
#include "chatmodel.h"
#include <QPainter>
#include <QApplication>
#include <QTextLayout>

ChatMessageDelegate::ChatMessageDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void ChatMessageDelegate::setMyUsername(const QString &username) { myUsername = username; }

QSizeF ChatMessageDelegate::layoutText(const QString &text, const QFont &font, qreal maxWidth) const {
    QTextLayout layout(text, font);
    layout.beginLayout();

    qreal height = 0;
    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid()) break;
        line.setLineWidth(maxWidth);
        height += line.height();
    }

    layout.endLayout();
    return QSizeF(maxWidth, height);
}

QSize ChatMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QString text = index.data(1).toString();

    QFont font = option.font;
    font.setPointSize(14);

    qreal maxWidth = option.rect.width() * 0.6;
    QSizeF textSize = layoutText(text, font, maxWidth);

    int bubblePadding = 20;
    int totalHeight = textSize.height() + bubblePadding * 2 + 20;

    return QSize(option.rect.width(), totalHeight);
}

void ChatMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QString text = index.data(1).toString();
    QString author = index.data(2).toString();

    bool my = (author == myUsername);

    QFont font = option.font;
    font.setPointSize(14);
    painter->setFont(font);

    qreal maxWidth = option.rect.width() * 0.6;
    QSizeF textSize = layoutText(text, font, maxWidth);

    int pad = 20;
    int bubbleWidth = textSize.width() + pad * 2;
    int bubbleHeight = textSize.height() + pad * 2;

    QRect bubbleRect;
    if (my) {
        bubbleRect = QRect(option.rect.right() - bubbleWidth - 10, option.rect.top() + 20, bubbleWidth, bubbleHeight);
        painter->setBrush(QColor("#DCF8C6"));
    } else {
        bubbleRect = QRect(option.rect.left() + 10, option.rect.top() + 20, bubbleWidth, bubbleHeight);
        painter->setBrush(QColor("#FFFFFF"));
    }

    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(bubbleRect, 16, 16);

    painter->setPen(Qt::black);

    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);
    painter->drawText(bubbleRect.adjusted(pad, pad, -pad, -pad), text, opt);

    painter->setPen(Qt::gray);
    painter->setFont(QFont(option.font.family(), 10));
    painter->drawText(option.rect.adjusted(10, 0, -10, 0), my ? Qt::AlignRight : Qt::AlignLeft, author);

    painter->restore();
}
