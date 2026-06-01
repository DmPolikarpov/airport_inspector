#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QDate>
#include <QDebug>

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
    // set months
    ui->cbMonths->clear();
    ui->cbMonths->addItems({
        "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
        "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"
    });
    ui->cbMonths->setCurrentIndex(0);
    // main label
    ui->lblTitle->setText(QString("Загруженность аэропорта: %1").arg(name));
    // set tabs content
    setupYearlyTab(code);
    setupMonthlyTab(code);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setupYearlyTab(const QString &code) {
    QBarSet *set = new QBarSet("Всего рейсов");
    QStringList categories;

    // 1. Fetch data from the database
    QSqlQuery query = dbManager->getYearlyStats(code);

    int maxValue = 0; // For adjusting the Y axis gracefully

    while (query.next()) {
        int count = query.value(0).toInt();
        QDate date = query.value(1).toDate();

        *set << count;

        // Convert date
        categories << date.toString("MMM yy");

        if (count > maxValue) maxValue = count;
    }

    if (categories.isEmpty()) {
        qDebug() << "Нет данных для графика за год по аэропорту:" << code;
        *set << 0;
        categories << "Нет данных";
        maxValue = 10;
    }

    set->setColor(QColor("#E07A1F"));

    // 2. Add data to the series
    QBarSeries *series = new QBarSeries();
    series->append(set);

    // 3. Create the chart container
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Суммарная загруженность за год (Прилеты + Вылеты)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    applyDarkThemeToChart(chart);

    // 4. Setup Axes
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#CFCFCF"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(QColor("#CFCFCF"));
    axisY->setRange(0, maxValue + (maxValue * 0.1));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 5. Inject chart into QChartView
    ui->yearlyChartView->setChart(chart);
    ui->yearlyChartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsDialog::setupMonthlyTab(const QString &code) {
    // 1. Pre-fill the cache with 0 to prevent empty days
    for (int month = 1; month <= 12; ++month) {
        for (int day = 1; day <= 31; ++day) {
            dailyStatsCache[month][day] = 0;
        }
    }

    // 2. Fetch the whole daily stats from DB
    QSqlQuery query = dbManager->getDailyStats(code);
    while (query.next()) {
        int count = query.value(0).toInt();
        QDate date = query.value(1).toDate();

        if (date.isValid() && date.month() >= 1 && date.month() <= 12 && date.day() >= 1 && date.day() <= 31) {
            dailyStatsCache[date.month()][date.day()] = count;
        }
    }

    int currentMonthIndex = ui->cbMonths->currentIndex();
    if (currentMonthIndex < 0) currentMonthIndex = 0; // Защита, если список пуст

    updateMonthlyChart(currentMonthIndex);
}

void StatisticsDialog::updateMonthlyChart(int monthIndex) {
    if (monthIndex < 0) return;

    int targetMonth = monthIndex + 1;

    QChart *chart = new QChart();
    applyDarkThemeToChart(chart);

    QLineSeries *series = new QLineSeries();
    series->setName("Ежедневная активность");

    QPen pen(QColor("#3B82F6"));
    pen.setWidth(3);
    series->setPen(pen);

    int maxFlightsInDay = 0;

    QDate tempDate(2016, targetMonth, 1);
    int daysInMonth = tempDate.daysInMonth();

    for (int day = 1; day <= daysInMonth; ++day) {
        int count = dailyStatsCache[targetMonth][day];
        series->append(day, count);
        if (count > maxFlightsInDay) maxFlightsInDay = count;
    }

    chart->addSeries(series);

    QString monthName = ui->cbMonths->itemText(monthIndex);
    chart->setTitle(QString("Ежедневная загруженность за %1").arg(monthName));

    chart->setAnimationOptions(QChart::SeriesAnimations);

    // axis X
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, daysInMonth);
    axisX->setLabelFormat("%d");

    axisX->setTickCount(7);
    axisX->setLabelsColor(QColor("#CFCFCF"));
    axisX->setGridLineColor(QColor("#333333"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // axis Y
    QValueAxis *axisY = new QValueAxis();

    int yMax = maxFlightsInDay > 0 ? maxFlightsInDay + 5 : 10;
    axisY->setRange(0, yMax);
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#CFCFCF"));
    axisY->setGridLineColor(QColor("#333333"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->monthlyChartView->setChart(chart);
    ui->monthlyChartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsDialog::applyDarkThemeToChart(QChart *chart) {
    chart->setTheme(QChart::ChartThemeDark);
    chart->setBackgroundBrush(QBrush(QColor("#282828")));
    chart->setTitleBrush(QBrush(QColor("#E07A1F")));
    chart->legend()->hide();
}

void StatisticsDialog::on_cbMonths_currentIndexChanged(int index)
{
    updateMonthlyChart(index);
}

void StatisticsDialog::on_btnClose_clicked()
{
    close();
}
