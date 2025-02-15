#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QDialog>

namespace Ui {
class attendance;  // Forward declaration of UI class
}

class attendance : public QDialog
{
    Q_OBJECT

public:
    explicit attendance(QWidget *parent = nullptr);
    ~attendance();

private:
    Ui::attendance *ui;  // This was missing
};

#endif // ATTENDANCE_H
