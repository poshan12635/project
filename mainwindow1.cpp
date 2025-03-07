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
#include <QSqlDatabase>

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

    // Get the existing admin database connection
    QSqlDatabase db1 = QSqlDatabase::database("admin");
    if (!db1.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Admin database connection is not open.");
        return;
    }

    QSqlQuery db2(db1);
    db2.prepare("SELECT * FROM studentlog WHERE Regno = :regno AND Password = :password");
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

    // Open student database
    QString dbPath = "C:/Users/karki/project/database/student.db";
    QSqlDatabase db3 = QSqlDatabase::addDatabase("QSQLITE", "student");
    db3.setDatabaseName(dbPath);

    if (!db3.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open student database: " + db3.lastError().text());
        return;
    }

    // Get the existing admin database connection
    QSqlDatabase db1 = QSqlDatabase::database("admin");
    if (!db1.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Admin database connection is not open.");
        db3.close();
        return;
    }

    // Use the existing admin connection for student details
    QSqlQuery db(db1), query(db3);
    db.prepare("SELECT Regno, Rollno, Name, Count FROM " + classname + " WHERE Regno = :regno");
    db.bindValue(":regno", regno);

    query.prepare("SELECT marks FROM " + classname + " WHERE reg_number = :regno");
    query.bindValue(":regno", regno);

    if (!db.exec()) {
        QMessageBox::critical(this, "Database Error", "Error fetching student details: " + db.lastError().text());
        db3.close();
        return;
    }

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Error fetching marks: " + query.lastError().text());
        db3.close();
        return;
    }

    bool recordFound = false;
    int rollno = 0, count = 0, marks = 0;
    QString name;

    if (db.next()) {
        recordFound = true;
        rollno = db.value("Rollno").toInt();
        name = db.value("Name").toString();
        count = db.value("Count").toInt();
    }

    if (query.next()) {
        marks = query.value("marks").toInt();
    }

    if (!recordFound) {
        QMessageBox::information(this, "No Record", "No student details found.");
        db3.close();
        return;
    }

    // Create a modal dialog to display details
    QDialog detailsWindow(this);
    detailsWindow.setWindowTitle("Student Details");
    detailsWindow.setFixedSize(450, 350);

    QVBoxLayout *mainLayout = new QVBoxLayout(&detailsWindow);
    QGroupBox *box = new QGroupBox("Student Information");
    QVBoxLayout *contentLayout = new QVBoxLayout(box);

    // Beautified Stylesheet for the Box
    box->setStyleSheet(R"(
        QGroupBox {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #f3f3f3, stop:1 #d9e6f2);
            border: 2px solid #4A90E2;
            border-radius: 10px;
            padding: 10px;
        }
        QGroupBox::title {
            background-color: transparent;
            subcontrol-origin: margin;
            padding: 6px;
            font-weight: bold;
            font-size: 14px;
            color: #2C3E50;
        }
    )");

    // Styled QLabel for Student Information
    QLabel *label = new QLabel(QString(
                                   "<b><font color='#2C3E50' size='+1'>RegNo:</font></b> %1<br>"
                                   "<b><font color='#2C3E50' size='+1'>RollNo:</font></b> %2<br>"
                                   "<b><font color='#3498DB' size='+1'>Name:</font></b> %3<br>"
                                   "<b><font color='#27AE60' size='+1'>Attendance Count:</font></b> %4<br>"
                                   "<b><font color='#E74C3C' size='+1'>Marks:</font></b> %5")
                                   .arg(regno)
                                   .arg(rollno)
                                   .arg(name)
                                   .arg(count)
                                   .arg(marks));

    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 14px; padding: 10px; background-color: white; border-radius: 5px;");

    // Adding label to layout
    contentLayout->addWidget(label);
    box->setLayout(contentLayout);
    mainLayout->addWidget(box);
    detailsWindow.setLayout(mainLayout);

    // Show as a modal dialog
    detailsWindow.exec();

    // Close the student database connection after use
    db3.close();
}
