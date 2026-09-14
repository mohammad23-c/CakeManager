#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>

class QApplication;

class ThemeManager
{
public:
    static const QString DARK_THEME;
    static const QString LIGHT_THEME;

    static bool applyTheme(
        QApplication& app,
        const QString& themePath
        );
};

#endif // THEMEMANAGER_H
