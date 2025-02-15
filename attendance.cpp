#include "attendance.h"
#include "ui_attendance.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCheckBox>
#include <QHBoxLayout>

attendance::attendance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::attendance)
{
    ui->setupUi(this);
    ui->comboBox->addItem("ce23");
    ui->comboBox->addItem("ce24");
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &attendance::onComboBoxChanged);
}

attendance::~attendance()
{
    delete ui;
}

void attendance::onComboBoxChanged(const QString &selectedText)
{
    qDebug() << "ComboBox changed to:" << selectedText; // Debug message

    // Ensure the table is cleared before loading new data
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(2);

    // Set table headers
    QStringList headers;
    headers << "Name" << "Status";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Validate if a valid table is selected
    if (selectedText != "ce23" && selectedText != "ce24") {
        qDebug() << "Invalid table selected:" << selectedText; // Debug message
        return;  // If it's not ce23 or ce24, do nothing
    }

    // Query the database for the selected table
    QSqlQuery query;
    QString queryStr = QString("SELECT Name FROM %1").arg(selectedText);
    qDebug() << "Executing query:" << queryStr; // Debug message

    if (!query.exec(queryStr)) {
        qDebug() << "Query failed:" << query.lastError().text(); // Debug message
        QMessageBox::critical(this, "Query Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    // Insert data into the table
    int row = 0;
    while (query.next()) {
        QString name = query.value(0).toString(); // Get Name column data
        qDebug() << "Name:" << name; // Debug message

        // Insert a new row
        ui->tableWidget->insertRow(row);

        // Set Name column
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // Make it read-only
        ui->tableWidget->setItem(row, 0, nameItem);

        // Create checkbox in Status column
        QCheckBox *checkBox = new QCheckBox();
        connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) { onCheckboxClicked(state, name, selectedText); });

        QWidget *checkBoxWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(checkBoxWidget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        checkBoxWidget->setLayout(layout);

        ui->tableWidget->setCellWidget(row, 1, checkBoxWidget);

        row++;
    }

    // Resize columns for a better view
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
}

// Function to update "count" column in database when checkbox is clicked
void attendance::onCheckboxClicked(int state, const QString &name, const QString &tableName)
{
    if (state == Qt::Checked) {  // Only increase count when checked
        QSqlQuery query;
        QString updateQuery = QString("UPDATE %1 SET count = count + 1 WHERE Name = :name").arg(tableName);
        query.prepare(updateQuery);
        query.bindValue(":name", name);

        if (!query.exec()) {
            QMessageBox::critical(this, "Update Error", "Failed to update count: " + query.lastError().text());
        }
    }
}

void attendance::on_pushButton_clicked()
{
    this->close();
}

