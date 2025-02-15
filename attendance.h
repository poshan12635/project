#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QDialog>
#include <QTableWidget>  // 👈 Include this

namespace Ui {
class attendance;
}

class attendance : public QDialog
{
    Q_OBJECT

public:
    explicit attendance(QWidget *parent = nullptr);
    ~attendance();

private slots:
    void onComboBoxChanged(const QString &selectedText);
    void onCheckboxClicked(int state, const QString &name, const QString &tableName);

    void on_pushButton_clicked();

private:
    Ui::attendance *ui;
};

#endif // ATTENDANCE_H
