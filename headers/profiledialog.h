#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <cmath>

class ProfileDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProfileDialog(QWidget *parent = nullptr);

private slots:
    void calculateMetrics();

private:
    QLineEdit *leWeight, *leHeight, *leNeck, *leWaist, *leHips;
    QComboBox *cbGender;
    QLabel *lblResult;
    QTableWidget *historyTable;

    void loadHistory();
};

#endif
