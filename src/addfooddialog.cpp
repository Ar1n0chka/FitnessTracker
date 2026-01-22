#include "addfooddialog.h"
#include "ui_addfooddialog.h"
// #include "database.h"
// #include <QSqlQuery>

AddFoodDialog::AddFoodDialog(QDate date, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::AddFoodDialog),
    targetDate(date)
{
    ui->setupUi(this);

    ui->mealCombo->addItems({"Завтрак", "Обед", "Ужин", "Перекус"});

    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &AddFoodDialog::onSearchTextChanged);
    connect(ui->btnAdd, &QPushButton::clicked,
            this, &AddFoodDialog::onAddClicked);
    connect(ui->resultsTable, &QTableWidget::cellClicked,
            this, &AddFoodDialog::onTableClicked);

    onSearchTextChanged("");
}

void AddFoodDialog::onSearchTextChanged(const QString &text)
{
    ui->resultsTable->setRowCount(0);


    // QSqlQuery query;
    // query.prepare("SELECT name, calories, protein, fat, carbs "
    //               "FROM food_library WHERE name LIKE :search");
    // query.bindValue(":search", "%" + text + "%");
    // query.exec();
    //
    // while (query.next()) {
    //     int row = ui->resultsTable->rowCount();
    //     ui->resultsTable->insertRow(row);
    //     for (int i = 0; i < 5; ++i)
    //         ui->resultsTable->setItem(
    //             row, i,
    //             new QTableWidgetItem(query.value(i).toString())
    //         );
    // }

    Q_UNUSED(text); // чтобы не было warning
}

void AddFoodDialog::onTableClicked(int row, int col)
{
    Q_UNUSED(col); // убираем warning

    // Данные берём из таблицы (если будут заполнены вручную)
    if (!ui->resultsTable->item(row, 0))
        return;

    selKcal  = ui->resultsTable->item(row, 1)->text().toDouble();
    selProt  = ui->resultsTable->item(row, 2)->text().toDouble();
    selFat   = ui->resultsTable->item(row, 3)->text().toDouble();
    selCarbs = ui->resultsTable->item(row, 4)->text().toDouble();

    ui->searchEdit->setText(ui->resultsTable->item(row, 0)->text());
}

void AddFoodDialog::onAddClicked()
{
    double weight = ui->weightSpin->value();
    double ratio = weight / 100.0;

    // ❌ Добавление в БД отключено
    // DatabaseManager::addEntry(
    //     targetDate,
    //     ui->mealCombo->currentIndex() + 1,
    //     ui->searchEdit->text(),
    //     weight,
    //     selKcal * ratio,
    //     selProt * ratio,
    //     selFat * ratio,
    //     selCarbs * ratio
    // );

    accept(); // просто закрываем диалог
}

AddFoodDialog::~AddFoodDialog()
{
    delete ui;
}
