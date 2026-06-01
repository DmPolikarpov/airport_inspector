#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "statisticsdialog.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    // create database manager and connect to database
    dbManager = new DatabaseManager(this);
    connect(dbManager, &DatabaseManager::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    dbManager->connectToDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("Инспектор аэропортов");
    resize(900, 600);

    // main title
    ui->lblMain->setAlignment(Qt::AlignCenter);
    ui->lblMain->setObjectName("mainLabel");

    // Left Panel: Table
    ui->tableSchedule->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSchedule->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Right Panel: Controls
    ui->rbDeparture->setChecked(true); // Default to departures
    ui->deDate->setCalendarPopup(true);
    // Strict date bounds to prevent user error
    ui->deDate->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->deDate->setDate(QDate(2016, 9, 1));

    // Status Bar
    lblStatus = new QLabel("Отключено", this);
    lblStatus->setStyleSheet("color: #EF4444; font-weight: bold; border: none; padding-left: 5px;");
    statusBar()->addWidget(lblStatus);
    // disable right panel till database is not connected
    setUIEnabled(false);
}

// enable or disable right panel
// if database is not connected the panel is disabled
// otherwise, it is enabled
void MainWindow::setUIEnabled(bool state) {
    ui->cbAirports->setEnabled(state);
    ui->rbArrival->setEnabled(state);
    ui->rbDeparture->setEnabled(state);
    ui->deDate->setEnabled(state);
    ui->btnShowSchedule->setEnabled(state);
    ui->btnShowStats->setEnabled(state);
}

void MainWindow::onConnectionStatusChanged(bool connected, const QString &errorMsg) {
    if (connected) {
        lblStatus->setText("Подключено");
        lblStatus->setStyleSheet("color: green; font-weight: bold;");

        // get all airports
        QSqlQuery query = dbManager->getAirports();
        ui->cbAirports->clear();
        airportCodes.clear();
        while (query.next()) {
            QString name = query.value(0).toString();
            QString code = query.value(1).toString();
            airportCodes.insert(name, code);
            ui->cbAirports->addItem(name);
        }
        setUIEnabled(true);
    } else {
        lblStatus->setText("Отключено");
        lblStatus->setStyleSheet("color: red; font-weight: bold;");
        setUIEnabled(false);
        QMessageBox::critical(this, "Ошибка БД", "Не удалось подключиться:\n" + errorMsg + "\nПовторная попытка через 5 секунд...");
    }
}

void MainWindow::on_btnShowSchedule_clicked()
{
    QString code = airportCodes[ui->cbAirports->currentText()];
    bool isArrival = ui->rbArrival->isChecked();
    QDate date = ui->deDate->date();

    QSqlQuery query = dbManager->getFlights(code, isArrival, date);

    ui->tableSchedule->clear();
    ui->tableSchedule->setRowCount(0);
    ui->tableSchedule->setColumnCount(3);

    if (isArrival) {
        ui->tableSchedule->setHorizontalHeaderLabels({"Номер рейса", "Время прилета", "Аэропорт отправления"});
    } else {
        ui->tableSchedule->setHorizontalHeaderLabels({"Номер рейса", "Время вылета", "Аэропорт назначения"});
    }

    int row = 0;
    while (query.next()) {
        ui->tableSchedule->insertRow(row);
        ui->tableSchedule->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableSchedule->setItem(row, 1, new QTableWidgetItem(query.value(1).toDateTime().toString("HH:mm")));
        ui->tableSchedule->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        row++;
    }
}

void MainWindow::on_btnShowStats_clicked()
{
    QString code = airportCodes[ui->cbAirports->currentText()];
    QString name = ui->cbAirports->currentText();

    StatisticsDialog dialog(code, name, dbManager, this);
    dialog.exec();
}
