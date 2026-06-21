#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QtCharts>



class StatisticsDialog : public QDialog {
    Q_OBJECT
public:
    explicit StatisticsDialog(QWidget *parent = nullptr);

private:
    void setupChart();
    QChartView *chartView;
};

#endif // STATISTICSDIALOG_H
