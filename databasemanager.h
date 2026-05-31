#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDate>

// Class to interact with database
class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    void connectToDatabase();
    bool isConnected() const;

    // Queries
    QSqlQuery getAirports();
    QSqlQuery getFlights(const QString &airportCode, bool isArrival, const QDate &date);
    QSqlQuery getYearlyStats(const QString &airportCode);
    QSqlQuery getDailyStats(const QString &airportCode);

signals:
    void connectionStatusChanged(bool connected, const QString &errorMessage = "");

private slots:
    void retryConnection();

private:
    QSqlDatabase db;
    QTimer *retryTimer;
};

#endif // DATABASEMANAGER_H
