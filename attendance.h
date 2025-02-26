#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QMainWindow>  // Change from QDialog to QMainWindow
#include <QTableWidget>
#include <QCheckBox>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>
#include <QSqlRecord>
#include <QComboBox>
#include <QLabel>

namespace Ui {
class attendance;
}

class attendance : public QMainWindow  // Change from QDialog
{
    Q_OBJECT

public:
    explicit attendance(QWidget *parent = nullptr);
    ~attendance();

private slots:
    void onCheckBoxClicked(Qt::CheckState state, const QString &name, const QString &tableName);
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::attendance *ui;

    // UI Components for dynamic elements
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;
    QLabel *titleLabel;

    void setupUI();  // Function to initialize scroll area and layouts dynamically
};

#endif // ATTENDANCE_H
