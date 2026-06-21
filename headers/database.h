#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDate>
#include <QDir>
#include <QFile>

class DatabaseManager
{
public:
    // Инициализация базы данных (вызывать один раз при запуске приложения)
    static bool initDatabase();

    // Получение подключения к БД (для использования в других классах)
    static QSqlDatabase database();

    // ======================= Пользователь =======================
    static bool createDefaultUser();                    // Создаёт пользователя с id = 1, если его нет
    static int getCurrentUserId();                      // Возвращает ID текущего пользователя (пока 1)

    // ======================= Продукты =======================
    static bool addFoodToLibrary(const QString &name, double calories, double protein, double fat, double carbs);
    static bool foodExists(const QString &name);
    static QSqlQuery getFoodBySearch(const QString &searchText);

    // ======================= Дневник питания =======================
    static bool addNutritionEntry(const QDate &date, int mealType, const QString &foodName,
                                  double weightG, double kcal, double protein, double fat, double carbs);

    static QSqlQuery getDailyNutrition(const QDate &date);   // Для загрузки в дерево

    static QMap<QString, double> getDailyTotals(const QDate &date); // kcal, protein, fat, carbs

    // ======================= Тренировки =======================
    static int createWorkoutSession(const QDate &date, const QString &workoutName = "",
                                    int durationMin = 0, double totalKcal = 0.0, const QString &notes = "");
    static bool addSetToWorkout(int workoutId, const QString &exerciseName, int reps, double weightKg);
    static int getNextSetNumber(int workoutId, const QString &exerciseName);

    // ======================= Дневная активность =======================
    static bool saveDailyActivity(const QDate &date, int steps = 0, double activeCalories = 0.0,
                                  double sleepHours = 0.0, const QString &sleepQuality = "");

    // ======================= AI Советы =======================
    static bool saveAIPrediction(const QDate &date, const QString &predictionType,
                                 const QString &content, const QString &modelVersion = "unknown",
                                 double confidence = 1.0, const QString &inputSummary = "");

    static QSqlQuery getAIPredictionsForDate(const QDate &date);
    static void populateDefaultFoods();
    static bool deleteNutritionEntry(int id);
    static bool updateNutritionEntryWeight(int id, double newWeightG);
    static bool createUsersTable();
    static int addUser(const QString &nickname); // Возвращает ID нового пользователя
    static int getUserId(const QString &nickname); // Поиск пользователя
    static bool saveMeasurement(int userId, const QDate &date, double weight, double chest, double waist, double hips, double neck);
    static bool saveBodyMeasurements(int userId, double weight, double height, double neck, double waist, double hips, double bmi, double fat);
    static void setCurrentUserId(int id);
    static QString getUserName(int userId);
    static QSqlQuery getMeasurementHistory();
    static int getWorkoutId(const QDate &date);
    static int getOrCreateWorkout(const QDate &date);
    static QSqlQuery getTodayWorkoutExercises(int workoutId);
    static int createNewWorkout(const QDate &date, const QString &workoutName);
    static QSqlQuery getWorkoutsForDate(const QDate &date);
    static QSqlQuery getDailyActivity(const QDate &date);
    static void populateDefaultExercises();
    static QSqlQuery getExercisesList();
    static QString getExerciseStats(const QString &exerciseName);
    static void getWeeklyStats(double &avgKcal, double &avgProt, int &workoutCount, int &avgSteps);




private:
    static bool createTables();
    static QString getDatabasePath();
    static bool ensureDatabaseDirectory();
};

#endif // DATABASE_H
