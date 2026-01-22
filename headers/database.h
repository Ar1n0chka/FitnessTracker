#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDate>
#include <QVariant>

class DatabaseManager {
public:
    static bool initDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        db.setDatabaseName(path + "/fitness_tracker.db");

        if (!db.open()) return false;

        QSqlQuery query;
        // 1. База продуктов (добавим пару тестовых продуктов сразу)
        query.exec("CREATE TABLE IF NOT EXISTS food_library ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT, calories REAL, protein REAL, fat REAL, carbs REAL)");

        // Проверка: если таблица пустая, добавим тестовые данные
        query.exec("SELECT count(*) FROM food_library");
        if (query.next() && query.value(0).toInt() == 0) {
            query.exec("INSERT INTO food_library (name, calories, protein, fat, carbs) VALUES "
                       "('Куриная грудка', 113, 23.6, 1.9, 0.4),"
                       "('Гречка отварная', 101, 3.8, 1.1, 20.5),"
                       "('Яблоко', 52, 0.3, 0.2, 13.8),"
                       "('Творог 5%', 121, 17.2, 5.0, 1.8)");
        }

        // 2. Дневник питания (расширенный)
        // meal_type: 1=Завтрак, 2=Обед, 3=Ужин, 4=Перекус
        query.exec("CREATE TABLE IF NOT EXISTS nutrition_log ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "date_entry TEXT, "    // Формат YYYY-MM-DD
                   "meal_type INTEGER, "
                   "food_name TEXT, "
                   "weight REAL, "
                   "kcal REAL, protein REAL, fat REAL, carbs REAL)");

        return true;
    }

    // Метод добавления записи
    static void addEntry(QDate date, int mealType, QString name, double weight, double k, double p, double f, double c) {
        QSqlQuery query;
        query.prepare("INSERT INTO nutrition_log (date_entry, meal_type, food_name, weight, kcal, protein, fat, carbs) "
                      "VALUES (:date, :type, :name, :w, :k, :p, :f, :c)");
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        query.bindValue(":type", mealType);
        query.bindValue(":name", name);
        query.bindValue(":w", weight);
        query.bindValue(":k", k); query.bindValue(":p", p); query.bindValue(":f", f); query.bindValue(":c", c);
        query.exec();
    }
};

#endif // DATABASE_H
