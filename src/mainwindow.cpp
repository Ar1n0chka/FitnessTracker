#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "addfooddialog.h"
#include "datasync.h"
#include "statisticsdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include "profiledialog.h"
#include "logindialog.h"
#include "addexercisedialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnGenerateAI, &QPushButton::clicked, this, &MainWindow::onGenerateAIAdviceClicked);

    // Подключаем сигналы
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged,
            this, &MainWindow::onDateChanged);

    connect(ui->btnAddFood, &QPushButton::clicked,
            this, &MainWindow::onAddFoodClicked);

    connect(ui->btnSync, &QPushButton::clicked, this, &MainWindow::onSyncClicked);
    connect(ui->btnStats, &QPushButton::clicked, this, &MainWindow::onStatsClicked);
    connect(ui->btnExportML, &QPushButton::clicked, this, &MainWindow::onExportMLClicked);

    ui->foodTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->foodTree, &QTreeWidget::customContextMenuRequested,
            this, &MainWindow::onFoodTreeContextMenu);

    connect(ui->btnProfile, &QPushButton::clicked, this, &MainWindow::onProfileClicked);
    connect(ui->btnChangeUser, &QPushButton::clicked, this, &MainWindow::onChangeUserClicked);

    // Настраиваем колонки для дерева тренировок
    ui->workoutTree->setHeaderLabels({"Упражнение", "Подходы", "Повторения", "Вес (кг)"});
    ui->workoutTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Строгий запрет на редактирование ячеек пользователем (на всякий случай)
    ui->workoutTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Подключаем вашу кнопку из XML
    connect(ui->btnAddWorkout, &QPushButton::clicked, this, &MainWindow::onAddWorkoutClicked);

    // Начальная загрузка данных на сегодня
    onDateChanged();
    QString currentName = DatabaseManager::getUserName(DatabaseManager::getCurrentUserId());
    this->setWindowTitle("Фитнес Трекер — Аккаунт: " + currentName);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDateChanged()
{
    loadDailyData();
    loadWorkoutData();
}

void MainWindow::loadDailyData()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    ui->foodTree->clear();

    QSqlQuery query = DatabaseManager::getDailyNutrition(selectedDate);

    double totalKcal = 0.0, totalProtein = 0.0, totalFat = 0.0, totalCarbs = 0.0;

    while (query.next()) {
        int mealType = query.value(0).toInt();
        QString foodName = query.value(1).toString();
        double weight = query.value(2).toDouble();
        double kcal = query.value(3).toDouble();
        double protein = query.value(4).toDouble();
        double fat = query.value(5).toDouble();
        double carbs = query.value(6).toDouble();
        int entryId = query.value(7).toInt(); // Получаем ID

        totalKcal += kcal;
        totalProtein += protein;
        totalFat += fat;
        totalCarbs += carbs;

        // Создаем категорию (Завтрак, Обед и т.д.)
        QTreeWidgetItem *categoryItem = getOrCreateMealCategory(mealType);

        // Создаем элемент продукта
        QTreeWidgetItem *item = new QTreeWidgetItem(categoryItem);

        // РАСПРЕДЕЛЯЕМ ДАННЫЕ ПО СТОЛБЦАМ:
        item->setText(0, foodName);
        item->setText(1, QString::number(weight, 'f', 0) + " г");
        item->setText(2, QString::number(kcal, 'f', 0));
        item->setText(3, QString::number(protein, 'f', 1));
        item->setText(4, QString::number(fat, 'f', 1));
        item->setText(5, QString::number(carbs, 'f', 1));

        // Обязательно сохраняем ID для удаления и редактирования
        item->setData(0, Qt::UserRole, entryId);
    }
    ui->foodTree->expandAll();

    // Обновляем итоговую строку
    ui->lblTotal->setText(QString("Итого за день: %1 ккал | Б: %2 | Ж: %3 | У: %4")
                              .arg(totalKcal, 0, 'f', 0)
                              .arg(totalProtein, 0, 'f', 1)
                              .arg(totalFat, 0, 'f', 1)
                              .arg(totalCarbs, 0, 'f', 1));
}

QTreeWidgetItem* MainWindow::getOrCreateMealCategory(int mealType)
{
    QString mealName;
    switch (mealType) {
    case 1: mealName = "Завтрак"; break;
    case 2: mealName = "Обед"; break;
    case 3: mealName = "Ужин"; break;
    case 4: mealName = "Перекус"; break;
    default: mealName = "Другое"; break;
    }

    // Проверяем, существует ли уже такая категория
    for (int i = 0; i < ui->foodTree->topLevelItemCount(); ++i) {
        if (ui->foodTree->topLevelItem(i)->text(0) == mealName) {
            return ui->foodTree->topLevelItem(i);
        }
    }

    // Создаём новую категорию
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->foodTree);
    item->setText(0, mealName);

    // Красим ВСЕ колонки этой строки в светло-серый цвет (как у кнопок) и делаем текст жирным
    for (int col = 0; col < ui->foodTree->columnCount(); ++col) {
        item->setBackground(col, QColor(100, 100, 100));
        QFont f = item->font(col);
        f.setBold(true);
        item->setFont(col, f);
    }

    return item;
}

void MainWindow::onAddFoodClicked()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();

    AddFoodDialog dialog(selectedDate, this);

    if (dialog.exec() == QDialog::Accepted) {
        loadDailyData();        // Обновляем данные после добавления
    }
}




void MainWindow::onSyncClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Выбрать JSON со смартфона", "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        if (DataSync::importActivityFromJson(fileName)) {
            QMessageBox::information(this, "Синхронизация", "Данные успешно загружены!");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные.");
        }
    }
}

void MainWindow::onStatsClicked() {
    StatisticsDialog statsDialog(this);
    statsDialog.exec();
}

void MainWindow::onExportMLClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить датасет", "dataset_ml.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    // Заголовки: X1, X2, X3, Target
    out << "date,steps,sleep_hours,kcal_in,target_kcal_next_day\n";

    QSqlQuery query(DatabaseManager::database());
    // Джоиним таблицы активности и питания по дате
    query.exec(R"(
        SELECT a.date_entry, a.steps, a.sleep_hours, IFNULL(SUM(n.kcal), 0) as kcal_in
        FROM daily_activity a
        LEFT JOIN nutrition_log n ON a.date_entry = n.date_entry
        GROUP BY a.date_entry
        ORDER BY a.date_entry ASC
    )");

    while (query.next()) {
        QString date = query.value(0).toString();
        int steps = query.value(1).toInt();
        double sleep = query.value(2).toDouble();
        double kcalIn = query.value(3).toDouble();

        // В качестве простой метрики для y (target) пока берем формулу базового обмена + активность
        // В будущем нейросеть будет сама выводить эту закономерность
        double target = 2000.0 + (steps * 0.04) + (sleep > 7.0 ? 0 : 200);

        out << date << "," << steps << "," << sleep << "," << kcalIn << "," << target << "\n";
    }

    file.close();
    QMessageBox::information(this, "Экспорт", "Датасет для обучения модели (CSV) успешно сформирован.");
}

void MainWindow::onFoodTreeContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->foodTree->itemAt(pos);
    // Меню должно работать только на продуктах, а не на категориях (у категорий нет родителя)
    if (!item || !item->parent()) return;

    QVariant idVar = item->data(0, Qt::UserRole);
    if (!idVar.isValid()) return;
    int entryId = idVar.toInt();

    QMenu menu(this);
    QAction *editAction = menu.addAction("Редактировать количество (г)");
    QAction *deleteAction = menu.addAction("Удалить из списка съеденного");

    QAction *selected = menu.exec(ui->foodTree->mapToGlobal(pos));

    if (selected == editAction) {
        bool ok;
        // Запрашиваем новый вес через стандартное всплывающее окошко
        double newWeight = QInputDialog::getDouble(this, "Редактирование порции",
                                                   "Введите новый вес порции в граммах:",
                                                   100, 1, 5000, 0, &ok);
        if (ok && newWeight > 0) {
            DatabaseManager::updateNutritionEntryWeight(entryId, newWeight);
            loadDailyData(); // Перезагружаем дерево для обновления цифр
        }
    }
    else if (selected == deleteAction) {
        auto result = QMessageBox::question(this, "Удаление",
                                            "Вы точно хотите удалить этот продукт из дневника?",
                                            QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            DatabaseManager::deleteNutritionEntry(entryId);
            loadDailyData(); // Перезагружаем дерево
        }
    }
}

void MainWindow::onProfileClicked() {
    ProfileDialog profileDialog(this);
    profileDialog.exec(); // Открываем окно модально (поверх главного)
}

void MainWindow::onChangeUserClicked() {
    this->hide(); // Прячем главное окно

    LoginDialog login(this);
    if (login.exec() == QDialog::Accepted) {
        QString nickname = login.getNickname();
        if (nickname.isEmpty()) nickname = "default_user";

        // Ищем или создаем пользователя
        int userId = DatabaseManager::getUserId(nickname);
        if (userId == -1) {
            userId = DatabaseManager::addUser(nickname);
        }
        DatabaseManager::setCurrentUserId(userId);

        // Обновляем заголовок окна
        QString currentName = DatabaseManager::getUserName(userId);
        this->setWindowTitle("Фитнес Трекер — Аккаунт: " + currentName);

        // ВАЖНО: Перезагружаем дерево питания для нового пользователя!
        onDateChanged();

        this->show(); // Показываем главное окно обратно
    } else {
        this->show(); // Если передумали и нажали "Отмена" в окне логина
    }
}
void MainWindow::loadWorkoutData() {
    QDate selectedDate = ui->calendarWidget->selectedDate();
    ui->workoutTree->clear(); // Очищаем дерево перед загрузкой

    // Превращаем дату в красивую строку, например "21.06.2026"
    QString dateString = selectedDate.toString("dd.MM.yyyy");

    QSqlQuery actQuery = DatabaseManager::getDailyActivity(selectedDate);
    if (actQuery.next()) {
        int steps = actQuery.value(0).toInt();
        double activeKcal = actQuery.value(1).toDouble();
        double sleep = actQuery.value(2).toDouble();

        // Создаем зеленую плашку для активности
        QTreeWidgetItem *actHeader = new QTreeWidgetItem(ui->workoutTree);
        actHeader->setText(0, "🍏 Фоновая активность (Синхр. с iPhone)");
        actHeader->setFirstColumnSpanned(true);
        actHeader->setBackground(0, QColor(220, 245, 220)); // Светло-зеленый фон
        actHeader->setForeground(0, QColor(0, 100, 0));     // Темно-зеленый текст
        QFont af = actHeader->font(0);
        af.setBold(true);
        actHeader->setFont(0, af);

        // Добавляем шаги
        QTreeWidgetItem *stepsItem = new QTreeWidgetItem(actHeader);
        stepsItem->setText(0, "Шаги");
        stepsItem->setText(1, QString::number(steps));

        // Добавляем активные калории
        QTreeWidgetItem *kcalItem = new QTreeWidgetItem(actHeader);
        kcalItem->setText(0, "Активные калории");
        kcalItem->setText(1, QString::number(activeKcal, 'f', 0) + " ккал");

        // Добавляем сон
        QTreeWidgetItem *sleepItem = new QTreeWidgetItem(actHeader);
        sleepItem->setText(0, "Сон");
        sleepItem->setText(1, QString::number(sleep, 'f', 1) + " ч");
    }

    // 1. Получаем ВСЕ тренировки за выбранный день
    QSqlQuery wQuery = DatabaseManager::getWorkoutsForDate(selectedDate);

    while (wQuery.next()) {
        int wId = wQuery.value(0).toInt();
        QString wName = wQuery.value(1).toString();

        // 2. Создаем ГЛАВНЫЙ узел (Название тренировки + Дата)
        QTreeWidgetItem *wItem = new QTreeWidgetItem(ui->workoutTree);

        // === ИЗМЕНЕННАЯ СТРОКА: ТЕПЕРЬ ТУТ ВЫВОДИТСЯ ДАТА ===
        wItem->setText(0, QString("Тренировка: %1   [%2]").arg(wName).arg(dateString));

        wItem->setFirstColumnSpanned(true); // Растягиваем на всю ширину
        wItem->setBackground(0, QColor(80, 80, 80)); // Темно-серый
        wItem->setForeground(0, Qt::white);             // Белый текст
        QFont wf = wItem->font(0);
        wf.setBold(true);
        wItem->setFont(0, wf);

        // 3. Получаем упражнения для этой тренировки
        QSqlQuery exQuery = DatabaseManager::getTodayWorkoutExercises(wId);
        QString lastExercise = "";
        QTreeWidgetItem *exItem = nullptr;

        while (exQuery.next()) {
            QString exName = exQuery.value(0).toString();
            int setNum = exQuery.value(1).toInt();
            int reps = exQuery.value(2).toInt();
            double weight = exQuery.value(3).toDouble();

            // Если началось новое упражнение, создаем ПОДУЗЕЛ
            if (exName != lastExercise) {
                exItem = new QTreeWidgetItem(wItem); // Кладем внутрь тренировки (wItem)
                exItem->setText(0, exName);
                exItem->setFirstColumnSpanned(true);
                exItem->setBackground(0, QColor(100, 100, 100));
                QFont ef = exItem->font(0);
                ef.setBold(true);
                exItem->setFont(0, ef);

                lastExercise = exName;
            }

            // Создаем строку с подходом и кладем её ВНУТРЬ упражнения (exItem)
            QTreeWidgetItem *setItem = new QTreeWidgetItem(exItem);
            setItem->setText(0, ""); // Под названием пусто
            setItem->setText(1, QString("Подход %1").arg(setNum));
            setItem->setText(2, QString::number(reps));
            setItem->setText(3, QString::number(weight) + " кг");
        }
    }

    // Раскрываем все папки по умолчанию, чтобы сразу было видно список
    ui->workoutTree->expandAll();
}

void MainWindow::onAddWorkoutClicked() {
    QDate selectedDate = ui->calendarWidget->selectedDate();
    int workoutId = -1;

    // Ищем, есть ли уже тренировки на этот день
    QSqlQuery q = DatabaseManager::getWorkoutsForDate(selectedDate);
    QMap<int, QString> existingWorkouts;
    while (q.next()) {
        existingWorkouts[q.value(0).toInt()] = q.value(1).toString();
    }

    if (!existingWorkouts.isEmpty()) {
        // Спрашиваем пользователя: продолжить старую или начать новую?
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Управление тренировками");
        msgBox.setText("На этот день уже есть сохраненные тренировки.");
        msgBox.setInformativeText("Вы хотите продолжить последнюю тренировку или начать новую?");

        QPushButton *btnContinue = msgBox.addButton("Продолжить", QMessageBox::ActionRole);
        QPushButton *btnNew = msgBox.addButton("Новая тренировка", QMessageBox::ActionRole);
        msgBox.addButton("Отмена", QMessageBox::RejectRole);

        msgBox.exec();

        if (msgBox.clickedButton() == btnContinue) {
            workoutId = existingWorkouts.keys().last(); // Берем последнюю начатую
        } else if (msgBox.clickedButton() == btnNew) {
            workoutId = -1; // Сигнал к созданию новой
        } else {
            return; // Отмена
        }
    }

    // Создаем новую тренировку, если выбрали "Новая" или если за день вообще ничего нет
    if (workoutId == -1) {
        bool ok;
        QString wName = QInputDialog::getText(this, "Новая тренировка",
                                              "Введите название (например: Грудь и Трицепс):",
                                              QLineEdit::Normal, "Силовая тренировка", &ok);
        if (!ok || wName.trimmed().isEmpty()) return; // Если отменили ввод

        workoutId = DatabaseManager::createNewWorkout(selectedDate, wName.trimmed());
    }

    // Запускаем окно активной тренировки
    if (workoutId != -1) {
        AddExerciseDialog dlg(workoutId, this);
        dlg.exec();
        loadWorkoutData(); // Моментально обновляем таблицу
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать тренировку в базе данных.");
    }
}

void MainWindow::onGenerateAIAdviceClicked() {
    double avgKcal, avgProt;
    int workoutCount, avgSteps;

    // Получаем данные из базы
    DatabaseManager::getWeeklyStats(avgKcal, avgProt, workoutCount, avgSteps);

    // Формируем красивый HTML-отчет
    QString advice = "<h2>📊 Анализ ваших показателей за последние 7 дней:</h2>";
    advice += QString("<ul>");
    advice += QString("<li><b>В среднем калорий:</b> %1 ккал/день</li>").arg(avgKcal, 0, 'f', 0);
    advice += QString("<li><b>В среднем белка:</b> %1 г/день</li>").arg(avgProt, 0, 'f', 1);
    advice += QString("<li><b>Силовых тренировок:</b> %1</li>").arg(workoutCount);
    advice += QString("<li><b>В среднем шагов:</b> %1/день</li>").arg(avgSteps);
    advice += QString("</ul><hr>");

    advice += "<h2>🤖 Рекомендации системы:</h2><ul>";

    // --- БЛОК АНАЛИЗА ПИТАНИЯ ---
    if (avgKcal < 100) {
        advice += "<li>⚠️ <b>Недостаточно данных:</b> Записывайте каждый прием пищи, чтобы анализ был точнее.</li>";
    } else if (avgKcal < 1300) {
        advice += "<li>🍎 <b>Питание:</b> Вы потребляете критически мало калорий. Это замедляет метаболизм и ведет к потере мышц. Попробуйте немного увеличить порции сложных углеводов.</li>";
    } else if (avgKcal > 3000) {
        advice += "<li>🍎 <b>Питание:</b> Ваш калораж достаточно высок. Если ваша цель — похудение, стоит создать дефицит в 300-400 ккал за счет уменьшения жиров и быстрых углеводов.</li>";
    } else {
        advice += "<li>🍎 <b>Питание:</b> Отличный уровень калорийности! Вы держите хороший баланс.</li>";
    }

    // --- БЛОК АНАЛИЗА ТРЕНИРОВОК И БЕЛКА ---
    if (workoutCount > 0 && avgProt < 60) {
        advice += "<li>💪 <b>Мышцы:</b> Вы активно тренируетесь, но потребляете мало белка. Добавьте в рацион больше мяса, яиц или протеина для восстановления мышц.</li>";
    } else if (workoutCount >= 3) {
        advice += "<li>🔥 <b>Тренировки:</b> У вас отличная тренировочная дисциплина! Не забывайте про качественный сон (7-8 часов) для восстановления ЦНС.</li>";
    } else if (workoutCount == 0) {
        advice += "<li>🏋️ <b>Тренировки:</b> На этой неделе не было силовых нагрузок. Постарайтесь выделить хотя бы 2 дня по 30 минут для поддержания мышечного тонуса.</li>";
    }

    // --- БЛОК АНАЛИЗА АКТИВНОСТИ ---
    if (avgSteps > 0 && avgSteps < 5000) {
        advice += "<li>🚶 <b>Активность:</b> Ваша фоновая активность снижена. Попробуйте проходить хотя бы 8000 шагов: откажитесь от лифта или гуляйте вечером.</li>";
    } else if (avgSteps >= 10000) {
        advice += "<li>🚶 <b>Активность:</b> Шикарная фоновая активность! Ваша сердечно-сосудистая система скажет вам спасибо.</li>";
    }

    advice += "</ul>";

    // Выводим результат на экран
    ui->textAI->setHtml(advice);
}
