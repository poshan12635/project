#include "form.h"
#include "ui_form.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QLabel>

Form::Form(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

}

Form::~Form()
{
    delete ui;
}

void Form::on_addbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);  // Show page with groupbox2
    on_pushButton_2_clicked();

}

void Form::on_deletebutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);  // Show page with groupbo
    on_pushButton_clicked();
}

void Form::on_pushButton_2_clicked()
{
    QString cname = ui->classname->text();
    QString name = ui->nameofst->text();
    int regno = ui->regnoofstd->text().toInt();
    int rollno = ui->rollnostd->text().toInt();
    int count = 0;

    QSqlQuery db;
    db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name = :cname");
    db.bindValue(":cname", cname);

    if (db.exec() && db.next()) {
        QString queryStr = QString("INSERT INTO %1 (Regno, Rollno, Name, Count) VALUES (:regno, :rollno, :name, :count)").arg(cname);
        db.prepare(queryStr);
        db.bindValue(":regno", regno);
        db.bindValue(":rollno", rollno);
        db.bindValue(":name", name);
        db.bindValue(":count", count);

        if (db.exec()) {
            QMessageBox::information(this, "Success", "Record inserted successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to insert record: " + db.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "Error", "Class name does not exist. Input a valid class name.");
    }
}

void Form::on_pushButton_clicked()
{
    QString cname = ui->classname_2->text();
    int regno = ui->regnoofstd_2->text().toInt();

    QSqlQuery db;
    db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name = :cname");
    db.bindValue(":cname", cname);

    if (db.exec() && db.next()) {
        QString queryStr = QString("DELETE FROM %1 WHERE Regno = :regno").arg(cname);
        db.prepare(queryStr);
        db.bindValue(":regno", regno);

        if (db.exec()) {
            if (db.numRowsAffected() > 0) {
                QMessageBox::information(this, "Success", "Record deleted successfully.");
            } else {
                QMessageBox::warning(this, "Not Found", "No matching record found.");
            }
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete record: " + db.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "Error", "Class name does not exist. Input a valid class name.");
    }
}
