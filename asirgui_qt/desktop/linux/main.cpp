#include "core/AppConfig.h"
#include "core/LocalAsirEngine.h"
#include "desktop/common/MainWindow.h"

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QTimer>

namespace {

void configureWebEngineForLinux()
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QByteArray flags = qgetenv("QTWEBENGINE_CHROMIUM_FLAGS").trimmed();
    const QList<QByteArray> requiredFlags = {
        QByteArrayLiteral("--disable-gpu"),
        QByteArrayLiteral("--disable-gpu-compositing")
    };

    for (const QByteArray &flag : requiredFlags) {
        if (!flags.contains(flag)) {
            if (!flags.isEmpty()) {
                flags.append(' ');
            }
            flags.append(flag);
        }
    }

    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", flags);
}

} // namespace

int main(int argc, char *argv[])
{
    configureWebEngineForLinux();

    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("OpenXM"));
    app.setApplicationName(QStringLiteral("AsirGUI"));

    AppConfig config = AppConfig::load(QStringLiteral(ASIRGUI_DEFAULT_OPENXM_HOME));
    LocalAsirEngine engine(config);
    if (!engine.initialize()) {
        QMessageBox::critical(nullptr, QStringLiteral("AsirGUI"), QStringLiteral("Asir エンジンの初期化に失敗しました。"));
        return 1;
    }

    MainWindow window(&engine, &config);
    window.show();
    window.showNormal();
    window.raise();
    window.activateWindow();
    QTimer::singleShot(0, &window, [&window]() {
        window.showNormal();
        window.raise();
        window.activateWindow();
    });
    return app.exec();
}
