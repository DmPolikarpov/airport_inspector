#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMap>

#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void setUIEnabled(bool state);
    void setupUI();

    QLabel *lblStatus;
    DatabaseManager *dbManager;
    QMap<QString, QString> airportCodes;

private slots:
    void onConnectionStatusChanged(bool connected, const QString &errorMsg);

    void on_btnShowSchedule_clicked();
};
#endif // MAINWINDOW_H
