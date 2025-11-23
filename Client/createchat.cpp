#include "createchat.h"
#include <QVBoxLayout>
#include <QLabel>

CreateChatDialog::CreateChatDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Создать чат");

    le_editName = new QLineEdit(this);
    QPushButton *buttonOk = new QPushButton("Создать", this);
    QPushButton *buttonCancel = new QPushButton("Отмена", this);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(buttonOk);
    buttons->addWidget(buttonCancel);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel("Название чата:"));
    layout->addWidget(le_editName);
    layout->addLayout(buttons);

    setLayout(layout);

    connect(buttonOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString CreateChatDialog::get_chatName() const { return le_editName->text(); }
