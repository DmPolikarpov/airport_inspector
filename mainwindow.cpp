#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
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
}

