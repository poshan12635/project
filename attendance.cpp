#include "attendance.h"
#include "ui_attendance.h"

attendance::attendance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::attendance)
{
    ui->setupUi(this);


}

attendance::~attendance()
{
    delete ui;
}
