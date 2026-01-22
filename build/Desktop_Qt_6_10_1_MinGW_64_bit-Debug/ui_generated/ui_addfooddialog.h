/********************************************************************************
** Form generated from reading UI file 'addfooddialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDFOODDIALOG_H
#define UI_ADDFOODDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AddFoodDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *searchEdit;
    QTableWidget *resultsTable;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *weightSpin;
    QComboBox *mealCombo;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnAdd;

    void setupUi(QDialog *AddFoodDialog)
    {
        if (AddFoodDialog->objectName().isEmpty())
            AddFoodDialog->setObjectName("AddFoodDialog");
        AddFoodDialog->resize(608, 400);
        verticalLayout = new QVBoxLayout(AddFoodDialog);
        verticalLayout->setObjectName("verticalLayout");
        searchEdit = new QLineEdit(AddFoodDialog);
        searchEdit->setObjectName("searchEdit");

        verticalLayout->addWidget(searchEdit);

        resultsTable = new QTableWidget(AddFoodDialog);
        if (resultsTable->columnCount() < 5)
            resultsTable->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        resultsTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        resultsTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        resultsTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        resultsTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        resultsTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        resultsTable->setObjectName("resultsTable");
        resultsTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayout->addWidget(resultsTable);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(AddFoodDialog);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        weightSpin = new QDoubleSpinBox(AddFoodDialog);
        weightSpin->setObjectName("weightSpin");
        weightSpin->setMaximum(5000.000000000000000);
        weightSpin->setValue(100.000000000000000);

        horizontalLayout->addWidget(weightSpin);

        mealCombo = new QComboBox(AddFoodDialog);
        mealCombo->setObjectName("mealCombo");

        horizontalLayout->addWidget(mealCombo);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnAdd = new QPushButton(AddFoodDialog);
        btnAdd->setObjectName("btnAdd");

        horizontalLayout->addWidget(btnAdd);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(AddFoodDialog);

        QMetaObject::connectSlotsByName(AddFoodDialog);
    } // setupUi

    void retranslateUi(QDialog *AddFoodDialog)
    {
        AddFoodDialog->setWindowTitle(QCoreApplication::translate("AddFoodDialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\277\321\200\320\276\320\264\321\203\320\272\321\202", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("AddFoodDialog", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \320\275\320\260\320\267\320\262\320\260\320\275\320\270\320\265 \320\277\321\200\320\276\320\264\321\203\320\272\321\202\320\260 \320\264\320\273\321\217 \320\277\320\276\320\270\321\201\320\272\320\260...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = resultsTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("AddFoodDialog", "\320\237\321\200\320\276\320\264\321\203\320\272\321\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = resultsTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("AddFoodDialog", "\320\232\320\272\320\260\320\273", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = resultsTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("AddFoodDialog", "\320\221\320\265\320\273\320\272\320\270", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = resultsTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("AddFoodDialog", "\320\226\320\270\321\200\321\213", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = resultsTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("AddFoodDialog", "\320\243\320\263\320\273.", nullptr));
        label->setText(QCoreApplication::translate("AddFoodDialog", "\320\222\320\265\321\201 (\320\263):", nullptr));
        btnAdd->setText(QCoreApplication::translate("AddFoodDialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\262 \320\264\320\275\320\265\320\262\320\275\320\270\320\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddFoodDialog: public Ui_AddFoodDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDFOODDIALOG_H
