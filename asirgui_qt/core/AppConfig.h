#pragma once

#include <QColor>
#include <QFont>
#include <QList>
#include <QString>

struct AppConfig {
    QString filePath;
    QString openXmHome;
    QString asirLibDir;
    QString asirContribDir;
    QColor plotForegroundColor = QColor(17, 24, 39);
    QColor plotBackgroundColor = QColor(248, 250, 252);
    int plotLineThickness = 1;
    double plotZoomFactor = 1.0;
    bool plotFitToWidth = true;
    bool plotShowAxes = true;
    bool plotShowAxisLabels = true;
    QFont inputFont;
    QFont outputFont;
    QList<int> displaySplitterSizes;

    static QString portableConfigPath();
    static QString userConfigPath();
    static QString resolveConfigPath();
    static AppConfig load(const QString &defaultOpenXmHome);

    void save() const;
};
