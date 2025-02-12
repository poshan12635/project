#ifndef REGI_H
#define REGI_H

#include <QDialog>

namespace Ui {
class regi;
}

class regi : public QDialog  // ✅ Ensure inheritance from QDialog
{
    Q_OBJECT

public:
    explicit regi(QWidget *parent = nullptr);  // ✅ Constructor
    ~regi();

private slots:
    void on_pushButton_clicked();  // ✅ Register Button
    void on_pushButton_2_clicked(); // ✅ Back to Login Button

private:
    Ui::regi *ui;
};

#endif // REGI_H
