#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include "DatabaseManager.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    StatisticsDialog(const QString &airportCode, const QString &airportName, DatabaseManager *db, QWidget *parent = nullptr);
    ~StatisticsDialog();

private:
    Ui::StatisticsDialog *ui;

    void setupYearlyTab();
    void setupMonthlyTab();

    DatabaseManager *dbManager;

    // Cache daily data to avoid DB spam when changing combobox
    QMap<int, QVector<int>> dailyStatsCache;
};

#endif // STATISTICSDIALOG_H
