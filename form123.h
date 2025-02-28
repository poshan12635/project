#ifndef FORM123_H
#define FORM123_H

#include <QWidget>

namespace Ui {
class Form123;
}

class Form123 : public QWidget
{
    Q_OBJECT

public:
    explicit Form123(QWidget *parent = nullptr);
    int reg;
    QString cn;
    ~Form123();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Form123 *ui;
};

#endif // FORM123_H
