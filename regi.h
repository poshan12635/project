#ifndef REGI_H
#define REGI_H

#include <QDialog>

namespace Ui {
class regi;
}

class regi : public QDialog
{
    Q_OBJECT

public:
    explicit regi(QWidget *parent = nullptr);
    ~regi();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    QString hashPassword(const QString &password);

private:
    Ui::regi *ui;
};

#endif // REGI_H
