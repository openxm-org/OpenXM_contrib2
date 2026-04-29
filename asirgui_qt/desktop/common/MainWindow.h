#pragma once

#include "core/AppConfig.h"
#include "core/AsirEngine.h"
#include "core/CommandHistory.h"

#include <QImage>
#include <QHash>
#include <QMainWindow>
#include <QSet>

class InputWidget;
class MathView;
class PlotView;
class QPlainTextEdit;
class QSplitter;
class QAction;
class QFile;
class QTabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(AsirEngine *engine, AppConfig *config, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void submitCommand(const QString &command);
    void handleResult(const AsirResult &result);
    void handleError(const QString &message);
    void setTextOnlyMode();
    void setMathOnlyMode();
    void setSplitMode();
    void toggleLogging(bool enabled);
    void openScript();
    void changeOutputFont();
    void changeInputFont();
    void changePlotForeground();
    void changePlotBackground();
    void showEditorHelp();
    void openUserManual();
    void openOfficialSite();
    void interruptComputation();
    void forceMathRender(const QString &token);

private:
    struct PendingCommand {
        int id = 0;
        QString displayCommand;
        bool showResult = true;
        bool referenceCapable = false;
    };

    struct DeferredMathRender {
        int id = 0;
        QString title;
        QString latex;
    };

    enum class DisplayMode {
        TextOnly,
        MathOnly,
        Split
    };

    PendingCommand prepareCommand(const QString &command, QString *engineCommand);
    void appendTranscriptLine(const QString &line);
    static QString stripCommandTerminator(const QString &command);
    static bool isExitCommand(const QString &command);
    static bool isSilentCommand(const QString &command);
    QString expandReferences(const QString &command) const;
    static bool canStoreReference(const QString &command);
    void createUi();
    void createMenus();
    void applyConfig();
    void saveConfig();
    void updateDisplayMode(DisplayMode mode);
    void appendLogLine(const QString &line);
    void openExternalUrl(const QString &title, const QString &url);
    QString historyPath() const;
    static QString escapeAsirPath(const QString &path);
    static bool isHighRiskMathCommand(const QString &command);
    static bool shouldSkipMathRendering(const QString &command, const AsirResult &result,
        bool *allowForce, QString *details);
    QString storeDeferredMathRender(int id, const QString &title, const QString &latex);

    AsirEngine *m_engine = nullptr;
    AppConfig *m_config = nullptr;
    CommandHistory m_history;
    InputWidget *m_input = nullptr;
    QPlainTextEdit *m_output = nullptr;
    MathView *m_mathView = nullptr;
    PlotView *m_plotView = nullptr;
    QTabWidget *m_resultTabs = nullptr;
    QSplitter *m_displaySplitter = nullptr;
    QSplitter *m_mainSplitter = nullptr;
    QAction *m_loggingAction = nullptr;
    QAction *m_interruptAction = nullptr;
    QFile *m_logFile = nullptr;
    DisplayMode m_displayMode = DisplayMode::Split;
    int m_nextCommandId = 1;
    QList<PendingCommand> m_pendingCommands;
    QSet<int> m_referenceableIds;
    int m_lastReferenceableId = 0;
    QHash<QString, DeferredMathRender> m_deferredMathRenders;
};
