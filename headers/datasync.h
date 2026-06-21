#ifndef DATASYNC_H
#define DATASYNC_H

#include <QString>

class DataSync {
public:
    // Парсит JSON файл со смартфона и сохраняет в таблицу daily_activity
    static bool importActivityFromJson(const QString &filePath);
};

#endif // DATASYNC_H
