#include "profiledialog.h"
#include <cmath>
#include <QFormLayout>
#include <QMessageBox>
#include <QHeaderView>
#include "database.h"

ProfileDialog::ProfileDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Личные данные и история");
    resize(500, 600); // Окно побольше, чтобы влезла таблица

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    cbGender = new QComboBox();
    cbGender->addItems({"Мужчина", "Женщина"});
    leWeight = new QLineEdit();
    leHeight = new QLineEdit();
    leNeck = new QLineEdit();
    leWaist = new QLineEdit();
    leHips = new QLineEdit();

    formLayout->addRow("Пол:", cbGender);
    formLayout->addRow("Вес (кг):", leWeight);
    formLayout->addRow("Рост (см):", leHeight);
    formLayout->addRow("Обхват шеи (см):", leNeck);
    formLayout->addRow("Обхват талии (см):", leWaist);
    formLayout->addRow("Обхват бедер (см):", leHips);

    QPushButton *btnCalc = new QPushButton("Рассчитать и сохранить");
    connect(btnCalc, &QPushButton::clicked, this, &ProfileDialog::calculateMetrics);

    lblResult = new QLabel("Введите данные для расчета");

    // --- Настраиваем таблицу истории ---
    historyTable = new QTableWidget();
    historyTable->setColumnCount(8);
    historyTable->setHorizontalHeaderLabels({"Дата", "Вес", "Рост", "Шея", "Талия", "Бедра", "ИМТ", "% Жира"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрещаем ручное редактирование ячеек

    // Собираем всё в главное окно
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(btnCalc);
    mainLayout->addWidget(lblResult);
    mainLayout->addWidget(new QLabel("<b>История ваших замеров:</b>"));
    mainLayout->addWidget(historyTable);

    loadHistory(); // Загружаем историю при открытии окна
}

void ProfileDialog::loadHistory() {
    historyTable->setRowCount(0); // Очищаем таблицу
    QSqlQuery query = DatabaseManager::getMeasurementHistory();

    int row = 0;
    while (query.next()) {
        historyTable->insertRow(row);
        for (int i = 0; i < 8; ++i) {
            QTableWidgetItem *item = nullptr;

            // Округляем ИМТ и Жир до 1 знака
            if (i >= 6) {
                double val = query.value(i).toDouble();
                item = new QTableWidgetItem(QString::number(val, 'f', 1));
            } else {
                item = new QTableWidgetItem(query.value(i).toString());
            }

            // --- ЛОГИКА РАСКРАСКИ ИМТ (Колонка 6) ---
            if (i == 6) {
                double bmi = query.value(i).toDouble();
                if (bmi > 0) { // Исключаем пустые значения
                    if (bmi <= 16.0 || bmi >= 30.0) {
                        item->setBackground(QColor(255, 120, 120)); // Красный
                    } else if ((bmi > 16.0 && bmi <= 18.5) || (bmi >= 25.0 && bmi < 30.0)) {
                        item->setBackground(QColor(255, 190, 80));  // Оранжевый
                    }
                    // В остальных случаях (18.51 - 24.99) цвет остается стандартным (прозрачным)
                }
            }

            historyTable->setItem(row, i, item);
        }
        row++;
    }
}

void ProfileDialog::calculateMetrics() {
    double w = leWeight->text().toDouble();
    double h = leHeight->text().toDouble();
    double neck = leNeck->text().toDouble();
    double waist = leWaist->text().toDouble();
    double hips = leHips->text().toDouble();
    bool isMale = (cbGender->currentIndex() == 0);

    if (w <= 0 || h <= 0) {
        lblResult->setText("Ошибка: введите корректный вес и рост");
        return;
    }

    double hM = h / 100.0;
    double bmi = w / (hM * hM);

    double fat = 0;
    if (isMale) {
        if (waist > neck)
            fat = (495.0 / (1.0324 - 0.19077 * log10(waist - neck) + 0.15456 * log10(h))) - 450.0;
    } else {
        if ((waist + hips - neck) > 0)
            fat = (495.0 / (1.29579 - 0.35004 * log10(waist + hips - neck) + 0.22100 * log10(h))) - 450.0;
    }

    lblResult->setText(QString("ИМТ: %1\nПроцент жира: %2%")
                           .arg(bmi, 0, 'f', 1)
                           .arg(fat, 0, 'f', 1));

    // Сохраняем замеры
    bool ok = DatabaseManager::saveBodyMeasurements(
        DatabaseManager::getCurrentUserId(), w, h, neck, waist, hips, bmi, fat
        );

    if (ok) {
        QMessageBox::information(this, "Успех", "Данные сохранены в историю!");
        loadHistory(); // Сразу обновляем табличку, чтобы увидеть новую запись
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные профиля.");
    }
}
