#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCryptographicHash>
#include "regi.h"
#include "attendance.h"
#include <QDebug>
#include"mainwindow1.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    // Database connection
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = "C:/Users/karki/project/database/admin.db";
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database.");
        return;
    }

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

QString MainWindow::hashPassword(const QString &password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// Login button clicked
void MainWindow::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    QString hashedPassword = hashPassword(password);  // Hash the entered password

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter both username and password.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);  // ✅ Use the hashed password

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }

    if (query.next()) {
        if (!attendanceWindow) {
            attendanceWindow = new attendance();
             attendanceWindow->setAttribute(Qt::WA_DeleteOnClose);
        }
        attendanceWindow->showMaximized();


    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}

// Register button clicked
void MainWindow::on_pushButton_2_clicked()
{
    regi *regiWindow = new regi(this);
    regiWindow->setModal(true);
    regiWindow->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    MainWindow1 *mainwindow=new MainWindow1(this);
    mainwindow->setWindowModality(Qt::ApplicationModal);
    mainwindow->show();
}

