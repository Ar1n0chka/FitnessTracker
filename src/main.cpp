#include "mainwindow.h"
#include "database.h"       // Добавляем этот инклуд
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Инициализируем базу данных перед открытием окна
    if (!DatabaseManager::initDatabase()) {
        // Можно добавить вывод ошибки, если база не открылась
    }

    MainWindow w;
    w.show();
    return a.exec();
}
