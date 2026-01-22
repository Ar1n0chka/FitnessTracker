/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabNutrition;
    QVBoxLayout *layoutNutri;
    QHBoxLayout *horizontalLayout;
    QCalendarWidget *calendarWidget;
    QTreeWidget *foodTree;
    QLabel *lblTotal;
    QPushButton *btnAddFood;
    QWidget *tabWorkout;
    QVBoxLayout *layoutWork;
    QTableWidget *workoutTable;
    QPushButton *btnAddWorkout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabNutrition = new QWidget();
        tabNutrition->setObjectName("tabNutrition");
        layoutNutri = new QVBoxLayout(tabNutrition);
        layoutNutri->setObjectName("layoutNutri");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        calendarWidget = new QCalendarWidget(tabNutrition);
        calendarWidget->setObjectName("calendarWidget");

        horizontalLayout->addWidget(calendarWidget);

        foodTree = new QTreeWidget(tabNutrition);
        foodTree->setObjectName("foodTree");

        horizontalLayout->addWidget(foodTree);


        layoutNutri->addLayout(horizontalLayout);

        lblTotal = new QLabel(tabNutrition);
        lblTotal->setObjectName("lblTotal");

        layoutNutri->addWidget(lblTotal);

        btnAddFood = new QPushButton(tabNutrition);
        btnAddFood->setObjectName("btnAddFood");

        layoutNutri->addWidget(btnAddFood);

        tabWidget->addTab(tabNutrition, QString());
        tabWorkout = new QWidget();
        tabWorkout->setObjectName("tabWorkout");
        layoutWork = new QVBoxLayout(tabWorkout);
        layoutWork->setObjectName("layoutWork");
        workoutTable = new QTableWidget(tabWorkout);
        workoutTable->setObjectName("workoutTable");
        workoutTable->setColumnCount(4);

        layoutWork->addWidget(workoutTable);

        btnAddWorkout = new QPushButton(tabWorkout);
        btnAddWorkout->setObjectName("btnAddWorkout");

        layoutWork->addWidget(btnAddWorkout);

        tabWidget->addTab(tabWorkout, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        QTreeWidgetItem *___qtreewidgetitem = foodTree->headerItem();
        ___qtreewidgetitem->setText(5, QCoreApplication::translate("MainWindow", "\320\243", nullptr));
        ___qtreewidgetitem->setText(4, QCoreApplication::translate("MainWindow", "\320\226", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "\320\221", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "\320\232\320\272\320\260\320\273", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "\320\222\320\265\321\201", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "\320\237\321\200\320\276\320\264\321\203\320\272\321\202", nullptr));
        lblTotal->setText(QCoreApplication::translate("MainWindow", "\320\230\321\202\320\276\320\263\320\276: 0 \320\272\320\272\320\260\320\273", nullptr));
        btnAddFood->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\277\321\200\320\270\320\265\320\274 \320\277\320\270\321\211\320\270", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabNutrition), QCoreApplication::translate("MainWindow", "\320\224\320\275\320\265\320\262\320\275\320\270\320\272 \320\277\320\270\321\202\320\260\320\275\320\270\321\217", nullptr));
        btnAddWorkout->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\277\320\270\321\201\320\260\321\202\321\214 \321\202\321\200\320\265\320\275\320\270\321\200\320\276\320\262\320\272\321\203", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabWorkout), QCoreApplication::translate("MainWindow", "\320\242\321\200\320\265\320\275\320\270\321\200\320\276\320\262\320\272\320\270", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
