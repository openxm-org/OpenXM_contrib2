#pragma once

#include "core/AppConfig.h"
#include "core/AsirEngine.h"

#include <QImage>
#include <QList>
#include <QMutex>
#include <QPair>
#include <QString>
#include <QThread>

#include <atomic>

class AsirWorker : public QObject {
    Q_OBJECT

public:
    explicit AsirWorker(AppConfig config, QObject *parent = nullptr);

    void requestCancel() { m_cancelRequested.store(true, std::memory_order_relaxed); }
    void requestInterrupt()
    {
        m_cancelRequested.store(true, std::memory_order_relaxed);
        m_userInterruptRequested.store(true, std::memory_order_relaxed);
    }
    bool isExecuting() const { return m_executing.load(std::memory_order_relaxed); }
    bool isPariActive() const { return m_pariActive.load(std::memory_order_relaxed); }
    unsigned long nativeThreadId() const { return m_nativeThreadId.load(std::memory_order_relaxed); }

public slots:
    void init();
    void execute(const QString &command, bool producePresentation = true);
    void shutdownWorker();

signals:
    void initialized();
    void initFailed(const QString &message);
    void resultReady(const AsirResult &result);
    void errorOccurred(const QString &message);

private:
    struct StringExecutionResult {
        QString text;
        bool isError = false;
    };

    struct PlotCommand {
        QString functionName;
        QString command;
        QStringList arguments;
        bool matched = false;
    };

    struct PlotImageResult {
        QImage image;
        int width = 0;
        int height = 0;
        int byteCount = 0;
        bool valid = false;
    };

    struct PlotExecutionResult {
        QString text;
        QString summary;
        QImage image;
        bool plotAxesValid = false;
        double plotXMin = 0.0;
        double plotXMax = 0.0;
        double plotYMin = 0.0;
        double plotYMax = 0.0;
        bool valid = false;
        bool isError = false;
    };

    struct CmoExecutionResult {
        QByteArray bytes;
        bool isError = false;
        QString errorText;
    };

    StringExecutionResult executeStringResult(const QString &command);
    QString executeToString(const QString &command);
    CmoExecutionResult executeToCmo(const QString &command);
    QString buildLatexCommand(const QString &command) const;
    static bool isCpuTimeToggleCommand(const QString &command, bool *enabled);
    static bool isPariCommand(const QString &command);
    static QString formatTimingText(double cpuSeconds, double wallSeconds);
    PlotCommand buildPlotCommand(const QString &command) const;
    PlotImageResult parsePlotCmo(const QByteArray &cmo) const;
    PlotExecutionResult executePlotCommand(const PlotCommand &plotCommand);
    static QString buildPlotSummary(const PlotCommand &plotCommand, int width, int height);
    PlotExecutionResult axesForPlotCommand(const PlotCommand &plotCommand);
    PlotExecutionResult renderFunctionPlot(const PlotCommand &plotCommand);
    PlotExecutionResult renderImplicitPlot(const PlotCommand &plotCommand, bool contour);
    PlotExecutionResult renderPolarPlot(const PlotCommand &plotCommand);
    bool evaluateNumericExpression(const QString &expression,
        const QList<QPair<QString, double>> &substitutions, double *value);
    static bool parseDoubleText(const QString &text, double *value);
    static QString formatAsirDouble(double value);
    static QStringList splitTopLevelArguments(const QString &arguments);
    static bool isGeometryArgument(const QString &argument);
    static bool isVariableRangeArgument(const QString &argument);
    static QStringList extractVariableNames(const QString &expression);
    static QString normalizedPlotArguments(const QString &functionName, const QString &arguments);
    QString extractLatexSource(const QString &command) const;
    QString normalizeCommand(const QString &command) const;
    QString stripCommandTerminator(const QString &command) const;

    bool isCancelled() const { return m_cancelRequested.load(std::memory_order_relaxed); }
    void clearCancel() { m_cancelRequested.store(false, std::memory_order_relaxed); }
    bool isUserInterruptRequested() const { return m_userInterruptRequested.load(std::memory_order_relaxed); }
    void clearUserInterrupt() { m_userInterruptRequested.store(false, std::memory_order_relaxed); }

    AppConfig m_config;
    bool m_initialized = false;
    bool m_printTexFormAvailable = false;
    bool m_cpuTimeEnabled = false;
    QMutex m_mutex;
    std::atomic<bool> m_cancelRequested{false};
    std::atomic<bool> m_userInterruptRequested{false};
    std::atomic<bool> m_executing{false};
    std::atomic<bool> m_pariActive{false};
    std::atomic<unsigned long> m_nativeThreadId{0};
};

class LocalAsirEngine : public AsirEngine {
    Q_OBJECT

public:
    explicit LocalAsirEngine(const AppConfig &config, QObject *parent = nullptr);
    ~LocalAsirEngine() override;

    bool initialize() override;
    void shutdown() override;
    void executeString(const QString &command, bool producePresentation = true) override;
    void interruptCurrent() override;
    bool isReady() const override;

private:
    AppConfig m_config;
    QThread m_workerThread;
    AsirWorker *m_worker = nullptr;
    bool m_ready = false;
};
