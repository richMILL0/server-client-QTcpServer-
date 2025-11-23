#ifndef INVITE_H
#define INVITE_H

#include <QDialog>
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class Invite : public QDialog {

public:
    explicit Invite(QWidget *parent = nullptr);


    void _okClick();
    void _cancelClick();
    QString get_username() const;

private:

    QLineEdit *nicknameEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};


#endif // INVITE_H
