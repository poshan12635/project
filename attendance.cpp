#include "attendance.h"
#include "ui_attendance.h"

attendance::attendance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::attendance)
{
    ui->setupUi(this);
    ui->comboBox->addItem("ce23");
    ui->comboBox->addItem("ce24");

}

attendance::~attendance()
{
    delete ui;
}
