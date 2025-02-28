#include "mainwindow1.h"
#include "ui_mainwindow1.h"
#include "form123.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCryptographicHash>
#include "regi.h"
#include "attendance.h"
#include <QDebug>
#include "form1234.h"

MainWindow1::MainWindow1(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow1)
{
    ui->setupUi(this);
}

MainWindow1::~MainWindow1()
{
    delete ui;
}

void MainWindow1::on_pushButton_2_clicked()
{
    Form123 *main = new Form123(this);
    main->setWindowModality(Qt::ApplicationModal);
    main->show();
}

void MainWindow1::on_pushButton_clicked()
{
    int regno = ui->lineEdit->text().toInt();
    QString pass = ui->lineEdit_2->text();

    // Validate empty fields
    if (regno == 0 || pass.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }

    QSqlQuery db2;
    db2.prepare("SELECT *  FROM studentlog WHERE Regno = :regno AND Password = :password");
    db2.bindValue(":regno", regno);
    db2.bindValue(":password", pass);

    if (!db2.exec()) {
        QMessageBox::critical(this, "Database Error", "Error querying data: " + db2.lastError().text());
        return;
    }

    if (db2.next()) {  // If login is successful
        int dbRegno = db2.value("Regno").toInt();
        QString dbPassword = db2.value("Password").toString();
        QString classname = db2.value(2).toString();

        // Verify that the fetched data is correct
        if (dbRegno != regno || dbPassword != pass) {
            QMessageBox::critical(this, "Login Error", "Incorrect username or password.");
            return;
        }

        if (classname.isEmpty()) {
            QMessageBox::critical(this, "Error", "Classname not found for the student.");
            return;
        }

        // Call show_details with regno and classname
        show_details(regno, classname);
    } else {
        QMessageBox::critical(this, "Invalid Login", "Incorrect username or password.");
    }
}

void MainWindow1::show_details(int regno, QString classname)
{
    if (classname.isEmpty()) {
        QMessageBox::critical(this, "Error", "Classname not found!");
        return;
    }

    QSqlQuery db1;
    db1.prepare("SELECT Regno, Rollno, Name, Count FROM " + classname + " WHERE Regno = :regno");
    db1.bindValue(":regno", regno);

    if (!db1.exec()) {
        QMessageBox::critical(this, "Database Error", "Error fetching details: " + db1.lastError().text());
        return;
    }

    // Use QDialog for a small popup window
    QDialog *detailsWindow = new QDialog(this);
    detailsWindow->setWindowTitle("Student Details");
    detailsWindow->setFixedSize(350, 250); // Adjusted size for better visibility

    QVBoxLayout *mainLayout = new QVBoxLayout(detailsWindow);
    QGroupBox *box = new QGroupBox("Student Information");
    QVBoxLayout *contentLayout = new QVBoxLayout(box);

    // Apply the provided stylesheet
    box->setStyleSheet(R"(
        QGroupBox {
            background-color: white;
            border: 1px solid gray;
            border-radius: 5px;
        }
        QGroupBox::title {
            background-color: transparent;
            subcontrol-origin: margin;
            padding: 5px;
        }
        QGroupBox QLabel {
            font-size: 18px;
            color: black;
        }
        QPushButton {
            background-color: #007BFF;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 5px 10px;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton:pressed {
            background-color: #004494;
        }
    )");

    bool recordFound = false;

    while (db1.next()) {
        recordFound = true;
        int rollno = db1.value("Rollno").toInt();
        QString name = db1.value("Name").toString();
        int count = db1.value("Count").toInt();

        QLabel *label = new QLabel(QString("RegNo: %1\nRollNo: %2\nName: %3\nCount: %4")
                                       .arg(regno)
                                       .arg(rollno)
                                       .arg(name)
                                       .arg(count));
        contentLayout->addWidget(label);
    }

    if (!recordFound) {
        QMessageBox::information(this, "No Record", "No student details found.");
        return;
    }

    box->setLayout(contentLayout);
    mainLayout->addWidget(box);
    detailsWindow->setLayout(mainLayout);

    // Show as a modal dialog
    detailsWindow->exec();
}
