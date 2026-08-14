#include "src/ui/mainwindow.h"
#include "src/database/DatabaseManager.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // create data base
    DatabaseManager database("dataBase.db");
    // open database
    if (!database.openDatabase())
    {
        qDebug() << "Failed to open database.";
        return -1;
    }
    //create tables
    if (!database.createTables())
    {
        qDebug() << "Failed to create database tables.";
        return -1;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
