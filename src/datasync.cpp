#include "datasync.h"
#include "database.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

bool DataSync::importActivityFromJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл синхронизации:" << filePath;
        return false;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isArray()) {
        qWarning() << "Ожидался массив JSON данных.";
        return false;
    }

    QJsonArray array = doc.array();
    QSqlQuery query(DatabaseManager::database());

    int importedCount = 0;

    // Ожидаемый формат JSON: [{"date": "2026-06-20", "steps": 8500, "sleep_hours": 7.5, "active_kcal": 400}]
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString dateStr = obj["date"].toString();
        int steps = obj["steps"].toInt();
        double sleepHours = obj["sleep_hours"].toDouble();
        double activeKcal = obj["active_kcal"].toDouble();

        if (dateStr.isEmpty()) continue;

        query.prepare(R"(
            INSERT OR REPLACE INTO daily_activity
            (user_id, date_entry, steps, active_calories, sleep_hours, synced_from_iphone)
            VALUES (:uid, :date, :steps, :akcal, :sleep, 1)
        )");
        query.bindValue(":uid", DatabaseManager::getCurrentUserId());
        query.bindValue(":date", dateStr);
        query.bindValue(":steps", steps);
        query.bindValue(":akcal", activeKcal);
        query.bindValue(":sleep", sleepHours);

        if (query.exec()) {
            importedCount++;
        } else {
            qWarning() << "Ошибка импорта активности:" << query.lastError().text();
        }
    }

    qDebug() << "Успешно синхронизировано записей:" << importedCount;
    return importedCount > 0;
}
