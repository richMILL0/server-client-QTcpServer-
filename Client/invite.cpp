#include "invite.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

Invite::Invite(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Пригласить пользователя");
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *label = new QLabel(tr("Введите ник пользователя:"), this);
    nicknameEdit = new QLineEdit(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton(tr("Отправить"), this);
    cancelButton = new QPushButton(tr("Отмена"), this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(label);
    mainLayout->addWidget(nicknameEdit);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &Invite::_okClick);
    connect(cancelButton, &QPushButton::clicked, this, &Invite::_cancelClick);
    connect(nicknameEdit, &QLineEdit::returnPressed, this, &Invite::_okClick);
}

void Invite::_okClick() {
    if (!nicknameEdit->text().trimmed().isEmpty())
        accept();
}

void Invite::_cancelClick() { reject(); }

QString Invite::get_username() const { return nicknameEdit->text().trimmed(); }
