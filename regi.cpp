#include "regi.h"
#include "ui_regi.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

regi::regi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::regi)
{
    ui->setupUi(this);


}

regi::~regi()
{
    delete ui;
}


void regi::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    QString confirmPassword = ui->lineEdit_3->text();


    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }


    if (password != confirmPassword) {
        QMessageBox::warning(this, "Password Mismatch", "Passwords do not match.");
        return;
    }


    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QMessageBox::critical(this, "Registration Error", "Failed to register: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Registration Successful", "You have been registered!");
    this->close();
}


void regi::on_pushButton_2_clicked()
{
    this->close();
}
