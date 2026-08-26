#include "src/ui/mainwindow.h"
#include "src/database/DatabaseManager.h"

#include <QApplication>
#include <QDebug>
#include <QDate>
#include <QCalendar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QDate today = QDate::currentDate();

    QCalendar jalali(QCalendar::System::Jalali);

    QCalendar::YearMonthDay parts =
        jalali.partsFromDate(today);

    qDebug() << "Gregorian:" << today;
    qDebug() << "Jalali:"
             << parts.year
             << parts.month
             << parts.day;

    return QApplication::exec();
}
