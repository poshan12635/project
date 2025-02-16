#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QDialog>
#include <QTableWidget>
#include <QCheckBox>

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
    void onCheckBoxClicked(Qt::CheckState state, const QString &name, const QString &tableName);  // Updated to use Qt::CheckState

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::attendance *ui;
};

#endif // ATTENDANCE_H
