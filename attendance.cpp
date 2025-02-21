#include "attendance.h"
#include "ui_attendance.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QInputDialog>
#include <QSqlRecord>


attendance::attendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::attendance)
{
    ui->setupUi(this);

    // Set window properties
    this->setWindowTitle("Attendance System");
    this->showMaximized(); // Ensure full screen


    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

     ui->groupBox->setMinimumWidth(200);
    ui->groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    mainLayout->addWidget(ui->groupBox, 1);
    mainLayout->addWidget(ui->groupBox_2, 8);


    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}

attendance::~attendance()
{
    delete ui;
}

void attendance::on_pushButton_clicked()
{

    if (!ui->groupBox_2->layout()) {
        ui->groupBox_2->setLayout(new QVBoxLayout());
    }

    //  Clear previous contents inside GroupBox_2 safely
    QLayout *layout = ui->groupBox_2->layout();
    while (QLayoutItem *child = layout->takeAt(0)) {
        if (child->widget()) {
            child->widget()->deleteLater(); // Safe deletion
        }
        delete child;
    }

    //  Scroll Area Setup (Ensure it's properly parented)
    QScrollArea *scrollArea = new QScrollArea(ui->groupBox_2);
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    //  Query to get all table names (excluding system tables)
    QSqlQuery classQuery("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' AND name != 'users'");

    while (classQuery.next()) {
        QString className = classQuery.value(0).toString();
        qDebug() << "Fetching data for table:" << className;

        // Add Label for Class Name
        QLabel *classLabel = new QLabel("Class: " + className);
        classLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        scrollLayout->addWidget(classLabel);

        //  Create TableWidget for students in this class
        QTableWidget *tableWidget = new QTableWidget();
        tableWidget->setColumnCount(2);
        tableWidget->setHorizontalHeaderLabels({"Name", "Status"});
        tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Query students for this class
        QSqlQuery studentQuery;
        studentQuery.prepare(QString("SELECT Name FROM %1").arg(className));

        if (!studentQuery.exec()) {
            qDebug() << "Query failed for table " << className << ":" << studentQuery.lastError().text();
            QMessageBox::critical(this, "Query Error", "Failed to fetch data from " + className + ": " + studentQuery.lastError().text());
            continue;
        }

        int row = 0;
        while (studentQuery.next()) {
            QString studentName = studentQuery.value(0).toString();

            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(studentName));

            // Add CheckBox for Attendance
            QCheckBox *checkBox = new QCheckBox();
            QWidget *checkBoxWidget = new QWidget();
            QHBoxLayout *hLayout = new QHBoxLayout(checkBoxWidget);
            hLayout->addWidget(checkBox);
            hLayout->setAlignment(Qt::AlignCenter);
            hLayout->setContentsMargins(0, 0, 0, 0);
            checkBoxWidget->setLayout(hLayout);
            tableWidget->setCellWidget(row, 1, checkBoxWidget);

            row++;
        }

        // Adjust table size
        tableWidget->resizeColumnsToContents();
        tableWidget->resizeRowsToContents();

        // Add table to layout
        scrollLayout->addWidget(tableWidget);
    }

    // Finalizing Scroll Area
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    //  Add Scroll Area to GroupBox_2
    layout->addWidget(scrollArea);
}


void attendance::onCheckBoxClicked(Qt::CheckState state, const QString &name, const QString &tableName)
{
    if (state == Qt::Checked) { // Only increase count if checked (present)
        if (tableName.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Table name or Name cannot be empty.");
            return;
        }

        QSqlQuery query;
        QString updateQuery = QString("UPDATE %1 SET count = count + 1 WHERE Name = :name").arg(tableName);

        // Debugging: Log the query being executed
        qDebug() << "Executing query:" << updateQuery;
        qDebug() << "With name:" << name;

        if (!query.prepare(updateQuery)) {
            qDebug() << "Query preparation failed:" << query.lastError().text();
            QMessageBox::critical(this, "Query Error", "Failed to prepare query: " + query.lastError().text());
            return;
        }

        query.bindValue(":name", name);

        if (!query.exec()) {
            qDebug() << "Query execution failed:" << query.lastError().text();
            QMessageBox::critical(this, "Update Error", "Failed to update count: " + query.lastError().text());
        } else {
            qDebug() << "Count updated successfully for" << name;
        }
    }
}


void attendance::on_pushButton_3_clicked()
{
    // Step 1: Ask for Class Name
    bool ok;
    QString className = QInputDialog::getText(this, "Enter Class Name", "Class Name:", QLineEdit::Normal, "", &ok);

    if (!ok || className.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Class name cannot be empty.");
        return;
    }

    // Step 2: Open File Dialog
    QString filePath = QFileDialog::getOpenFileName(this, "Select CSV File", "", "CSV Files (*.csv);;All Files (*.*)");

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "File Selection", "No file selected.");
        return;
    }

    // Step 3: Open the CSV File
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Failed to open the file.");
        return;
    }

    QTextStream in(&file);

    // Step 4: Create the Table for this Class
    QSqlQuery createTableQuery;
    QString createQuery = QString("CREATE TABLE IF NOT EXISTS %1 (Regno INTEGER PRIMARY KEY,Rollno INTEGER,Name TEXT , count INTEGER DEFAULT 0)").arg(className);

    if (!createTableQuery.exec(createQuery)) {
        qDebug() << "Table creation failed:" << createTableQuery.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to create table.");
        return;
    }

    // Step 5: Read and Insert Data from CSV
    QSqlQuery insertQuery;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");

        if (values.size() < 5) {
            qDebug() << "Invalid row format, skipping:" << line;
            continue;
        }

        QString name = values[2].trimmed();
        int rollNo = values[1].trimmed().toInt();
        int regno=values[0].trimmed().toInt();

        // Insert into database

        insertQuery.prepare(QString("INSERT INTO %1 (Regno,Rollno,Name) VALUES (:regno,:rollno, :name)").arg(className));
        insertQuery.bindValue(":regno",regno);
        insertQuery.bindValue(":rollno", rollNo);
        insertQuery.bindValue(":name", name);

        if (!insertQuery.exec()) {
            qDebug() << "Insert failed for" << name << ":" << insertQuery.lastError().text();
        }
    }

    file.close();
    QMessageBox::information(this, "Success", "CSV data uploaded successfully for class: " + className);
}


void attendance::on_pushButton_2_clicked()
{
    bool ok;
    QString className = QInputDialog::getText(this, "Enter Class Name", "Class Name:", QLineEdit::Normal, "", &ok);

    if (!ok || className.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Class name cannot be empty.");
        return;
    }

    // Fetch available class names from database
    QSqlQuery query;
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' AND name != 'users'");

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to fetch class names: " + query.lastError().text());
        return;
    }

    // Store class names in a list
    QStringList classList;
    while (query.next()) {
        classList.append(query.value(0).toString());
    }

    // Check if the entered class name exists
    if (!classList.contains(className)) {
        QMessageBox::warning(this, "Class Not Found", "The class name does not exist in the database.");
        return;
    }

    // Ask user for file path to save CSV
    QString filePath = QFileDialog::getSaveFileName(this, "Save CSV File", className + ".csv", "CSV Files (*.csv);;All Files (*.*)");

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Download Cancelled", "No file selected.");
        return;
    }

    // Fetch class data
    query.prepare(QString("SELECT * FROM %1").arg(className));

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    // Open file for writing
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Failed to create file.");
        return;
    }

    // Write data to CSV
    QTextStream out(&file);
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // Write header row
    for (int i = 0; i < columnCount; ++i) {
        out << record.fieldName(i);
        if (i < columnCount - 1) out << ",";
    }
    out << "\n";

    // Write data rows
    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            out << query.value(i).toString();
            if (i < columnCount - 1) out << ",";
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Download Complete", "CSV file saved successfully at: " + filePath);
}


void attendance::on_pushButton_4_clicked()
{
    this->close();
}

