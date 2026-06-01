#include "databasemanager.h"
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QPSQL");
    // set connection parameters
    db.setHostName("981757-ca08998.tmweb.ru");
    db.setPort(5432);
    db.setDatabaseName("demo");
    db.setUserName("netology_usr_cpp");
    db.setPassword("CppNeto3");

    retryTimer = new QTimer(this);
    retryTimer->setSingleShot(true);
    connect(retryTimer, &QTimer::timeout, this, &DatabaseManager::retryConnection);
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) db.close();
}

/**** DATABASE CONNECTION ******/
void DatabaseManager::connectToDatabase() {
    if (db.open()) {
        retryTimer->stop();
        emit connectionStatusChanged(true);
    } else {
        emit connectionStatusChanged(false, db.lastError().text());
        // Retry connection after 5 seconds if failed
        retryTimer->start(5000);
    }
}

bool DatabaseManager::isConnected() const { return db.isOpen(); }

void DatabaseManager::retryConnection() { connectToDatabase(); }

/**** QUERY API ******/
QSqlQuery DatabaseManager::getAirports() {
    QSqlQuery query(db);
    query.exec("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data ORDER BY \"airportName\"");
    return query;
}

QSqlQuery DatabaseManager::getFlights(const QString &airportCode, bool isArrival, const QDate &date) {
    QSqlQuery query(db);
    QString qStr;

    // Switch between arrival and departure queries
    if (isArrival) {
        qStr = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" "
               "FROM bookings.flights f "
               "JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport "
               "WHERE f.arrival_airport = :airportCode AND f.scheduled_arrival::date = :date";
    } else {
        qStr = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" "
               "FROM bookings.flights f "
               "JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport "
               "WHERE f.departure_airport = :airportCode AND f.scheduled_departure::date = :date";
    }

    query.prepare(qStr);
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getYearlyStats(const QString &airportCode) {
    QSqlQuery query(db);
    query.prepare("SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" "
                  "FROM bookings.flights f "
                  "WHERE (scheduled_departure::date > date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                  "AND (departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  "GROUP BY \"Month\" ORDER BY \"Month\"");
    query.bindValue(":airportCode", airportCode);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getDailyStats(const QString &airportCode) {
    QSqlQuery query(db);
    query.prepare("SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" "
                  "FROM bookings.flights f "
                  "WHERE (scheduled_departure::date > date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                  "AND (departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  "GROUP BY \"Day\" ORDER BY \"Day\"");
    query.bindValue(":airportCode", airportCode);
    query.exec();
    return query;
}
