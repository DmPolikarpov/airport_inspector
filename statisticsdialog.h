#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>
#include "databasemanager.h"

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

private slots:
    void updateMonthlyChart(int monthIndex);

    void on_cbMonths_currentIndexChanged(int index);

    void on_btnClose_clicked();

private:
    Ui::StatisticsDialog *ui;

    void setupYearlyTab(const QString &code);
    void setupMonthlyTab(const QString &code);

    void applyDarkThemeToChart(QChart *chart);

    DatabaseManager *dbManager;

    // Cache daily data to avoid DB spam when changing combobox
    int dailyStatsCache[13][32];
};

#endif // STATISTICSDIALOG_H
