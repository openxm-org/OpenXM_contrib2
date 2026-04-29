#include "core/LocalAsirEngine.h"

#include <QCoreApplication>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QTimer>

#include <cassert>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const QString openXmHome = QFileInfo(QStringLiteral(ASIRGUI_DEFAULT_OPENXM_HOME)).absoluteFilePath();
    AppConfig config;
    config.openXmHome = openXmHome;
    config.asirLibDir = QDir(openXmHome).filePath(QStringLiteral("lib/asir"));
    config.asirContribDir = QDir(openXmHome).filePath(QStringLiteral("lib/asir-contrib"));
    LocalAsirEngine engine(config);
    assert(engine.initialize());

    QEventLoop loop;
    AsirResult lastResult;

    QObject::connect(&engine, &LocalAsirEngine::resultReady, &loop,
        [&](const AsirResult &result) {
            lastResult = result;
            loop.quit();
        });
    QObject::connect(&engine, &LocalAsirEngine::errorOccurred, &loop,
        [&](const QString &message) {
            lastResult = AsirResult();
            lastResult.text = message;
            loop.quit();
        });

    auto executeAndWait = [&](const QString &command, bool producePresentation = true) {
        lastResult = AsirResult();
        engine.executeString(command, producePresentation);
        QTimer::singleShot(30000, &loop, &QEventLoop::quit);
        loop.exec();
    };

    executeAndWait(QStringLiteral("fctr(x^4-1);"));
    assert(lastResult.text.contains("x-1"));
    assert(!lastResult.latex.isEmpty());
    assert(lastResult.plotImage.isNull());
    assert(lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("fctr(x^4-1);"), false);
    assert(lastResult.text.contains("x-1"));
    assert(lastResult.latex.isEmpty());
    assert(lastResult.mathPlainText.isEmpty());
    assert(lastResult.plotImage.isNull());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("cputime(1);"));
    assert(lastResult.text == QStringLiteral("0"));
    assert(!lastResult.timingText.isEmpty());
    assert(lastResult.timingText.contains("sec"));
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("pari(factor,2^2^5+1);"));
    assert(!lastResult.text.isEmpty());
    assert(lastResult.text.contains("641"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.mathPlainText.isEmpty());
    assert(lastResult.mathPlainText.contains("641"));
    assert(lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("pari(factor,2^2^8+1);"));
    assert(!lastResult.text.isEmpty());
    assert(lastResult.text.contains("1238926361552897"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.mathPlainText.isEmpty());
    assert(lastResult.mathPlainText.contains("1238926361552897"));
    assert(lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("polarplot(1+cos(t),[t,0,6.283185307179586],[120,120]);"));
    assert(lastResult.text.contains("polarplot"));
    assert(lastResult.text.contains("120x120"));
    assert(lastResult.text.contains("0 <= t <= 6.283185307179586"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(lastResult.plotImage.width() == 120);
    assert(lastResult.plotImage.height() == 120);
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("ifplot(x^3-y^2);"));
    assert(lastResult.text.contains("ifplot"));
    assert(lastResult.text.contains("300x300"));
    assert(lastResult.text.contains("-2 <= x <= 2"));
    assert(lastResult.text.contains("-2 <= y <= 2"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("ifplot(x^2 + y^2 - 1, [300,300], [x,-2,2], [y,-2,2]);"));
    assert(lastResult.text.contains("ifplot"));
    assert(lastResult.text.contains("300x300"));
    assert(lastResult.text.contains("-2 <= x <= 2"));
    assert(lastResult.text.contains("-2 <= y <= 2"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("plot(x^2-x^3,[400,400],[x,-4,4]);"));
    assert(lastResult.text.contains("plot"));
    assert(lastResult.text.contains("400x400"));
    assert(lastResult.text.contains("-4 <= x <= 4"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("polarplot(1+cos(t),[240,120],[t,0,6.283185307179586]);"));
    assert(lastResult.text.contains("polarplot"));
    assert(lastResult.text.contains("240x120"));
    assert(lastResult.text.contains("0 <= t <= 6.283185307179586"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.plotImage.isNull());
    assert(!lastResult.timingText.isEmpty());
    assert(lastResult.plotImage.width() == 240);
    assert(lastResult.plotImage.height() == 120);
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("1+1;"));
    assert(lastResult.text == QStringLiteral("2"));
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("fctr(^3-1);"));
    assert(lastResult.text == QStringLiteral("syntax error\n0"));
    assert(lastResult.latex.isEmpty());
    assert(lastResult.mathPlainText.isEmpty());
    assert(lastResult.plotImage.isNull());
    assert(lastResult.isError);

    executeAndWait(QStringLiteral("plot(^2-1,[x,-2,2]);"));
    assert(lastResult.text == QStringLiteral("syntax error\n0"));
    assert(lastResult.plotImage.isNull());
    assert(lastResult.isError);

    executeAndWait(QStringLiteral("S = ox_launch_nox(0);"));
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("ox_execute_string(S, \"fctr(x^3-1);;\");"));
    assert(lastResult.text == QStringLiteral("0"));
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("ox_pop_local(S);"));
    assert(lastResult.text.contains("x-1"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("ox_shutdown(S);"));
    assert(lastResult.text == QStringLiteral("0"));
    assert(lastResult.latex.isEmpty());
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("1+1;"));
    assert(lastResult.text == QStringLiteral("2"));
    assert(!lastResult.isError);

    executeAndWait(QStringLiteral("cputime(0);"));
    assert(lastResult.text == QStringLiteral("0"));
    assert(lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    {
        lastResult = AsirResult();
        QTimer interruptTimer;
        interruptTimer.setSingleShot(true);
        QObject::connect(&interruptTimer, &QTimer::timeout, &engine, &LocalAsirEngine::interruptCurrent);
        engine.executeString(QStringLiteral("pari(factor,2^2^9+1);"));
        interruptTimer.start(100);
        QTimer::singleShot(30000, &loop, &QEventLoop::quit);
        loop.exec();
        interruptTimer.stop();
        assert(lastResult.text.contains("return to toplevel"));
        assert(lastResult.isError);
    }

    executeAndWait(QStringLiteral("1+1;"));
    assert(lastResult.text == QStringLiteral("2"));
    assert(lastResult.timingText.isEmpty());
    assert(!lastResult.isError);

    return 0;
}
