#include "database.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QSqlError>

static int g_currentUserId = 1;

bool DatabaseManager::initDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = getDatabasePath();

    if (!ensureDatabaseDirectory()) {
        qWarning() << "Не удалось создать директорию для базы данных!";
        return false;
    }

    db.setDatabaseName(dbPath);
    qDebug() << "Путь к базе данных:" << dbPath;

    if (!db.open()) {
        qCritical() << "Ошибка открытия базы данных:" << db.lastError().text();
        return false;
    }

    if (!createTables()) {
        qCritical() << "Ошибка создания таблиц!";
        return false;
    }

    if (!createDefaultUser()) {
        qWarning() << "Не удалось создать пользователя по умолчанию";
    }

    populateDefaultFoods();
    populateDefaultExercises();


    qDebug() << "База данных успешно инициализирована!";
    return true;
}

QSqlDatabase DatabaseManager::database()
{
    return QSqlDatabase::database();
}

QString DatabaseManager::getDatabasePath()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/fitness_tracker.db";
}

bool DatabaseManager::ensureDatabaseDirectory()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir().mkpath(path);
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;




    query.exec("CREATE TABLE IF NOT EXISTS body_measurements (id INTEGER PRIMARY KEY, user_id INTEGER, date_entry TEXT, weight REAL, height REAL, waist REAL, hips REAL, neck REAL, bmi REAL, fat_percent REAL)");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            username        TEXT UNIQUE NOT NULL,
            email           TEXT UNIQUE,
            password_hash   TEXT,
            birth_date      TEXT,
            gender          TEXT CHECK(gender IN ('male','female','other')),
            height_cm       REAL,
            created_at      TEXT DEFAULT (datetime('now')),
            last_sync       TEXT
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS food_library (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            name            TEXT NOT NULL UNIQUE,
            calories_per_100g REAL NOT NULL,
            protein_g       REAL,
            fat_g           REAL,
            carbs_g         REAL,
            is_user_added   INTEGER DEFAULT 0,
            source          TEXT,
            created_by      INTEGER REFERENCES users(id)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS nutrition_log (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id         INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            date_entry      TEXT NOT NULL,
            meal_type       INTEGER NOT NULL,
            food_name       TEXT NOT NULL,
            food_id         INTEGER REFERENCES food_library(id),
            weight_g        REAL NOT NULL,
            kcal            REAL NOT NULL,
            protein_g       REAL,
            fat_g           REAL,
            carbs_g         REAL,
            created_at      TEXT DEFAULT (datetime('now')),
            UNIQUE(user_id, date_entry, meal_type, food_name)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS exercise_library (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            name            TEXT NOT NULL UNIQUE,
            category        TEXT NOT NULL,
            muscle_group    TEXT,
            met_value       REAL,
            description     TEXT,
            is_user_added   INTEGER DEFAULT 0,
            created_by      INTEGER REFERENCES users(id)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS workout_sessions (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id         INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            date_entry      TEXT NOT NULL,
            workout_name    TEXT,
            duration_min    INTEGER,
            total_kcal      REAL,
            notes           TEXT,
            created_at      TEXT DEFAULT (datetime('now'))
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS workout_exercises (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            workout_id          INTEGER NOT NULL REFERENCES workout_sessions(id) ON DELETE CASCADE,
            exercise_id         INTEGER REFERENCES exercise_library(id),
            exercise_name       TEXT,
            sets                INTEGER,
            reps                INTEGER,
            weight_kg           REAL,
            duration_sec        INTEGER,
            distance_km         REAL,
            kcal                REAL,
            order_in_workout    INTEGER
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS daily_activity (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id             INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            date_entry          TEXT NOT NULL UNIQUE,
            steps               INTEGER,
            active_calories     REAL,
            total_calories      REAL,
            sleep_hours         REAL,
            sleep_quality       TEXT,
            heart_rate_avg      REAL,
            vo2max              REAL,
            standing_hours      INTEGER,
            synced_from_iphone  INTEGER DEFAULT 0,
            created_at          TEXT DEFAULT (datetime('now'))
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS ai_predictions (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id             INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            date_entry          TEXT NOT NULL,
            prediction_type     TEXT NOT NULL,
            model_version       TEXT,
            input_summary       TEXT,
            content             TEXT NOT NULL,
            confidence          REAL DEFAULT 1.0,
            is_read             INTEGER DEFAULT 0,
            is_applied          INTEGER DEFAULT 0,
            created_at          TEXT DEFAULT (datetime('now'))
        )
    )");

    // Индексы
    query.exec("CREATE INDEX IF NOT EXISTS idx_food_name ON food_library(name)");

    // ====================== ЗАПОЛНЕНИЕ ПРОДУКТОВ ======================
    QSqlQuery checkQuery("SELECT COUNT(*) FROM food_library");
    checkQuery.next();

    if (checkQuery.value(0).toInt() == 0) {
        qDebug() << "Добавляем все продукты по одному...";

        struct FoodItem {
            QString name;
            double kcal, protein, fat, carbs;
        };

        QList<FoodItem> allFoods = {
            {"Куриная грудка сырая", 110, 23.1, 1.2, 0.0},
            {"Куриное бедро сырое (с кожей)", 211, 17.0, 15.5, 0.0},
            {"Куриное филе сырое", 113, 23.6, 1.9, 0.0},
            {"Говядина вырезка сырая", 158, 22.0, 7.1, 0.0},
            {"Говяжий фарш 10% жирности", 176, 20.0, 10.0, 0.0},
            {"Свинина вырезка сырая", 142, 21.0, 6.0, 0.0},
            {"Индейка филе сырое", 135, 24.0, 3.0, 0.0},
            {"Баранина сырая", 203, 18.0, 14.0, 0.0},
            {"Лосось сырой (сёмга)", 208, 20.0, 13.0, 0.0},
            {"Тунец свежий", 144, 23.0, 5.0, 0.0},
            {"Треска сырая", 82, 18.0, 0.7, 0.0},
            {"Минтай сырой", 72, 15.9, 0.9, 0.0},
            {"Креветки сырые", 85, 20.0, 0.8, 0.0},
            {"Кальмар сырой", 92, 16.0, 1.7, 3.0},
            {"Яйцо куриное целое сырое", 143, 12.7, 9.5, 0.7},
            {"Яичный белок сырой", 48, 11.1, 0.2, 0.7},
            {"Творог 5% жирности", 121, 17.2, 5.0, 1.8},
            {"Творог 0% (обезжиренный)", 71, 16.5, 0.0, 1.3},
            {"Творог 9%", 169, 16.0, 9.0, 2.0},
            {"Молоко 2.5%", 52, 2.8, 2.5, 4.7},
            {"Кефир 1%", 40, 2.8, 1.0, 4.0},
            {"Сыр Чеддер", 402, 25.0, 33.0, 1.3},
            {"Сыр Российский", 363, 24.0, 30.0, 0.0},
            {"Моцарелла", 280, 22.0, 21.0, 2.2},
            {"Йогурт греческий 2%", 73, 9.0, 2.0, 4.0},
            {"Сметана 15%", 158, 2.6, 15.0, 3.0},
            {"Гречка сухая", 343, 13.3, 3.4, 62.1},
            {"Рис белый сухой", 360, 7.1, 0.7, 79.0},
            {"Рис бурый сухой", 370, 7.5, 2.7, 77.0},
            {"Овсяные хлопья сухие", 366, 13.0, 6.5, 61.0},
            {"Перловка сухая", 354, 12.3, 1.3, 73.7},
            {"Киноа сухая", 368, 14.1, 6.1, 64.2},
            {"Макароны твердых сортов сухие", 371, 13.0, 1.5, 75.0},
            {"Пшено сухое", 348, 11.5, 3.3, 69.0},
            {"Огурец свежий", 15, 0.8, 0.1, 2.5},
            {"Помидор свежий", 18, 0.9, 0.2, 3.9},
            {"Брокколи свежая", 34, 2.8, 0.4, 6.6},
            {"Цветная капуста свежая", 25, 1.9, 0.3, 5.0},
            {"Капуста белокочанная", 27, 1.8, 0.1, 4.7},
            {"Морковь свежая", 41, 0.9, 0.2, 9.6},
            {"Свёкла свежая", 43, 1.6, 0.1, 9.6},
            {"Кабачок свежий", 17, 1.2, 0.3, 3.1},
            {"Шпинат свежий", 23, 2.9, 0.3, 3.6},
            {"Перец болгарский", 20, 0.9, 0.2, 4.6},
            {"Салат листовой", 15, 1.4, 0.2, 2.0},
            {"Яблоко", 52, 0.3, 0.2, 13.8},
            {"Банан", 89, 1.1, 0.3, 22.8},
            {"Апельсин", 47, 0.9, 0.1, 11.7},
            {"Киви", 61, 1.1, 0.5, 14.7},
            {"Клубника", 32, 0.7, 0.3, 7.7},
            {"Черника", 57, 0.7, 0.3, 14.5},
            {"Малина", 52, 1.2, 0.7, 11.9},
            {"Авокадо", 160, 2.0, 14.7, 8.5},
            {"Груша", 57, 0.4, 0.1, 15.0},
            {"Виноград", 69, 0.7, 0.2, 18.1},
            {"Ананас", 50, 0.5, 0.1, 13.1},
            {"Миндаль", 579, 21.2, 49.9, 21.6},
            {"Грецкий орех", 654, 15.2, 65.2, 13.7},
            {"Арахис", 567, 25.8, 49.2, 16.1},
            {"Кешью", 553, 18.2, 43.9, 30.2},
            {"Семена чиа", 486, 16.5, 30.7, 42.1},
            {"Семена льна", 534, 18.3, 42.2, 28.9},
            {"Курага", 241, 3.4, 0.4, 62.6},
            {"Изюм", 299, 3.1, 0.5, 79.2},
            {"Чернослив", 240, 2.3, 0.7, 63.9},
            {"Хлеб цельнозерновой", 247, 8.6, 1.3, 52.0},
            {"Хлеб белый", 265, 8.9, 2.5, 50.0},
            {"Масло оливковое", 884, 0.0, 99.8, 0.0},
            {"Масло подсолнечное", 884, 0.0, 99.9, 0.0},
            {"Масло сливочное", 717, 0.8, 81.1, 0.1},
            {"Фасоль красная сухая", 333, 23.0, 1.5, 60.0},
            {"Чечевица сухая", 352, 24.6, 1.1, 60.1},
            {"Нут сухой", 364, 19.0, 6.0, 61.0},
            {"Картофель сырой", 77, 2.0, 0.1, 17.0},
            {"Тофу", 76, 8.1, 4.8, 1.9},
            {"Протеин сывороточный (на 100г)", 400, 80.0, 5.0, 8.0},
            {"Батончик протеиновый", 380, 30.0, 12.0, 35.0}
        };

        int count = 0;
        for (const auto& item : allFoods) {
            query.prepare("INSERT OR IGNORE INTO food_library "
                          "(name, calories_per_100g, protein_g, fat_g, carbs_g, source) "
                          "VALUES (:n, :k, :p, :f, :c, 'default')");

            query.bindValue(":n", item.name);
            query.bindValue(":k", item.kcal);
            query.bindValue(":p", item.protein);
            query.bindValue(":f", item.fat);
            query.bindValue(":c", item.carbs);

            if (query.exec()) {
                count++;
            } else {
                qWarning() << "Не добавлен:" << item.name << "—" << query.lastError().text();
            }
        }

        qDebug() << "Успешно добавлено продуктов:" << count << "из" << allFoods.size();
    }

    return true;
}

// Остальные функции оставляем без изменений (createDefaultUser, getCurrentUserId, addFoodToLibrary и т.д.)
// ... (вставь сюда все остальные функции из твоего файла database.cpp)


bool DatabaseManager::createDefaultUser()
{
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = 'default_user'");
    query.exec();

    if (!query.next()) {
        query.prepare("INSERT INTO users (username, email) VALUES ('default_user', 'user@example.com')");
        if (!query.exec()) {
            qWarning() << "Ошибка создания пользователя:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

void DatabaseManager::setCurrentUserId(int id) {
    g_currentUserId = id;
}

int DatabaseManager::getCurrentUserId() {
    return g_currentUserId;
}
int DatabaseManager::getUserId(const QString &nickname) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :nick");
    query.bindValue(":nick", nickname);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1; // Пользователь не найден
}

int DatabaseManager::addUser(const QString &nickname) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username) VALUES (:nick)");
    query.bindValue(":nick", nickname);
    if (query.exec()) {
        return query.lastInsertId().toInt(); // Возвращаем ID нового пользователя
    }
    return -1; // Ошибка создания
}

// =============================================
// Остальные методы
// =============================================

bool DatabaseManager::addFoodToLibrary(const QString &name, double calories, double protein, double fat, double carbs)
{
    if (foodExists(name)) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO food_library (name, calories_per_100g, protein_g, fat_g, carbs_g, is_user_added) "
                  "VALUES (:name, :cal, :prot, :fat, :carb, 1)");
    query.bindValue(":name", name);
    query.bindValue(":cal", calories);
    query.bindValue(":prot", protein);
    query.bindValue(":fat", fat);
    query.bindValue(":carb", carbs);

    return query.exec();
}

bool DatabaseManager::foodExists(const QString &name)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM food_library WHERE name = :name");
    query.bindValue(":name", name);
    query.exec();
    return query.next();
}

QSqlQuery DatabaseManager::getFoodBySearch(const QString &searchText)
{
    QSqlQuery query(database());
    QString cleaned = searchText.trimmed();

    if (cleaned.isEmpty()) {
        query.prepare("SELECT name, calories_per_100g, protein_g, fat_g, carbs_g "
                      "FROM food_library ORDER BY name");
    } else {
        query.prepare("SELECT name, calories_per_100g, protein_g, fat_g, carbs_g "
                      "FROM food_library "
                      "WHERE name LIKE :search "
                      "ORDER BY name");
        query.bindValue(":search", "%" + cleaned + "%");
    }

    if (!query.exec()) {
        qWarning() << "Ошибка поиска продуктов:" << query.lastError().text();
    }

    return query;
}

bool DatabaseManager::addNutritionEntry(const QDate &date, int mealType, const QString &foodName,
                                        double weightG, double kcal, double protein, double fat, double carbs)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO nutrition_log
        (user_id, date_entry, meal_type, food_name, weight_g, kcal, protein_g, fat_g, carbs_g)
        VALUES (:uid, :date, :type, :name, :w, :k, :p, :f, :c)
    )");

    query.bindValue(":uid", getCurrentUserId());
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.bindValue(":type", mealType);
    query.bindValue(":name", foodName);
    query.bindValue(":w", weightG);
    query.bindValue(":k", kcal);
    query.bindValue(":p", protein);
    query.bindValue(":f", fat);
    query.bindValue(":c", carbs);

    return query.exec();
}

QSqlQuery DatabaseManager::getDailyNutrition(const QDate &date)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT meal_type, food_name, weight_g, kcal, protein_g, fat_g, carbs_g, id
        FROM nutrition_log
        WHERE user_id = :uid AND date_entry = :date
        ORDER BY meal_type
    )");
    query.bindValue(":uid", getCurrentUserId());
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.exec();
    return query;
}


bool DatabaseManager::deleteNutritionEntry(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM nutrition_log WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

// 3. Реализуем пропорциональный пересчет БЖУ при изменении веса
bool DatabaseManager::updateNutritionEntryWeight(int id, double newWeightG)
{
    QSqlQuery query;
    query.prepare("SELECT weight_g, kcal, protein_g, fat_g, carbs_g FROM nutrition_log WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec() || !query.next()) return false;

    double oldWeight = query.value(0).toDouble();
    if (oldWeight <= 0) return false;

    // Считаем коэффициент изменения веса
    double ratio = newWeightG / oldWeight;
    double newKcal = query.value(1).toDouble() * ratio;
    double newProt = query.value(2).toDouble() * ratio;
    double newFat = query.value(3).toDouble() * ratio;
    double newCarbs = query.value(4).toDouble() * ratio;

    QSqlQuery updateQuery;
    updateQuery.prepare(R"(
        UPDATE nutrition_log
        SET weight_g = :w, kcal = :k, protein_g = :p, fat_g = :f, carbs_g = :c
        WHERE id = :id
    )");
    updateQuery.bindValue(":w", newWeightG);
    updateQuery.bindValue(":k", newKcal);
    updateQuery.bindValue(":p", newProt);
    updateQuery.bindValue(":f", newFat);
    updateQuery.bindValue(":c", newCarbs);
    updateQuery.bindValue(":id", id);
    return updateQuery.exec();
}

QMap<QString, double> DatabaseManager::getDailyTotals(const QDate &date)
{
    QMap<QString, double> totals;
    QSqlQuery query;
    query.prepare(R"(
        SELECT SUM(kcal), SUM(protein_g), SUM(fat_g), SUM(carbs_g)
        FROM nutrition_log
        WHERE user_id = :uid AND date_entry = :date
    )");
    query.bindValue(":uid", getCurrentUserId());
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.exec();

    if (query.next()) {
        totals["kcal"]    = query.value(0).toDouble();
        totals["protein"] = query.value(1).toDouble();
        totals["fat"]     = query.value(2).toDouble();
        totals["carbs"]   = query.value(3).toDouble();
    }
    return totals;
}

void DatabaseManager::populateDefaultFoods()
{
    struct FoodItem {
        QString name;
        double kcal;
        double protein;
        double fat;
        double carbs;
    };

    // Расширенный список продуктов (БЖУ указано на 100 г)
    QVector<FoodItem> defaultFoods = {
        {"Яйцо куриное (1 шт)", 74, 6.2, 5.0, 0.4},
        {"Молоко 2.5%", 52, 2.8, 2.5, 4.7},
        {"Овсяные хлопья", 352, 12.3, 6.2, 61.8},
        {"Говядина отварная", 254, 25.8, 16.8, 0.0},
        {"Филе индейки", 115, 24.1, 1.7, 0.0},
        {"Картофель отварной", 82, 2.0, 0.4, 16.7},
        {"Томаты", 18, 0.6, 0.2, 4.2},
        {"Огурцы", 15, 0.8, 0.1, 2.8},
        {"Хлеб пшеничный", 242, 8.1, 1.0, 48.8},
        {"Масло оливковое", 898, 0.0, 99.8, 0.0},
        {"Сыр Гауда", 356, 25.0, 27.0, 0.0},
        {"Миндаль", 609, 18.6, 53.7, 13.0},
        {"Йогурт натуральный", 66, 4.3, 3.2, 3.5},
        {"Макароны отварные", 112, 3.7, 0.4, 22.9}
    };

    for (const auto &food : defaultFoods) {
        // Проверяем, нет ли уже такого продукта, чтобы не дублировать
        if (!foodExists(food.name)) {
            addFoodToLibrary(food.name, food.kcal, food.protein, food.fat, food.carbs);
        }
    }
}


bool DatabaseManager::saveBodyMeasurements(int userId, double weight, double height, double neck, double waist, double hips, double bmi, double fat) {
    QSqlQuery query;
    query.prepare("INSERT INTO body_measurements (user_id, date_entry, weight, height, neck, waist, hips, bmi, fat_percent) "
                  "VALUES (:uid, :date, :w, :h, :neck, :waist, :hips, :bmi, :fat)");
    query.bindValue(":uid", userId);
    query.bindValue(":date", QDate::currentDate().toString("yyyy-MM-dd"));
    query.bindValue(":w", weight);
    query.bindValue(":h", height);
    query.bindValue(":neck", neck);
    query.bindValue(":waist", waist);
    query.bindValue(":hips", hips);
    query.bindValue(":bmi", bmi);
    query.bindValue(":fat", fat);
    return query.exec();
}


QString DatabaseManager::getUserName(int userId) {
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toString(); // Возвращаем имя
    }
    return "Неизвестный";
}


    QSqlQuery DatabaseManager::getMeasurementHistory() {
    QSqlQuery query;
    // Добавлена сортировка DESC (по убыванию даты и ID)
    query.prepare("SELECT date_entry, weight, height, neck, waist, hips, bmi, fat_percent "
                  "FROM body_measurements WHERE user_id = :uid "
                  "ORDER BY date_entry DESC, id DESC");
    query.bindValue(":uid", getCurrentUserId());
    query.exec();
    return query;
}

    // ======================= ТРЕНИРОВКИ =======================

    int DatabaseManager::getWorkoutId(const QDate &date) {
        QSqlQuery query;
        query.prepare("SELECT id FROM workout_sessions WHERE user_id = :uid AND date_entry = :date LIMIT 1");
        query.bindValue(":uid", getCurrentUserId());
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        if (query.exec() && query.next()) return query.value(0).toInt();
        return -1;
    }

    int DatabaseManager::getOrCreateWorkout(const QDate &date) {
        int id = getWorkoutId(date);
        if (id != -1) return id; // Если тренировка уже есть, возвращаем её

        QSqlQuery query;
        query.prepare("INSERT INTO workout_sessions (user_id, date_entry, workout_name) VALUES (:uid, :date, 'Силовая')");
        query.bindValue(":uid", getCurrentUserId());
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        if (query.exec()) return query.lastInsertId().toInt();
        return -1;
    }
    void DatabaseManager::populateDefaultExercises() {
        // Используем QMap для привязки списка упражнений к их категории
        QMap<QString, QStringList> exerciseMap;

        exerciseMap["Ноги"] = {
            "Приседания", "Плие-приседания", "Болгарские сплит-приседания", "Выпады вперёд",
            "Зашагивания на возвышенность", "Ягодичный мостик", "Приседания «Пистолетик»",
            "Приседания со штангой (на спине)", "Фронтальные приседания", "Румынская тяга",
            "Выпады с гантелями в ходьбе", "Гакк-приседания", "Сгибание ног лёжа"
        };
        exerciseMap["Грудь"] = {
            "Отжимания от пола", "Отжимания с широкой постановкой рук", "Отжимания с узкой постановкой рук",
            "Отжимания ногами на возвышенности", "Алмазные отжимания", "Жим штанги лёжа",
            "Жим гантелей лёжа", "Жим гантелей на наклонной скамье", "Разводка гантелей лёжа",
            "Сведение рук в кроссовере", "Отжимания на брусьях"
        };
        exerciseMap["Спина"] = {
            "Австралийские подтягивания", "Подтягивания на турнике (разные хваты)", "Гиперэкстензия («Лодочка»)",
            "Планка с отведением руки вперёд", "Тяга штанги в наклоне", "Тяга гантели в наклоне",
            "Тяга гантели одной рукой к поясу", "Тяга верхнего блока к груди", "Тяга эспандера к лицу",
            "Тяга горизонтального блока", "Шраги"
        };
        exerciseMap["Плечи"] = {
            "Отжимания в стойке у стены", "Отжимания углом", "Подъёмы рук в стороны (без веса)",
            "Жим штанги сидя (армейский жим)", "Жим гантелей сидя", "Жим Арнольда",
            "Махи гантелями в стороны", "Махи гантелями перед собой", "Разводки гантелей в наклоне"
        };
        exerciseMap["Бицепс"] = {
            "Подтягивания обратным хватом", "Сгибание рук с рюкзаком / сумкой", "Сгибание рук со штангой стоя",
            "Сгибание рук с гантелями стоя", "«Молотки»", "Сгибание на скамье Скотта"
        };
        exerciseMap["Трицепс"] = {
            "Обратные отжимания от стула", "Отжимания с узкой постановкой рук", "Жим лёжа узким хватом",
            "Французский жим", "Разгибание рук на верхнем блоке", "Разгибание рук с эспандером"
        };
        exerciseMap["Пресс (кор)"] = {
            "Скручивания", "Обратные скручивания", "Подъём ног в висе", "Подъём ног лёжа (угол 90°)",
            "«Велосипед»", "«Ножницы»", "Боковые скручивания", "Наклоны в стороны с гантелью",
            "Планка классическая", "Боковая планка", "Вакуум", "«Скалолаз»"
        };
        exerciseMap["Кардио"] = {
            "Jumping Jacks («звёздочка»)", "Скакалка", "Бег на месте с высоким подниманием бедер",
            "Берпи", "Боксёрские тени"
        };

        QSqlQuery q;
        // INSERT OR IGNORE гарантирует, что если упражнение уже есть в базе, оно не вызовет ошибку
        q.prepare("INSERT OR IGNORE INTO exercise_library (name, category) VALUES (:name, :cat)");

        for (const QString& category : exerciseMap.keys()) {
            for (const QString& ex : exerciseMap[category]) {
                q.bindValue(":name", ex);
                q.bindValue(":cat", category);
                q.exec();
            }
        }
    }

    QSqlQuery DatabaseManager::getExercisesList() {
        // Сортируем сначала по категории, потом по алфавиту, чтобы в выпадающем списке был порядок
        return QSqlQuery("SELECT name FROM exercise_library ORDER BY category ASC, name ASC");
    }

    QSqlQuery DatabaseManager::getTodayWorkoutExercises(int workoutId) {
        QSqlQuery query;
        query.prepare("SELECT exercise_name, sets, reps, weight_kg FROM workout_exercises WHERE workout_id = :wid ORDER BY id ASC");
        query.bindValue(":wid", workoutId);
        query.exec();
        return query;
    }

    QString DatabaseManager::getExerciseStats(const QString &exerciseName) {
        QSqlQuery query;
        query.prepare("SELECT MAX(weight_kg), SUM(sets * reps * weight_kg) FROM workout_exercises "
                      "JOIN workout_sessions ON workout_exercises.workout_id = workout_sessions.id "
                      "WHERE exercise_name = :name AND workout_sessions.user_id = :uid");
        query.bindValue(":name", exerciseName);
        query.bindValue(":uid", getCurrentUserId());
        query.exec();

        if (query.next() && query.value(0).toDouble() > 0) {
            return QString("Максимальный вес: %1 кг\nОбщий тоннаж за всё время: %2 кг")
                .arg(query.value(0).toDouble(), 0, 'f', 1).arg(query.value(1).toDouble(), 0, 'f', 1);
        }
        return "Статистики пока нет.";
    }

    int DatabaseManager::getNextSetNumber(int workoutId, const QString &exerciseName) {
        QSqlQuery q;
        q.prepare("SELECT MAX(sets) FROM workout_exercises WHERE workout_id = :wid AND exercise_name = :name");
        q.bindValue(":wid", workoutId);
        q.bindValue(":name", exerciseName);
        if (q.exec() && q.next()) {
            return q.value(0).toInt() + 1; // Возвращаем следующий номер подхода
        }
        return 1;
    }

    bool DatabaseManager::addSetToWorkout(int workoutId, const QString &exerciseName, int reps, double weightKg) {
        int setNum = getNextSetNumber(workoutId, exerciseName);
        QSqlQuery query;
        // Мы используем колонку sets для хранения номера подхода (1, 2, 3...)
        query.prepare("INSERT INTO workout_exercises (workout_id, exercise_name, sets, reps, weight_kg) VALUES (:wid, :name, :sets, :reps, :w)");
        query.bindValue(":wid", workoutId);
        query.bindValue(":name", exerciseName);
        query.bindValue(":sets", setNum);
        query.bindValue(":reps", reps);
        query.bindValue(":w", weightKg);
        return query.exec();
    }

    int DatabaseManager::createNewWorkout(const QDate &date, const QString &workoutName) {
        QSqlQuery query;
        query.prepare("INSERT INTO workout_sessions (user_id, date_entry, workout_name) VALUES (:uid, :date, :name)");
        query.bindValue(":uid", getCurrentUserId());
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        query.bindValue(":name", workoutName);

        if (query.exec()) return query.lastInsertId().toInt();
        return -1;
    }

    QSqlQuery DatabaseManager::getWorkoutsForDate(const QDate &date) {
        QSqlQuery query;
        // Получаем список всех тренировок за конкретный день
        query.prepare("SELECT id, workout_name FROM workout_sessions WHERE user_id = :uid AND date_entry = :date ORDER BY id ASC");
        query.bindValue(":uid", getCurrentUserId());
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        query.exec();
        return query;
    }
    QSqlQuery DatabaseManager::getDailyActivity(const QDate &date) {
        QSqlQuery query;
        query.prepare("SELECT steps, active_calories, sleep_hours FROM daily_activity WHERE user_id = :uid AND date_entry = :date");
        query.bindValue(":uid", getCurrentUserId());
        query.bindValue(":date", date.toString("yyyy-MM-dd"));
        query.exec();
        return query;
    }

    void DatabaseManager::getWeeklyStats(double &avgKcal, double &avgProt, int &workoutCount, int &avgSteps) {
        avgKcal = 0.0; avgProt = 0.0; workoutCount = 0; avgSteps = 0;
        int uid = getCurrentUserId();

        QDate endDate = QDate::currentDate();
        QDate startDate = endDate.addDays(-7);
        QString startStr = startDate.toString("yyyy-MM-dd");
        QString endStr = endDate.toString("yyyy-MM-dd");

        // 1. Считаем средние калории и белок (используем подзапрос, чтобы сгруппировать по дням)
        QSqlQuery qNut(database());
        qNut.prepare("SELECT AVG(daily_kcal), AVG(daily_prot) FROM "
                     "(SELECT date_entry, SUM(kcal) as daily_kcal, SUM(protein_g) as daily_prot "
                     "FROM nutrition_log WHERE user_id = :uid AND date_entry BETWEEN :start AND :end GROUP BY date_entry)");
        qNut.bindValue(":uid", uid);
        qNut.bindValue(":start", startStr);
        qNut.bindValue(":end", endStr);
        if (qNut.exec() && qNut.next()) {
            avgKcal = qNut.value(0).toDouble();
            avgProt = qNut.value(1).toDouble();
        }

        // 2. Считаем количество тренировок
        QSqlQuery qWork(database());
        qWork.prepare("SELECT COUNT(id) FROM workout_sessions WHERE user_id = :uid AND date_entry BETWEEN :start AND :end");
        qWork.bindValue(":uid", uid);
        qWork.bindValue(":start", startStr);
        qWork.bindValue(":end", endStr);
        if (qWork.exec() && qWork.next()) workoutCount = qWork.value(0).toInt();

        // 3. Считаем средние шаги
        QSqlQuery qSteps(database());
        qSteps.prepare("SELECT AVG(steps) FROM daily_activity WHERE user_id = :uid AND date_entry BETWEEN :start AND :end");
        qSteps.bindValue(":uid", uid);
        qSteps.bindValue(":start", startStr);
        qSteps.bindValue(":end", endStr);
        if (qSteps.exec() && qSteps.next()) avgSteps = qSteps.value(0).toInt();
    }

