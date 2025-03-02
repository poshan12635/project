#include "form123.h"
#include "ui_form123.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCryptographicHash>


Form123::Form123(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form123)
{
    ui->setupUi(this);
    reg=ui->lineEdit_2->text().toInt();
    cn=ui->lineEdit->text();
}

Form123::~Form123()
{
    delete ui;
}
void Form123::on_pushButton_clicked()
{
    QString classnamee = ui->lineEdit->text();
    int regno = ui->lineEdit_2->text().toInt();
    // Convert to integer
    QString password = ui->lineEdit_3->text();
    QString confirmpas = ui->lineEdit_4->text();

    // Check if any field is empty
    if (classnamee.isEmpty() || password.isEmpty() || confirmpas.isEmpty() || regno == 0) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }

    // Check if passwords match
    if (password != confirmpas) {
        QMessageBox::warning(this, "Password Mismatch", "Passwords do not match.");
        return;
    }

    // Check if classname exists
    QSqlQuery db1;
    db1.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name = :classname");
    db1.bindValue(":classname", classnamee);
    if (!db1.exec()) {
        QMessageBox::critical(this, "Database Error", "Error checking class existence: " + db1.lastError().text());
        return;
    }
    if (!db1.next()) {
        QMessageBox::warning(this, "Class Not Found", "Class does not exist.");
        return;
    }

    // Check if regno exists in the class table
    QSqlQuery db2;
    QString regquery = "SELECT * FROM " + classnamee + " WHERE Regno = :regno";
    db2.prepare(regquery);
    db2.bindValue(":regno", regno);
    if (!db2.exec()) {
        QMessageBox::critical(this, "Database Error", "Error checking registration number: " + db2.lastError().text());
        return;
    }
    if (!db2.next()) {
        QMessageBox::warning(this, "Registration Error", "Classname and RegNo do not match. You are not allowed to register.");
        return;
    }

    // ✅ Password Hashing
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Insert the student details into the `studentlog` table
    QSqlQuery db3;
    db3.prepare("INSERT INTO studentlog (Regno, Password,class) VALUES (:regno, :password, :classnamee)");
    db3.bindValue(":regno", regno);
    db3.bindValue(":password", password);
    db3.bindValue(":classnamee",classnamee);

    if (!db3.exec()) {
        QMessageBox::critical(this, "Database Error", "Error inserting data: " + db3.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", "Registration successful!");
}


void Form123::on_pushButton_2_clicked()
{
    this->close();
}

