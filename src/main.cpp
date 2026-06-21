#include "mainwindow.h"
#include "database.h"
#include "logindialog.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::initDatabase()) return -1;

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        QString nickname = login.getNickname();

        // Защита от пустого ввода
        if (nickname.isEmpty()) {
            nickname = "default_user";
        }

        // Ищем пользователя в базе
        int userId = DatabaseManager::getUserId(nickname);

        // Если такого нет — создаем
        if (userId == -1) {
            userId = DatabaseManager::addUser(nickname);
            if (userId == -1) {
                QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать пользователя!");
                return -1;
            }
        }

        // Устанавливаем его как активного для всей остальной программы
        DatabaseManager::setCurrentUserId(userId);

        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0; // Закрываем программу, если пользователь отменил вход
}
