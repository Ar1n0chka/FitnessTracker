#ifndef ADDFOODDIALOG_H
#define ADDFOODDIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui { class AddFoodDialog; }

class AddFoodDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddFoodDialog(QDate currentDate, QWidget *parent = nullptr);
    ~AddFoodDialog();

private slots:
    void onSearchTextChanged(const QString &text);
    void onProductSelected();
    void onAddClicked();
    void updateNutritionInfo(double weight);   // Обновляет расчёт БЖУ
    void onCreateFoodClicked();

private:
    Ui::AddFoodDialog *ui;
    QDate targetDate;

    // Значения выбранного продукта на 100 г
    double selectedKcal  = 0.0;
    double selectedProt  = 0.0;
    double selectedFat   = 0.0;
    double selectedCarbs = 0.0;
};

#endif // ADDFOODDIALOG_H
