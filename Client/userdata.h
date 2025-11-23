#ifndef USERDATA_H
#define USERDATA_H

#include <QDialog>
#include <QLineEdit>
#include <QTabWidget>

class UserData : public QDialog {

public:

    enum Mode {
        Login,
        Register
    };

    explicit UserData(QWidget *parent = nullptr);

    Mode get_mode() const;
    QString get_username() const;
    QString get_password() const;

    void _display();
    void _buttonClicked();

private:

    QString username;
    QString password;

    QTabWidget *tabWidget = nullptr;
    QLineEdit *le_loginUsername;
    QLineEdit *le_loginPassword;
    QLineEdit *le_registerUsername;
    QLineEdit *le_registerPassword;
    Mode currentMode;
};

#endif // USERDATA_H
