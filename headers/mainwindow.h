#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QCalendarWidget>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddFoodClicked();
    void onDateChanged();
    void onSyncClicked();
    void onStatsClicked();
    void onExportMLClicked();
    void onFoodTreeContextMenu(const QPoint &pos);
    void onProfileClicked();
    void onChangeUserClicked();
    void loadWorkoutData();
    void onAddWorkoutClicked();
    void onGenerateAIAdviceClicked();


private:
    Ui::MainWindow *ui;
    void loadDailyData();
    QTreeWidgetItem* getOrCreateMealCategory(int mealType);

};

#endif
