#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class Form;
}

class Form : public QMainWindow
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private slots:
    void on_addbutton_clicked();

    void on_pushButton_2_clicked();

    void on_deletebutton_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
