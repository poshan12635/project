#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "regi.h"
#include "attendance.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Users\\karki\\project\\database\\admin.db");


    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database.");
        return;
    }


    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();


    // Prepare the SQL query
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");

    // Bind values to the placeholders
    query.bindValue(":username", username);  // Binding the username parameter
    query.bindValue(":password", password);

    // Debugging: Check the query being executed
    qDebug() << "Executing query: " << query.executedQuery();

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        db.close();
        return;
    }

    // Check if query returns any result
   else  if (query.next()) {
        // Login successful
        QMessageBox::information(this, "Login Successful", "Welcome!");
        attendance *attendanceWindow=new attendance(this);
        attendanceWindow->setModal(true);
        attendanceWindow->show();


    } else {
        // Login failed
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }

    // Close the database connection
    db.close();
}

void MainWindow::on_pushButton_2_clicked()
{
    regi *regiWindow = new regi(this);
    regiWindow->setModal(true);
    regiWindow->show();
}
