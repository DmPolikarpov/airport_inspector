#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(const QString &code, const QString &name, DatabaseManager *db, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::StatisticsDialog),
    dbManager(db)
{
    ui->setupUi(this);
    // common ui settings
    setWindowTitle("Статистика загруженности");
    resize(800, 600);
    setModal(true);
    // main label
    ui->lblTitle->setText(QString("Загруженность аэропорта: %1").arg(name));
    // set tabs content
    setupYearlyTab();
    setupMonthlyTab();
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setupYearlyTab() {

}

void StatisticsDialog::setupMonthlyTab() {

}
