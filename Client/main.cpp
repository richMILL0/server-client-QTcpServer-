#include "client.h"
#include "userdata.h"
#include <QApplication>
#include <QFile>

void _loadStyle(QApplication &app) {
    QFile styleFile(":/main.qss");
    if (!styleFile.open(QFile::ReadOnly)) {
        qWarning("Не удалось открыть QSS файл");
        return;
    }
    qWarning("Норм все");
    QString style = styleFile.readAll();
    app.setStyleSheet(style);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    _loadStyle(a);

    UserData dialog;
    if (dialog.exec() != QDialog::Accepted) {
        return 0;
    }

    Client w;
    w.set_data(dialog.get_username(), dialog.get_password());
    w.show();

    return a.exec();
}
