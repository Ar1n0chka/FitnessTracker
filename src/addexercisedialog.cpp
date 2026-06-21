#include "addexercisedialog.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

AddExerciseDialog::AddExerciseDialog(int workoutId, QWidget *parent)
    : QDialog(parent), currentWorkoutId(workoutId), timeLeft(0) {
    setWindowTitle("Идет тренировка...");
    resize(550, 600); // Сделали окно больше

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- 1. Выбор упражнения ---
    QHBoxLayout *exLayout = new QHBoxLayout();
    exLayout->addWidget(new QLabel("Упражнение:"));
    cbExercises = new QComboBox();
    QSqlQuery exQuery = DatabaseManager::getExercisesList();
    while(exQuery.next()) cbExercises->addItem(exQuery.value(0).toString());
    exLayout->addWidget(cbExercises, 1);
    mainLayout->addLayout(exLayout);

    // --- 2. Таблица подходов ---
    setsTable = new QTableWidget(0, 3, this);
    setsTable->setHorizontalHeaderLabels({"Подход", "Повторения", "Вес (кг)"});
    setsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(setsTable);

    // --- 3. Добавление подхода ---
    QHBoxLayout *addLayout = new QHBoxLayout();
    sbReps = new QSpinBox(); sbReps->setRange(1, 100); sbReps->setValue(10);
    sbWeight = new QDoubleSpinBox(); sbWeight->setRange(0, 500); sbWeight->setSuffix(" кг");

    addLayout->addWidget(new QLabel("Повторы:"));
    addLayout->addWidget(sbReps);
    addLayout->addWidget(new QLabel("Вес:"));
    addLayout->addWidget(sbWeight);

    QPushButton *btnAdd = new QPushButton("Записать подход");
    btnAdd->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 5px;");
    addLayout->addWidget(btnAdd);
    mainLayout->addLayout(addLayout);

    // --- 4. Таймер отдыха ---
    QFormLayout *timerLayout = new QFormLayout();
    lblTimer = new QLabel("00:00");
    lblTimer->setStyleSheet("font-size: 40px; font-weight: bold; color: #333;");
    lblTimer->setAlignment(Qt::AlignCenter);

    sbRestTime = new QSpinBox();
    sbRestTime->setRange(10, 300);
    sbRestTime->setValue(60);
    sbRestTime->setSuffix(" сек");

    QPushButton *btnTimer = new QPushButton("Начать отдых");

    QHBoxLayout *tCtrl = new QHBoxLayout();
    tCtrl->addWidget(sbRestTime);
    tCtrl->addWidget(btnTimer);

    timerLayout->addRow("Время отдыха:", tCtrl);
    mainLayout->addLayout(timerLayout);
    mainLayout->addWidget(lblTimer);

    // --- 5. Кнопка закрытия ---
    QPushButton *btnClose = new QPushButton("Свернуть тренировку / Назад");
    mainLayout->addWidget(btnClose);

    // --- Подключения ---
    connect(cbExercises, &QComboBox::currentTextChanged, this, &AddExerciseDialog::onExerciseChanged);
    connect(btnAdd, &QPushButton::clicked, this, &AddExerciseDialog::onAddSetClicked);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    restTimer = new QTimer(this);
    connect(btnTimer, &QPushButton::clicked, this, &AddExerciseDialog::startTimer);
    connect(restTimer, &QTimer::timeout, this, &AddExerciseDialog::updateTimer);

    // Загружаем подходы при открытии
    loadSetsForCurrentExercise();
}

void AddExerciseDialog::onExerciseChanged() {
    loadSetsForCurrentExercise();
}

void AddExerciseDialog::loadSetsForCurrentExercise() {
    setsTable->setRowCount(0);
    QSqlQuery q;
    q.prepare("SELECT sets, reps, weight_kg FROM workout_exercises WHERE workout_id = :wid AND exercise_name = :name ORDER BY sets ASC");
    q.bindValue(":wid", currentWorkoutId);
    q.bindValue(":name", cbExercises->currentText());
    q.exec();

    int row = 0;
    while (q.next()) {
        setsTable->insertRow(row);
        setsTable->setItem(row, 0, new QTableWidgetItem(QString("Подход %1").arg(q.value(0).toInt())));
        setsTable->setItem(row, 1, new QTableWidgetItem(QString::number(q.value(1).toInt())));
        setsTable->setItem(row, 2, new QTableWidgetItem(QString::number(q.value(2).toDouble())));
        row++;
    }
}

void AddExerciseDialog::onAddSetClicked() {
    if (cbExercises->currentText().isEmpty()) return;

    if (DatabaseManager::addSetToWorkout(currentWorkoutId, cbExercises->currentText(), sbReps->value(), sbWeight->value())) {
        loadSetsForCurrentExercise(); // Моментально обновляем табличку
        startTimer(); // Автоматически запускаем таймер отдыха
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить подход.");
    }
}

void AddExerciseDialog::startTimer() {
    timeLeft = sbRestTime->value();
    lblTimer->setStyleSheet("color: red; font-size: 40px; font-weight: bold;");
    updateTimer();
    restTimer->start(1000);
}

void AddExerciseDialog::updateTimer() {
    if (timeLeft <= 0) {
        restTimer->stop();
        lblTimer->setText("ГОТОВ К ПОДХОДУ!");
        lblTimer->setStyleSheet("color: green; font-size: 32px; font-weight: bold;");
    } else {
        int m = timeLeft / 60;
        int s = timeLeft % 60;
        lblTimer->setText(QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
        timeLeft--;
    }
}
