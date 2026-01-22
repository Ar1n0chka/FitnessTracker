#include "mainwindow.h"
#include "ui_mainwindow.h"
// #include "database.h"        // ❌ БД отключена
#include "addfooddialog.h"

// #include <QSqlQuery>        // ❌ БД отключена

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged,
            this, &MainWindow::onDateChanged);
    connect(ui->btnAddFood, &QPushButton::clicked,
            this, &MainWindow::onAddFoodClicked);

    onDateChanged(); // Загрузка данных при старте
}

void MainWindow::onDateChanged()
{
    loadDailyData();
}

void MainWindow::loadDailyData()
{
    ui->foodTree->clear(); // Очищаем дерево

    // QDate date = ui->calendarWidget->selectedDate(); // ❌ БД отключена

    // QSqlQuery query;                                // ❌ БД отключена
    // query.prepare("SELECT meal_type, food_name, weight, kcal, protein, fat, carbs "
    //               "FROM nutrition_log WHERE date_entry = :date ORDER BY meal_type");
    // query.bindValue(":date", date.toString("yyyy-MM-dd"));
    // query.exec();

    double totalKcal = 0;
    double totalP = 0;
    double totalF = 0;
    double totalC = 0;

    // ❌ Загрузка данных из БД отключена
    // while (query.next()) {
    //     int type = query.value(0).toInt();
    //     QString name = query.value(1).toString();
    //     double w = query.value(2).toDouble();
    //     double k = query.value(3).toDouble();
    //     double p = query.value(4).toDouble();
    //     double f = query.value(5).toDouble();
    //     double c = query.value(6).toDouble();
    //
    //     totalKcal += k;
    //     totalP += p;
    //     totalF += f;
    //     totalC += c;
    //
    //     QTreeWidgetItem *parentItem = getOrCreateMealCategory(type);
    //
    //     QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
    //     item->setText(0, name);
    //     item->setText(1, QString::number(w));
    //     item->setText(2, QString::number(k));
    //     item->setText(3, QString::number(p));
    //     item->setText(4, QString::number(f));
    //     item->setText(5, QString::number(c));
    // }

    ui->foodTree->expandAll(); // Раскрыть все ветки

    // Временно показываем нули
    ui->lblTotal->setText(
        QString("Итого за день: %1 ккал | Б: %2 | Ж: %3 | У: %4")
            .arg(totalKcal)
            .arg(totalP)
            .arg(totalF)
            .arg(totalC)
        );
}

QTreeWidgetItem* MainWindow::getOrCreateMealCategory(int mealType)
{
    QString name;
    switch (mealType) {
    case 1: name = "Завтрак"; break;
    case 2: name = "Обед";    break;
    case 3: name = "Ужин";   break;
    default: name = "Перекус"; break;
    }

    for (int i = 0; i < ui->foodTree->topLevelItemCount(); ++i) {
        if (ui->foodTree->topLevelItem(i)->text(0) == name)
            return ui->foodTree->topLevelItem(i);
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->foodTree);
    item->setText(0, name);
    return item;
}

void MainWindow::onAddFoodClicked()
{
    AddFoodDialog dialog(ui->calendarWidget->selectedDate(), this);
    if (dialog.exec() == QDialog::Accepted) {
        loadDailyData(); // пока просто перерисовка
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
