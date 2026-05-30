#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile styleFile(":/style.qss");

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString qss = QLatin1String(styleFile.readAll());
        a.setStyleSheet(qss);
        styleFile.close();
    } else {
        qDebug() << "cannot apply stylesheets!";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
