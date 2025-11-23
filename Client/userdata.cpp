#include "userdata.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>

UserData::UserData(QWidget *parent) : QDialog(parent), currentMode(Login) {
    _display();
    currentMode = Register;
}

void UserData::_display() {
    setWindowTitle("Авторизация");
    setFixedSize(400, 300);
    tabWidget = new QTabWidget(this);
    QPushButton *continueButton = new QPushButton("Продолжить", this);
    ////==================================================ВХОД=================================================================
    QWidget *loginTab = new QWidget();
    le_loginUsername = new QLineEdit();
    le_loginPassword = new QLineEdit();
    le_loginPassword->setEchoMode(QLineEdit::Password);

    QFormLayout *loginLayout = new QFormLayout();
    loginLayout->addRow("Введите username:", le_loginUsername);
    loginLayout->addRow("Введите пароль:", le_loginPassword);
    loginTab->setLayout(loginLayout);
    ////==================================================РЕГИСТРАЦИЯ==========================================================
    QWidget *registerTab = new QWidget();
    le_registerUsername = new QLineEdit();
    le_registerPassword = new QLineEdit();
    le_registerPassword->setEchoMode(QLineEdit::Password);

    QFormLayout *registerLayout = new QFormLayout();
    registerLayout->addRow("Введите username:", le_registerUsername);
    registerLayout->addRow("Придумайте пароль:", le_registerPassword);
    registerTab->setLayout(registerLayout);
    ////=======================================================================================================================
    tabWidget->addTab(registerTab, "Регистрация");
    tabWidget->addTab(loginTab, "Вход");

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(continueButton, Qt::AlignCenter);

    setLayout(mainLayout);
    ////==================================================КОННЕКТЫ=============================================================
    connect(continueButton, &QPushButton::clicked, this, &UserData::_buttonClicked);
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        currentMode = (index == 1) ? Login : Register;
    });
}

void UserData::_buttonClicked() {
    username = (currentMode == Login) ? le_loginUsername->text() : le_registerUsername->text();
    password = (currentMode == Login) ? le_loginPassword->text() : le_registerPassword->text();
    accept();
}

UserData::Mode UserData::get_mode() const { return currentMode; }

QString UserData::get_username() const { return username; }

QString UserData::get_password() const { return password; }
