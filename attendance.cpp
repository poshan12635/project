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
#include <QComboBox>

attendance::attendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::attendance)
{
    ui->setupUi(this);
    this->showMaximized();

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *universityLabel = new QLabel("Kathmandu University", this);
    universityLabel->setAlignment(Qt::AlignCenter);
    universityLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: white; "
                                   "background-color: #0056b3; padding: 12px; "
                                   "border-radius: 10px;");

    titleLabel = new QLabel("Attendance Management System", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white; "
                              "background-color: #007BFF; padding: 10px; "
                              "border-radius: 10px;");

    mainLayout->addWidget(universityLabel);
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *contentLayout = new QHBoxLayout();

    ui->groupBox->setFixedWidth(180); // **Smaller width for button area**
    ui->groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->groupBox_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    contentLayout->addWidget(ui->groupBox);
    contentLayout->addWidget(ui->groupBox_2, 1); // GroupBox_2 takes most of the space

    mainLayout->addLayout(contentLayout);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}



attendance::~attendance()
{
    delete ui;
}

void attendance::on_pushButton_clicked()
{
    // Ensure layout exists
    if (!ui->groupBox_2->layout()) {
        ui->groupBox_2->setLayout(new QVBoxLayout());
    }

    // Clear previous contents
    QLayout *layout = ui->groupBox_2->layout();
    while (QLayoutItem *child = layout->takeAt(0)) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // Create a ComboBox and add it to layout
    QComboBox *comboBox = new QComboBox();
    layout->addWidget(comboBox);

    // Fetch class names from the database (excluding system tables and "users")
    QSqlQuery classQuery("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' AND name != 'users'");
    while (classQuery.next()) {
        comboBox->addItem(classQuery.value(0).toString());
    }

    // Scroll Area for student data
    QScrollArea *scrollArea = new QScrollArea(ui->groupBox_2);
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    // Load data based on selected class
    connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString &className) {
        // Clear previous content
        while (QLayoutItem *child = scrollLayout->takeAt(0)) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }

        qDebug() << "Fetching data for class: " << className;

        // Table for student attendance
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
            return;
        }

        int row = 0;
        while (studentQuery.next()) {
            QString studentName = studentQuery.value(0).toString();

            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(studentName));

            // Checkbox for attendance
            QCheckBox *checkBox = new QCheckBox();
            QWidget *checkBoxWidget = new QWidget();
            QHBoxLayout *hLayout = new QHBoxLayout(checkBoxWidget);
            hLayout->addWidget(checkBox);
            hLayout->setAlignment(Qt::AlignCenter);
            hLayout->setContentsMargins(0, 0, 0, 0);
            checkBoxWidget->setLayout(hLayout);
            tableWidget->setCellWidget(row, 1, checkBoxWidget);

            // Connect checkbox to update function
            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                onCheckBoxClicked(static_cast<Qt::CheckState>(state), studentName, className);
            });

            row++;
        }

        tableWidget->resizeColumnsToContents();
        tableWidget->resizeRowsToContents();
        scrollLayout->addWidget(tableWidget);
    });

    // Default: Load first class if available
    if (comboBox->count() > 0) {
        comboBox->setCurrentIndex(0);
        emit comboBox->currentTextChanged(comboBox->currentText());
    }

    // Finalizing Scroll Area
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);
}

void attendance::onCheckBoxClicked(Qt::CheckState state, const QString &name, const QString &tableName)
{
    if (state == Qt::Checked) {
        if (tableName.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Table name or Name cannot be empty.");
            return;
        }

        QSqlQuery query;
        QString updateQuery = QString("UPDATE %1 SET count = count + 1 WHERE Name = :name").arg(tableName);
        qDebug() << "Executing query:" << updateQuery;
        query.prepare(updateQuery);
        query.bindValue(":name", name);

        if (!query.exec()) {
            qDebug() << "Update failed:" << query.lastError().text();
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
        int regno=values[0].trimmed().toInt();
        int rollNo = values[1].trimmed().toInt();

        QString name = values[2].trimmed();

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
