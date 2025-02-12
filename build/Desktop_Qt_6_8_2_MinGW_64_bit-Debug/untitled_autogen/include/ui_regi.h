/********************************************************************************
** Form generated from reading UI file 'regi.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGI_H
#define UI_REGI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_regi
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *regi)
    {
        if (regi->objectName().isEmpty())
            regi->setObjectName("regi");
        regi->resize(472, 318);
        regi->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-image: url(:/Desktop/IMG_9514.JPG);\n"
"    background-repeat: no-repeat;\n"
"    background-size: cover;\n"
"}\n"
"QGroupBox {\n"
"    background: transparent;\n"
"    border: 2px solid gray;\n"
"}\n"
""));
        verticalLayout_2 = new QVBoxLayout(regi);
        verticalLayout_2->setObjectName("verticalLayout_2");
        groupBox = new QGroupBox(regi);
        groupBox->setObjectName("groupBox");
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(120, 20, 181, 41));
        label->setStyleSheet(QString::fromUtf8("QLabel{\n"
"font-size:20px}"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 120, 91, 31));
        label_2->setStyleSheet(QString::fromUtf8("QLabel{\n"
"font-size:20px}"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 170, 91, 31));
        label_3->setStyleSheet(QString::fromUtf8("QLabel{\n"
"font-size:20px}"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 220, 91, 31));
        label_4->setStyleSheet(QString::fromUtf8("QLabel{\n"
"font-size:20px}"));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(110, 120, 321, 21));
        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(110, 170, 321, 21));
        lineEdit_3 = new QLineEdit(groupBox);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setGeometry(QRect(110, 220, 321, 21));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(10, 260, 141, 41));
        pushButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"font-size:20px}"));
        pushButton_2 = new QPushButton(groupBox);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(260, 260, 131, 41));
        pushButton_2->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"font-size:20px}"));

        verticalLayout_2->addWidget(groupBox);


        retranslateUi(regi);

        QMetaObject::connectSlotsByName(regi);
    } // setupUi

    void retranslateUi(QWidget *regi)
    {
        regi->setWindowTitle(QCoreApplication::translate("regi", "Form", nullptr));
        groupBox->setTitle(QString());
        label->setText(QCoreApplication::translate("regi", "Admin Registration", nullptr));
        label_2->setText(QCoreApplication::translate("regi", "Username", nullptr));
        label_3->setText(QCoreApplication::translate("regi", "Password", nullptr));
        label_4->setText(QCoreApplication::translate("regi", "Confirm", nullptr));
        pushButton->setText(QCoreApplication::translate("regi", "Register", nullptr));
        pushButton_2->setText(QCoreApplication::translate("regi", "Login Page", nullptr));
    } // retranslateUi

};

namespace Ui {
    class regi: public Ui_regi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGI_H
