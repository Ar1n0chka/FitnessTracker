#include "statisticsdialog.h"
#include "database.h"
#include <QVBoxLayout>
#include <QDate>
#include <QSqlQuery>
#include <QBarSet>
#include <QBarSeries>
#include <QChart>
#include <QBarCategoryAxis>
#include <QValueAxis>

StatisticsDialog::StatisticsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Статистика питания (7 дней)");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    setupChart();
}

void StatisticsDialog::setupChart() {
    QBarSet *setKcal = new QBarSet("Калории (потреблено)");

    QStringList categories;
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-6); // За последние 7 дней

    // Собираем данные из БД
    QSqlQuery query(DatabaseManager::database());
    query.prepare(R"(
        SELECT date_entry, SUM(kcal)
        FROM nutrition_log
        WHERE user_id = :uid AND date_entry BETWEEN :start AND :end
        GROUP BY date_entry
        ORDER BY date_entry ASC
    )");
    query.bindValue(":uid", DatabaseManager::getCurrentUserId());
    query.bindValue(":start", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", endDate.toString("yyyy-MM-dd"));
    query.exec();

    // Заполняем мапу, чтобы учесть дни, когда записей не было
    QMap<QString, double> kcalMap;
    while (query.next()) {
        kcalMap[query.value(0).toString()] = query.value(1).toDouble();
    }

    double maxKcal = 0;
    for (int i = 0; i <= 6; ++i) {
        QDate d = startDate.addDays(i);
        QString dStr = d.toString("yyyy-MM-dd");
        categories << d.toString("dd.MM");

        double k = kcalMap.value(dStr, 0.0);
        *setKcal << k;
        if (k > maxKcal) maxKcal = k;
    }

    QBarSeries *series = new QBarSeries();
    series->append(setKcal);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Потребление калорий за последнюю неделю");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxKcal + 500); // Даем запас сверху
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setChart(chart);
}
