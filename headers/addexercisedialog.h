#ifndef ADDEXERCISEDIALOG_H
#define ADDEXERCISEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTimer>
#include <QTableWidget>

class AddExerciseDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddExerciseDialog(int workoutId, QWidget *parent = nullptr);

private slots:
    void onAddSetClicked();
    void onExerciseChanged();
    void startTimer();
    void updateTimer();

private:
    int currentWorkoutId;
    QComboBox *cbExercises;
    QSpinBox *sbReps;
    QDoubleSpinBox *sbWeight;
    QTableWidget *setsTable; // Таблица подходов внутри окна тренировки

    QLabel *lblTimer;
    QTimer *restTimer;
    int timeLeft;
    QSpinBox *sbRestTime;

    void loadSetsForCurrentExercise();
};

#endif
