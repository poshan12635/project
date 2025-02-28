#ifndef MAINWINDOW1_H
#define MAINWINDOW1_H

#include <QMainWindow>

namespace Ui {
class MainWindow1;
}

class MainWindow1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow1(QWidget *parent = nullptr);
    ~MainWindow1();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();
    void show_details(int regno, QString classname);

private:
    Ui::MainWindow1 *ui;
};

#endif // MAINWINDOW1_H
