#include "ThemeManager.h"

#include <QApplication>
#include <QFile>

const QString ThemeManager::DARK_THEME  = ":/styles/dark.qss";
const QString ThemeManager::LIGHT_THEME = ":/styles/light.qss";

bool ThemeManager::applyTheme(
    QApplication& app,
    const QString& themePath
    )
{
    QFile file(themePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open theme:" << themePath;
        return false;
    }

    QString styleSheet =
        QString::fromUtf8(file.readAll());

    app.setStyleSheet(styleSheet);

    return true;
}
