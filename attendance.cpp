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
#include "form.h"

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
    ui->groupBox->setFixedWidth(180);
    ui->groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    contentLayout->addWidget(ui->groupBox);
    contentLayout->addWidget(ui->groupBox_2, 1);
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
    if (!ui->groupBox_2->layout()) {
        ui->groupBox_2->setLayout(new QVBoxLayout());
    }

    QLayout *layout = ui->groupBox_2->layout();
    while (QLayoutItem *child = layout->takeAt(0)) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QComboBox *comboBox = new QComboBox();
    layout->addWidget(comboBox);

    QSqlQuery classQuery("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' AND name != 'users' and name !='studentlog'");
    while (classQuery.next()) {
        comboBox->addItem(classQuery.value(0).toString());
    }

    QScrollArea *scrollArea = new QScrollArea(ui->groupBox_2);
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString &className) {
        while (QLayoutItem *child = scrollLayout->takeAt(0)) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }

        QTableWidget *tableWidget = new QTableWidget();
        tableWidget->setColumnCount(3);
        tableWidget->setHorizontalHeaderLabels({"Name", "Status", "Reset"});
        tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QSqlQuery studentQuery;
        studentQuery.prepare(QString("SELECT Name FROM %1 order by Rollno ASC").arg(className));

        if (!studentQuery.exec()) {
            QMessageBox::critical(this, "Query Error", "Failed to fetch data from " + className);
            return;
        }

        int row = 0;
        while (studentQuery.next()) {
            QString studentName = studentQuery.value(0).toString();

            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(studentName));

            QCheckBox *checkBox = new QCheckBox();
            QWidget *checkBoxWidget = new QWidget();
            QHBoxLayout *hLayout = new QHBoxLayout(checkBoxWidget);
            hLayout->addWidget(checkBox);
            hLayout->setAlignment(Qt::AlignCenter);
            hLayout->setContentsMargins(0, 0, 0, 0);
            checkBoxWidget->setLayout(hLayout);
            tableWidget->setCellWidget(row, 1, checkBoxWidget);

            QPushButton *resetButton = new QPushButton("Reset");
            connect(resetButton, &QPushButton::clicked, this, [=]() {
                on_reset_button_clicked(checkBox, studentName, className);
            });

            QWidget *resetBoxWidget = new QWidget();
            QHBoxLayout *hLayout1 = new QHBoxLayout(resetBoxWidget);
            hLayout1->addWidget(resetButton);
            hLayout1->setAlignment(Qt::AlignCenter);
            hLayout1->setContentsMargins(0, 0, 0, 0);
            resetBoxWidget->setLayout(hLayout1);
            tableWidget->setCellWidget(row, 2, resetBoxWidget);

            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                onCheckBoxClicked(static_cast<Qt::CheckState>(state), studentName, className);
            });

            row++;
        }

        tableWidget->resizeColumnsToContents();
        tableWidget->resizeRowsToContents();
        scrollLayout->addWidget(tableWidget);
    });

    if (comboBox->count() > 0) {
        comboBox->setCurrentIndex(0);
        emit comboBox->currentTextChanged(comboBox->currentText());
    }

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

void attendance::on_reset_button_clicked(QCheckBox *checkBox, const QString &name, const QString &tableName)
{
    if (!checkBox) return; // Exit if checkbox is null

    checkBox->setChecked(false); // Uncheck the checkbox

    QSqlQuery query;
    QString updateQuery = QString("UPDATE %1 SET count = count - 1 WHERE Name = :name AND count > 0").arg(tableName);

    query.prepare(updateQuery);
    query.bindValue(":name", name);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to update count: " + query.lastError().text());
        qDebug() << "SQL Error: " << query.lastError().text();
        qDebug() << "Query: " << updateQuery;
    } else {
        qDebug() << "Count successfully updated for" << name << "in table" << tableName;
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
    QString createQuery = QString(
                              "CREATE TABLE IF NOT EXISTS %1 ("
                              "Regno INTEGER PRIMARY KEY, "
                              "Rollno INTEGER, "
                              "Name TEXT, "
                              "count INTEGER DEFAULT 0)").arg(className);

    if (!createTableQuery.exec(createQuery)) {
        qDebug() << "Table creation failed:" << createTableQuery.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to create table.");
        file.close();
        return;
    }

    // Step 5: Read and Insert Data from CSV
    QSqlQuery insertQuery;
    int rowCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");

        // Ensure there are at least 3 values (Regno, Rollno, Name)
        if (values.size() < 3) {
            qDebug() << "Invalid row format, skipping:" << line;
            continue;
        }

        int regno = values[0].trimmed().toInt();
        int rollNo = values[1].trimmed().toInt();
        QString name = values[2].trimmed();

        if (name.isEmpty()) {
            qDebug() << "Skipping row due to empty name:" << line;
            continue;
        }

        // Insert into database
        insertQuery.prepare(QString("INSERT INTO %1 (Regno, Rollno, Name, count) VALUES (:regno, :rollno, :name, :count)").arg(className));
        insertQuery.bindValue(":regno", regno);
        insertQuery.bindValue(":rollno", rollNo);
        insertQuery.bindValue(":name", name);
        insertQuery.bindValue(":count", 0); // Explicitly inserting default count

        if (!insertQuery.exec()) {
            qDebug() << "Insert failed for" << name << ":" << insertQuery.lastError().text();
        } else {
            rowCount++;
        }
    }

    file.close();

    if (rowCount > 0) {
        QMessageBox::information(this, "Success", QString("CSV data uploaded successfully for class: %1\nTotal Records: %2").arg(className).arg(rowCount));
    } else {
        QMessageBox::warning(this, "Upload Warning", "No valid records were inserted.");
    }
}


void attendance::on_pushButton_2_clicked()
{
    bool ok;
    QString className = QInputDialog::getText(this, "Enter Class Name", "Class Name:", QLineEdit::Normal, "", &ok);

    if (!ok || className.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Class name cannot be empty.");
        return;
    }

    // Connect to both databases
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE", "admin_connection");
    db1.setDatabaseName("C:/Users/karki/project/database/admin.db");

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "student_connection");
    db2.setDatabaseName("C:/Users/karki/project/database/student.db");

    if (!db1.open() || !db2.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open databases.");
        return;
    }

    // Validate if className exists in student.db
    QSqlQuery checkClassQuery(db2);
    checkClassQuery.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name = :className");
    checkClassQuery.bindValue(":className", className);

    if (!checkClassQuery.exec() || !checkClassQuery.next()) {
        QMessageBox::warning(this, "Class Not Found", "The class name does not exist in the student database.");
        return;
    }

    // Ask for the total number of classes held
    int totalClasses = QInputDialog::getInt(this, "Total Classes", "Enter the total number of classes:", 1, 1, 1000, 1);

    // Create a new table named className + "info" in student.db
    QString infoTableName = className + "info";
    QSqlQuery createTableQuery(db2);
    QString createTableSQL = QString(
                                 "CREATE TABLE IF NOT EXISTS %1 ("
                                 "roll_number INTEGER PRIMARY KEY, "
                                 "reg_number TEXT UNIQUE, "
                                 "attendance_count INTEGER, "
                                 "attendance_percentage REAL, "
                                 "marks INTEGER"
                                 ")"
                                 ).arg(infoTableName);

    if (!createTableQuery.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to create table: " + createTableQuery.lastError().text());
        return;
    }

    // Fetch roll_number and attendance count from admin.db
    QSqlQuery adminQuery(db1);
    adminQuery.prepare(QString("SELECT Rollno, count FROM %1").arg(className));

    if (!adminQuery.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to fetch roll numbers and counts from admin database.");
        return;
    }

    // Open CSV file for writing
    QString filePath = QFileDialog::getSaveFileName(this, "Save CSV File", className + ".csv", "CSV Files (*.csv);;All Files (*.*)");
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Download Cancelled", "No file selected.");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Failed to create file.");
        return;
    }

    QTextStream out(&file);
    out << "Roll Number,Registration Number,Attendance Count,Attendance Percentage,Marks\n";

    // Process each student
    while (adminQuery.next()) {
        int rollNumber = adminQuery.value(0).toInt();
        int attendanceCount = adminQuery.value(1).toInt();

        // Calculate attendance percentage
        double attendancePercentage = (static_cast<double>(attendanceCount) / totalClasses) * 100.0;

        // Fetch reg_number and marks from student.db
        QSqlQuery studentQuery(db2);
        studentQuery.prepare(QString("SELECT reg_number, marks FROM %1 WHERE roll_number = :rollNumber").arg(className));
        studentQuery.bindValue(":rollNumber", rollNumber);

        QString regNumber;
        int marks = 0;
        if (studentQuery.exec() && studentQuery.next()) {
            regNumber = studentQuery.value(0).toString();
            marks = studentQuery.value(1).toInt();
        } else {
            QMessageBox::warning(this, "Warning", "No matching reg_number and marks found for roll number " + QString::number(rollNumber));
            continue;
        }

        // Insert or update data into className + "info" table
        QSqlQuery insertQuery(db2);
        insertQuery.prepare(QString("INSERT OR REPLACE INTO %1 (roll_number, reg_number, attendance_count, attendance_percentage, marks) VALUES (:rollNumber, :regNumber, :attendanceCount, :attendancePercentage, :marks)")
                                .arg(infoTableName));
        insertQuery.bindValue(":rollNumber", rollNumber);
        insertQuery.bindValue(":regNumber", regNumber);
        insertQuery.bindValue(":attendanceCount", attendanceCount);
        insertQuery.bindValue(":attendancePercentage", attendancePercentage);
        insertQuery.bindValue(":marks", marks);

        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Database Error", "Failed to insert data: " + insertQuery.lastError().text());
            return;
        }

        // Write to CSV
        out << rollNumber << "," << regNumber << "," << attendanceCount << "," << attendancePercentage << "," << marks << "\n";
    }

    file.close();
    db1.close();
    db2.close();

    QMessageBox::information(this, "Download Complete", "CSV file and database records updated successfully.");
}



void attendance::on_pushButton_4_clicked()
{
    this->close();
}


void attendance::on_pushButton_5_clicked()
{
    Form *formWindow = new Form();
    formWindow->setAttribute(Qt::WA_DeleteOnClose);
    formWindow->show();
}



void attendance::on_pushButton_6_clicked()
{
    // Ensure groupBox_2 has a layout
    if (!ui->groupBox_2->layout()) {
        ui->groupBox_2->setLayout(new QVBoxLayout());
    }

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->groupBox_2->layout());

    // Clear previous widgets inside groupBox_2
    while (QLayoutItem *child = layout->takeAt(0)) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // Create new GroupBox
    QGroupBox *newGroupBox = new QGroupBox(this);
    newGroupBox->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; border: 1px solid gray; border-radius: 10px;");

    // Create Layout for GroupBox
    QGridLayout *groupBoxLayout = new QGridLayout(newGroupBox);

    // Add QLabel for "Upload Internal Marks"
    QLabel *titleLabel = new QLabel("Upload Internal Marks", newGroupBox);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
    titleLabel->setAlignment(Qt::AlignCenter);
    groupBoxLayout->addWidget(titleLabel, 0, 0, 1, 2, Qt::AlignCenter); // Span across 2 columns

    // Create Widgets
    QLabel *label1 = new QLabel("Class Name:", newGroupBox);
    label1->setStyleSheet("font-size: 14px;");
    QLineEdit *line1 = new QLineEdit(newGroupBox);
    line1->setPlaceholderText("Type here...");
    line1->setStyleSheet("font-size: 14px; padding: 5px; border: 1px solid gray; border-radius: 5px;");

    QLabel *label2 = new QLabel("Registration Number:", newGroupBox);
    label2->setStyleSheet("font-size: 14px;");
    QLineEdit *line2 = new QLineEdit(newGroupBox);
    line2->setPlaceholderText("Type here...");
    line2->setStyleSheet("font-size: 14px; padding: 5px; border: 1px solid gray; border-radius: 5px;");

    QLabel *label3 = new QLabel("Marks:", newGroupBox);
    label3->setStyleSheet("font-size: 14px;");
    QLineEdit *line3 = new QLineEdit(newGroupBox);
    line3->setPlaceholderText("Type here...");
    line3->setStyleSheet("font-size: 14px; padding: 5px; border: 1px solid gray; border-radius: 5px;");

    QPushButton *submitButton = new QPushButton("Submit", newGroupBox);
    submitButton->setStyleSheet("background-color: #007BFF; color: white; font-size: 14px; padding: 5px 10px; border-radius: 5px;");

    // Add Widgets to Layout with Proper Positioning
    groupBoxLayout->addWidget(label1, 1, 0);
    groupBoxLayout->addWidget(line1, 1, 1);

    groupBoxLayout->addWidget(label2, 2, 0);
    groupBoxLayout->addWidget(line2, 2, 1);

    groupBoxLayout->addWidget(label3, 3, 0);
    groupBoxLayout->addWidget(line3, 3, 1);

    groupBoxLayout->addWidget(submitButton, 4, 0, 1, 2, Qt::AlignCenter); // Spanning 2 columns, centered

    // Set layout for GroupBox
    newGroupBox->setLayout(groupBoxLayout);

    // Add the GroupBox to the main layout inside groupBox_2
    layout->addWidget(newGroupBox);

    // Connect button to action
    connect(submitButton, &QPushButton::clicked, this, [=]() {
        QString className = line1->text().trimmed();
        QString regNumber = line2->text().trimmed();
        QString marks = line3->text().trimmed();

        if (className.isEmpty() || regNumber.isEmpty() || marks.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please fill in all fields before submitting.");
            return;
        }

        // Validate marks (ensure it's a number)
        bool ok;
        int marksValue = marks.toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Invalid Input", "Marks should be a valid number.");
            return;
        }

        // Initialize Database Connection
        QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "student_connection");
        QString dbPath1 = "C:/Users/karki/project/database/student.db";
        db2.setDatabaseName(dbPath1);

        if (!db2.open()) {
            QMessageBox::critical(this, "Database Error", "Failed to open database: " + db2.lastError().text());
            return;
        }

        QSqlQuery createTableQuery(db2);
        QString createTableSQL = QString(
                                     "CREATE TABLE IF NOT EXISTS %1 ("

                                     "reg_number TEXT UNIQUE, "
                                     "marks INTEGER"
                                     ");"
                                     ).arg(className);

        if (!createTableQuery.exec(createTableSQL)) {
            QMessageBox::critical(this, "Database Error", "Failed to create table: " + createTableQuery.lastError().text());
            db2.close();
            return;
        }

        // Insert Data into the Table
        QSqlQuery insertQuery(db2);
        insertQuery.prepare(QString("INSERT INTO %1 (reg_number, marks) VALUES (:reg_number, :marks)").arg(className));
        insertQuery.bindValue(":reg_number", regNumber);
        insertQuery.bindValue(":marks", marksValue);

        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Database Error", "Failed to insert data: " + insertQuery.lastError().text());
            db2.close();
            return;
        }

        QMessageBox::information(this, "Data Submitted", "Your data has been successfully submitted.");


        // Close the database connection
        db2.close();
    });
}

