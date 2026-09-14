#include "src/ui/mainwindow.h"
#include "src/database/DatabaseManager.h"
#include "src/managers/appmanager.h"
#include <QApplication>
#include <QDebug>
#include <QDate>
#include <QCalendar>
#include "src/managers/ThemeManager.h"

bool resetDatabase(QString databasePath)
{
    return QFile::remove(databasePath);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("CakeManager");
    QCoreApplication::setApplicationName("CakeManager");

    // QString dbPath = getDatabasePath();

    // qDebug() << "MAIN DATABASE PATH:" << dbPath;

    // resetDatabase(dbPath);

    ThemeManager::applyTheme(
        a,
        ThemeManager::DARK_THEME
        );

    MainWindow w;
    w.show();

    return QApplication::exec();
}
