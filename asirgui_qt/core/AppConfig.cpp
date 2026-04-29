#include "core/AppConfig.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

namespace {

QString normalizedAbsolutePath(const QString &path)
{
    const QString trimmed = path.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }
    return QFileInfo(trimmed).absoluteFilePath();
}

QString defaultAsirLibDir(const QString &openXmHome)
{
    if (openXmHome.isEmpty()) {
        return {};
    }
    return QDir(openXmHome).filePath(QStringLiteral("lib/asir"));
}

QString defaultAsirContribDir(const QString &openXmHome)
{
    if (openXmHome.isEmpty()) {
        return {};
    }
    return QDir(openXmHome).filePath(QStringLiteral("lib/asir-contrib"));
}

} // namespace

QString AppConfig::portableConfigPath()
{
    return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral(".asirguirc"));
}

QString AppConfig::userConfigPath()
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return QDir(base).filePath(QStringLiteral("asirgui/config"));
}

QString AppConfig::resolveConfigPath()
{
    const QString portable = portableConfigPath();
    if (QFileInfo::exists(portable)) {
        return portable;
    }
    return userConfigPath();
}

AppConfig AppConfig::load(const QString &defaultOpenXmHome)
{
    AppConfig config;
    config.filePath = resolveConfigPath();
    config.openXmHome = normalizedAbsolutePath(defaultOpenXmHome);
    config.asirLibDir = defaultAsirLibDir(config.openXmHome);
    config.asirContribDir = defaultAsirContribDir(config.openXmHome);

    if (!QFileInfo::exists(config.filePath)) {
        return config;
    }

    QSettings settings(config.filePath, QSettings::IniFormat);

    const QString configuredHome = normalizedAbsolutePath(settings.value(QStringLiteral("env/OpenXM_HOME")).toString());
    if (!configuredHome.isEmpty()) {
        config.openXmHome = configuredHome;
    }

    config.asirLibDir = normalizedAbsolutePath(settings.value(QStringLiteral("env/ASIR_LIBDIR")).toString());
    if (config.asirLibDir.isEmpty()) {
        config.asirLibDir = defaultAsirLibDir(config.openXmHome);
    }

    config.asirContribDir = normalizedAbsolutePath(settings.value(QStringLiteral("env/ASIR_CONTRIB_DIR")).toString());
    if (config.asirContribDir.isEmpty()) {
        config.asirContribDir = defaultAsirContribDir(config.openXmHome);
    }

    const QColor foreground(settings.value(QStringLiteral("plot/foreground"), config.plotForegroundColor.name()).toString());
    if (foreground.isValid()) {
        config.plotForegroundColor = foreground;
    }

    const QColor background(settings.value(QStringLiteral("plot/background"), config.plotBackgroundColor.name()).toString());
    if (background.isValid()) {
        config.plotBackgroundColor = background;
    }

    config.plotLineThickness = qMax(1, settings.value(QStringLiteral("plot/lineThickness"), config.plotLineThickness).toInt());
    config.plotZoomFactor = qBound(0.25, settings.value(QStringLiteral("plot/zoomFactor"), config.plotZoomFactor).toDouble(), 8.0);
    config.plotFitToWidth = settings.value(QStringLiteral("plot/fitToWidth"), config.plotFitToWidth).toBool();
    config.plotShowAxes = settings.value(QStringLiteral("plot/showAxes"), config.plotShowAxes).toBool();
    config.plotShowAxisLabels = settings.value(QStringLiteral("plot/showAxisLabels"), config.plotShowAxisLabels).toBool();

    QFont inputFont;
    if (inputFont.fromString(settings.value(QStringLiteral("font/input")).toString())) {
        config.inputFont = inputFont;
    }

    QFont outputFont;
    if (outputFont.fromString(settings.value(QStringLiteral("font/output")).toString())) {
        config.outputFont = outputFont;
    }

    const QList<QVariant> splitterList = settings.value(QStringLiteral("ui/displaySplitterSizes")).toList();
    if (splitterList.size() == 2) {
        QList<int> sizes;
        sizes.reserve(2);
        for (const QVariant &value : splitterList) {
            sizes.append(qMax(0, value.toInt()));
        }
        if (sizes.at(0) > 0 && sizes.at(1) > 0) {
            config.displaySplitterSizes = sizes;
        }
    }
    return config;
}

void AppConfig::save() const
{
    const QString path = filePath.isEmpty() ? resolveConfigPath() : filePath;
    QFileInfo info(path);
    QDir().mkpath(info.dir().absolutePath());

    QSettings settings(path, QSettings::IniFormat);
    settings.setValue(QStringLiteral("env/OpenXM_HOME"), openXmHome);
    settings.setValue(QStringLiteral("env/ASIR_LIBDIR"), asirLibDir);
    settings.setValue(QStringLiteral("env/ASIR_CONTRIB_DIR"), asirContribDir);
    settings.setValue(QStringLiteral("plot/foreground"), plotForegroundColor.name());
    settings.setValue(QStringLiteral("plot/background"), plotBackgroundColor.name());
    settings.setValue(QStringLiteral("plot/lineThickness"), plotLineThickness);
    settings.setValue(QStringLiteral("plot/zoomFactor"), plotZoomFactor);
    settings.setValue(QStringLiteral("plot/fitToWidth"), plotFitToWidth);
    settings.setValue(QStringLiteral("plot/showAxes"), plotShowAxes);
    settings.setValue(QStringLiteral("plot/showAxisLabels"), plotShowAxisLabels);
    settings.setValue(QStringLiteral("font/input"), inputFont.toString());
    settings.setValue(QStringLiteral("font/output"), outputFont.toString());
    QVariantList splitterSizes;
    splitterSizes.reserve(displaySplitterSizes.size());
    for (const int size : displaySplitterSizes) {
        splitterSizes.append(size);
    }
    settings.setValue(QStringLiteral("ui/displaySplitterSizes"), splitterSizes);
    settings.sync();
}
