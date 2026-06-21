#include "addfooddialog.h"
#include "ui_addfooddialog.h"
#include "database.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

AddFoodDialog::AddFoodDialog(QDate date, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddFoodDialog)
    , targetDate(date)
    , selectedKcal(0.0)
    , selectedProt(0.0)
    , selectedFat(0.0)
    , selectedCarbs(0.0)
{
    ui->setupUi(this);

    // === Настройка ширины и расположения ===
    ui->label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    ui->label->setMinimumWidth(65);
    ui->label->setMaximumWidth(75);

    ui->weightSpin->setMinimumWidth(110);
    ui->weightSpin->setMaximumWidth(130);

    ui->mealCombo->setStyleSheet(
        "QComboBox { background-color: #e1e1e1; color: black; border: 1px solid #adadad; border-radius: 4px; padding: 4px; }"
        "QComboBox::drop-down { border-left: 1px solid #adadad; }"
        );
    // Делаем lblNutritionInfo широким
    ui->lblNutritionInfo->setMinimumHeight(58);
    ui->lblNutritionInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Увеличиваем общее окно
    resize(780, 520);
    setMinimumSize(720, 480);

    // Настройка таблицы
    ui->resultsTable->setColumnCount(5);
    ui->resultsTable->setHorizontalHeaderLabels({"Продукт", "Ккал/100г", "Белки", "Жиры", "Углеводы"});
    ui->resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Приём пищи
    ui->mealCombo->addItems({"Завтрак", "Обед", "Ужин", "Перекус"});

    // Подключаем сигналы
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &AddFoodDialog::onSearchTextChanged);
    connect(ui->resultsTable, &QTableWidget::itemSelectionChanged, this, &AddFoodDialog::onProductSelected);
    connect(ui->weightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddFoodDialog::updateNutritionInfo);
    connect(ui->btnAdd, &QPushButton::clicked, this, &AddFoodDialog::onAddClicked);
    connect(ui->btnCreateFood, &QPushButton::clicked, this, &AddFoodDialog::onCreateFoodClicked);

    // Начальный поиск
    onSearchTextChanged("");

    // Инициализируем QLabel пустым текстом
    updateNutritionInfo(ui->weightSpin->value());
}

AddFoodDialog::~AddFoodDialog()
{
    delete ui;
}

void AddFoodDialog::onSearchTextChanged(const QString &text)
{
    ui->resultsTable->setRowCount(0);

    QSqlQuery query = DatabaseManager::getFoodBySearch(text);

    int row = 0;
    while (query.next()) {
        ui->resultsTable->insertRow(row);

        ui->resultsTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // Продукт
        ui->resultsTable->setItem(row, 1, new QTableWidgetItem(QString::number(query.value(1).toDouble(), 'f', 0))); // Ккал
        ui->resultsTable->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 1))); // Б
        ui->resultsTable->setItem(row, 3, new QTableWidgetItem(QString::number(query.value(3).toDouble(), 'f', 1))); // Ж
        ui->resultsTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value(4).toDouble(), 'f', 1))); // У

        row++;
    }

    // Если ничего не найдено — показываем сообщение
    if (row == 0) {
        ui->resultsTable->insertRow(0);
        QTableWidgetItem *item = new QTableWidgetItem("Продукты не найдены");
        item->setForeground(Qt::gray);
        ui->resultsTable->setItem(0, 0, item);
        ui->resultsTable->setSpan(0, 0, 1, 5);
    }
}

void AddFoodDialog::onProductSelected()
{
    int row = ui->resultsTable->currentRow();
    if (row < 0) return;

    selectedKcal  = ui->resultsTable->item(row, 1)->text().toDouble();
    selectedProt  = ui->resultsTable->item(row, 2)->text().toDouble();
    selectedFat   = ui->resultsTable->item(row, 3)->text().toDouble();
    selectedCarbs = ui->resultsTable->item(row, 4)->text().toDouble();

    ui->searchEdit->setText(ui->resultsTable->item(row, 0)->text());

    updateNutritionInfo(ui->weightSpin->value());
}

void AddFoodDialog::updateNutritionInfo(double weight)
{
    if (selectedKcal <= 0) {
        // Если ничего не выбрано
        ui->lblNutritionInfo->setText("Выберите продукт из списка");
        return;
    }

    double ratio = weight / 100.0;

    double finalKcal  = selectedKcal * ratio;
    double finalProt  = selectedProt * ratio;
    double finalFat   = selectedFat * ratio;
    double finalCarbs = selectedCarbs * ratio;

    QString info = QString("На %1 г:  <b>%2 ккал</b>  |  Б: %3 г  |  Ж: %4 г  |  У: %5 г")
                       .arg(weight, 0, 'f', 0)
                       .arg(finalKcal, 0, 'f', 0)
                       .arg(finalProt, 0, 'f', 1)
                       .arg(finalFat, 0, 'f', 1)
                       .arg(finalCarbs, 0, 'f', 1);

    ui->lblNutritionInfo->setText(info);
}

void AddFoodDialog::onAddClicked()
{
    QString foodName = ui->searchEdit->text().trimmed();
    double weight = ui->weightSpin->value();

    if (foodName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите продукт из списка.");
        return;
    }
    if (weight <= 0) {
        QMessageBox::warning(this, "Ошибка", "Вес должен быть больше 0 г.");
        return;
    }
    if (selectedKcal <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите продукт из таблицы.");
        return;
    }

    int mealType = ui->mealCombo->currentIndex() + 1;

    bool success = DatabaseManager::addNutritionEntry(
        targetDate,
        mealType,
        foodName,
        weight,
        selectedKcal * (weight / 100.0),
        selectedProt  * (weight / 100.0),
        selectedFat   * (weight / 100.0),
        selectedCarbs * (weight / 100.0)
        );

    if (success) {
        QMessageBox::information(this, "Успешно",
                                 QString("✅ %1 (%2 г) добавлен в дневник питания.").arg(foodName).arg(weight));
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить запись.");
    }
}


void AddFoodDialog::onCreateFoodClicked()
{
    // Создаем окно "на лету"
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить новый продукт в базу");
    dialog.setMinimumWidth(300);

    QFormLayout form(&dialog);

    QLineEdit *leName = new QLineEdit(&dialog);
    QLineEdit *leKcal = new QLineEdit(&dialog);
    QLineEdit *leProt = new QLineEdit(&dialog);
    QLineEdit *leFat = new QLineEdit(&dialog);
    QLineEdit *leCarbs = new QLineEdit(&dialog);

    // Добавляем подсказки
    leName->setPlaceholderText("Например: Домашний борщ");
    leKcal->setPlaceholderText("Ккал на 100 г");

    form.addRow("Название продукта:", leName);
    form.addRow("Калорийность (ккал):", leKcal);
    form.addRow("Белки (г):", leProt);
    form.addRow("Жиры (г):", leFat);
    form.addRow("Углеводы (г):", leCarbs);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Если пользователь нажал "ОК"
    if (dialog.exec() == QDialog::Accepted) {
        QString name = leName->text().trimmed();
        // replace(",", ".") позволяет пользователям вводить дробные числа через запятую
        double kcal = leKcal->text().replace(",", ".").toDouble();
        double prot = leProt->text().replace(",", ".").toDouble();
        double fat = leFat->text().replace(",", ".").toDouble();
        double carbs = leCarbs->text().replace(",", ".").toDouble();

        if (name.isEmpty() || kcal <= 0) {
            QMessageBox::warning(this, "Ошибка", "Название не может быть пустым, а калорийность должна быть больше 0.");
            return;
        }

        // Сохраняем в базу данных (метод addFoodToLibrary уже написан у вас в DatabaseManager!)
        bool success = DatabaseManager::addFoodToLibrary(name, kcal, prot, fat, carbs);

        if (success) {
            QMessageBox::information(this, "Успех", "Продукт добавлен и теперь доступен для поиска!");
            ui->searchEdit->setText(name); // Сразу вставляем в поиск
            onSearchTextChanged(name);     // Обновляем таблицу
        } else {
            QMessageBox::warning(this, "Ошибка", "Такой продукт уже существует в базе или произошла ошибка.");
        }
    }
}
