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
    void onAddClicked();
    void onTableClicked(int row, int col); // Чтобы выбрать продукт кликом

private:
    Ui::AddFoodDialog *ui;
    QDate targetDate;

    // Временное хранение выбранного БЖУ на 100г
    double selKcal = 0, selProt = 0, selFat = 0, selCarbs = 0;
};

#endif // ADDFOODDIALOG_H
