#include "client.h"
#include "userdata.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    UserData dialog;
    if (dialog.exec() != QDialog::Accepted) {
        return 0;
    }

    Client w;
    w.set_data(dialog.get_username(), dialog.get_password());
    w.show();

    return a.exec();
}
