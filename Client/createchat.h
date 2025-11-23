#ifndef CREATECHAT_H
#define CREATECHAT_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class CreateChatDialog : public QDialog {

public:
    explicit CreateChatDialog(QWidget *parent = nullptr);

    QString get_chatName() const;

private:
    QLineEdit *le_editName;
};


#endif // CREATECHAT_H
