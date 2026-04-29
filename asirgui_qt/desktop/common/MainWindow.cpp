#include "desktop/common/MainWindow.h"

#include "desktop/common/InputWidget.h"
#include "desktop/common/MathView.h"
#include "desktop/common/PlotView.h"

#include <QActionGroup>
#include <QApplication>
#include <QDesktopServices>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QColorDialog>
#include <QFontDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QRegularExpression>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QTimer>
#include <QStringList>
#include <QUuid>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

namespace {

constexpr int kHighRiskLatexSoftLimit = 2000;
constexpr int kHighRiskTextSoftLimit = 4000;
constexpr int kLatexHardLimit = 100000;
constexpr int kTextHardLimit = 200000;

const QRegularExpression kHighRiskMathPattern(
    QStringLiteral("^\\s*(?:[A-Za-z_][A-Za-z0-9_]*\\s*=\\s*)?"
                   "(nd_gr|gr|dp_gr|fglm|syz)\\s*\\("),
    QRegularExpression::CaseInsensitiveOption);

QString referenceVariableName(int id)
{
    return QStringLiteral("AsirGuiRef%1").arg(id);
}


QString editorHelpHtml()
{
    return QStringLiteral(
        "<html><body>"
        "<h3>キーバインド</h3>"
        "<table cellspacing=\"4\" cellpadding=\"2\">"
        "<tr><td><code>Shift+Enter</code></td><td>コマンド実行</td></tr>"
        "<tr><td><code>Enter</code></td><td>改行</td></tr>"
        "<tr><td><code>Ctrl-C</code></td><td>実行中の計算を中断</td></tr>"
        "<tr><td><code>Ctrl-P</code></td><td>前の履歴（履歴モードに入る）</td></tr>"
        "<tr><td><code>Ctrl-N</code></td><td>次の履歴</td></tr>"
        "<tr><td><code>Up / Down</code></td><td>（履歴モードでの）履歴移動</td></tr>"
        "<tr><td><code>Ctrl-A</code></td><td>行頭へ移動</td></tr>"
        "<tr><td><code>Ctrl-E</code></td><td>行末へ移動</td></tr>"
        "<tr><td><code>Ctrl-B</code></td><td>1文字左</td></tr>"
        "<tr><td><code>Ctrl-F</code></td><td>1文字右</td></tr>"
        "<tr><td><code>Ctrl-H / Backspace</code></td><td>直前1文字削除</td></tr>"
        "<tr><td><code>Ctrl-D</code></td><td>カーソル位置1文字削除</td></tr>"
        "<tr><td><code>Ctrl-K</code></td><td>カーソルから行末まで削除</td></tr>"
        "<tr><td><code>Ctrl-U</code></td><td>行頭からカーソル位置まで削除</td></tr>"
        "<tr><td><code>Ctrl-W</code></td><td>直前単語削除</td></tr>"
        "<tr><td><code>Alt-B</code></td><td>前の単語へ移動</td></tr>"
        "<tr><td><code>Alt-F</code></td><td>次の単語へ移動</td></tr>"
        "<tr><td><code>Alt-Backspace</code></td><td>直前単語削除</td></tr>"
        "<tr><td><code>!!</code></td><td>直前履歴展開</td></tr>"
        "<tr><td><code>!prefix</code></td><td>prefix一致履歴展開</td></tr>"
        "<tr><td><code>!prefix:p</code></td><td>展開のみして未実行</td></tr>"
        "<tr><td><code>Tab</code></td><td>Asir関数名・変数名補完</td></tr>"
        "</table>"
        "<h3>計算中断</h3>"
        "<ul>"
        "<li><code>Ctrl-C</code> または <b>Run -&gt; Interrupt</b> で中断ダイアログを開きます。</li>"
        "<li>計算中に <b>Yes</b> を選ぶと、Asir は <code>return to toplevel</code> に戻ります。</li>"
        "<li>中断後も、そのまま次のコマンドを続けて実行できます。</li>"
        "</ul>"
        "<h3>補完の対象候補</h3>"
        "<ul>"
        "<li>Asir 組み込み関数名</li>"
        "<li>履歴中の識別子</li>"
        "<li>現在のエディタ内容</li>"
        "</ul>"
        "<h3>補完の具体例</h3>"
        "<p><b>履歴に</b></p>"
        "<pre>load(\"cyclic\");\nC = cyclic(4);\nA = nd_gr(C,vars(C),31991,0);</pre>"
        "<p>がある場合、<code>load</code>, <code>cyclic</code>, <code>C</code>, <code>A</code>, <code>nd_gr</code>, <code>vars</code> などが候補に入ります。</p>"
        "<p><b>現在のエディタに</b></p>"
        "<pre>def test(A) {\n    ResultValue = car(A);\n    return ResultValue;\n}</pre>"
        "<p>と書いてある場合、<code>test</code>, <code>A</code>, <code>ResultValue</code>, <code>car</code>, <code>return</code>, <code>def</code> などが候補に入ります。</p>"
        "</body></html>");
}

}

MainWindow::MainWindow(AsirEngine *engine, AppConfig *config, QWidget *parent)
    : QMainWindow(parent)
    , m_engine(engine)
    , m_config(config)
{
    m_history.load(historyPath());
    createUi();
    applyConfig();
    createMenus();

    connect(m_input, &InputWidget::commandSubmitted, this, &MainWindow::submitCommand);
    connect(m_input, &InputWidget::interruptRequested, this, &MainWindow::interruptComputation);
    connect(m_mathView, &MathView::forceRenderRequested, this, &MainWindow::forceMathRender);
    connect(m_engine, &AsirEngine::resultReady, this, &MainWindow::handleResult);
    connect(m_engine, &AsirEngine::errorOccurred, this, &MainWindow::handleError);
    connect(m_engine, &AsirEngine::engineReady, this, [this]() {
        statusBar()->showMessage(QStringLiteral("Asir エンジンの準備が完了しました。"));
    });
    if (m_engine->isReady()) {
        statusBar()->showMessage(QStringLiteral("Asir エンジンの準備が完了しました。"));
    }

    setWindowTitle(QStringLiteral("AsirGUI Linux"));
    resize(1200, 760);
}

MainWindow::~MainWindow()
{
    m_history.save(historyPath());
    saveConfig();
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void MainWindow::submitCommand(const QString &command)
{
    const QString stripped = stripCommandTerminator(command);
    if (isExitCommand(stripped)) {
        const int commandId = m_nextCommandId++;
        const QString displayCommand = command.trimmed();
        m_history.add(command);
        appendTranscriptLine(QStringLiteral("[%1] %2").arg(commandId).arg(displayCommand));
        appendLogLine(QStringLiteral("[%1] %2").arg(commandId).arg(displayCommand));
        statusBar()->showMessage(QStringLiteral("AsirGUI を終了します。"), 2000);
        QTimer::singleShot(0, this, [this]() { close(); });
        return;
    }

    QString engineCommand;
    const PendingCommand pending = prepareCommand(command, &engineCommand);

    m_history.add(command);
    m_pendingCommands.append(pending);
    appendTranscriptLine(QStringLiteral("[%1] %2").arg(pending.id).arg(pending.displayCommand));
    appendLogLine(QStringLiteral("[%1] %2").arg(pending.id).arg(pending.displayCommand));
    if (m_interruptAction) {
        m_interruptAction->setEnabled(true);
    }
    m_input->setInterruptEnabled(true);
    statusBar()->showMessage(QStringLiteral("実行中: [%1] %2   (Ctrl+C で中断)")
        .arg(pending.id).arg(pending.displayCommand));
    m_engine->executeString(engineCommand, pending.showResult);
}

void MainWindow::handleResult(const AsirResult &r)
{
    const PendingCommand pending = m_pendingCommands.isEmpty() ? PendingCommand{} : m_pendingCommands.takeFirst();
    const QString commandTitle = QStringLiteral("[%1] %2").arg(pending.id).arg(pending.displayCommand);

    const bool shouldDisplayResult = pending.showResult || r.isError;
    if (shouldDisplayResult) {
        if (!r.plotImage.isNull()) {
            const QString summaryLine = r.plotSummary.isEmpty() ? r.text : r.plotSummary;
            if (!summaryLine.isEmpty()) {
                const QString transcript = QStringLiteral("[Plot] [%1] %2").arg(pending.id).arg(summaryLine);
                appendTranscriptLine(transcript);
                appendLogLine(transcript);
            }
        } else if (!r.text.isEmpty()) {
            appendTranscriptLine(r.text);
            appendLogLine(r.text);
        }
    }
    if (!r.timingText.isEmpty()) {
        appendTranscriptLine(r.timingText);
        appendLogLine(QStringLiteral("Time[%1]> %2").arg(pending.id).arg(r.timingText));
    }
    if (pending.referenceCapable && !r.isError) {
        m_referenceableIds.insert(pending.id);
        m_lastReferenceableId = pending.id;
    }
    const bool hasMathContent = shouldDisplayResult && (!r.latex.isEmpty() || !r.mathPlainText.isEmpty());
    if (shouldDisplayResult && !r.mathPlainText.isEmpty()) {
        m_mathView->appendPlainText(commandTitle, r.mathPlainText);
        appendLogLine(QStringLiteral("MathText[%1]> %2").arg(pending.id).arg(r.mathPlainText));
    } else if (shouldDisplayResult && !r.latex.isEmpty()) {
        bool allowForce = false;
        QString details;
        if (shouldSkipMathRendering(pending.displayCommand, r, &allowForce, &details)) {
            const QString token = allowForce
                ? storeDeferredMathRender(pending.id, commandTitle, r.latex)
                : QString{};
            const QString message = allowForce
                ? QStringLiteral("結果が大きすぎるため、Math タブでの数式レンダリングを省略しました。\n"
                                 "上部の Output ペインで全文を確認してください。\n"
                                 "必要なら「強制レンダリング」を実行してください。")
                : QStringLiteral("結果が大きすぎるため、Math タブでの数式レンダリングは行いません。\n"
                                 "上部の Output ペインで全文を確認してください。");
            m_mathView->appendRenderSkipped(commandTitle, message, details, token, allowForce);
            appendLogLine(QStringLiteral("MathSkip[%1]> %2").arg(pending.id).arg(details));
            statusBar()->showMessage(QStringLiteral("Math タブの数式レンダリングを省略しました。"), 4000);
        } else {
            m_mathView->appendLatex(commandTitle, r.latex);
            appendLogLine(QStringLiteral("LaTeX[%1]> %2").arg(pending.id).arg(r.latex));
        }
    }
    if (shouldDisplayResult && !r.plotImage.isNull()) {
        const QString title = r.plotSummary.isEmpty() ? commandTitle : QStringLiteral("[%1] %2").arg(pending.id).arg(r.plotSummary);
        m_plotView->appendPlot(title, r.plotImage, r.axes.valid, r.axes.xMin, r.axes.xMax, r.axes.yMin, r.axes.yMax);
        m_resultTabs->setCurrentWidget(m_plotView);
        QTimer::singleShot(0, this, [this]() {
            m_plotView->scrollToLatest();
            m_input->setFocus(Qt::OtherFocusReason);
        });
        appendLogLine(QStringLiteral("Plot[%1]> %2").arg(pending.id).arg(title));
    } else if (hasMathContent) {
        m_resultTabs->setCurrentWidget(m_mathView);
        QTimer::singleShot(0, this, [this]() {
            m_mathView->scrollToBottom();
            m_input->setFocus(Qt::OtherFocusReason);
        });
    }
    appendTranscriptLine(QString());
    if (m_interruptAction) {
        m_interruptAction->setEnabled(!m_pendingCommands.isEmpty());
    }
    m_input->setInterruptEnabled(!m_pendingCommands.isEmpty());
    statusBar()->showMessage(QStringLiteral("実行完了"), 2000);
}

void MainWindow::forceMathRender(const QString &token)
{
    const auto it = m_deferredMathRenders.constFind(token);
    if (it == m_deferredMathRenders.constEnd()) {
        statusBar()->showMessage(QStringLiteral("強制レンダリング対象が見つかりません。"), 3000);
        return;
    }

    const DeferredMathRender deferred = it.value();
    m_deferredMathRenders.remove(token);
    m_mathView->appendLatex(deferred.title, deferred.latex);
    appendLogLine(QStringLiteral("LaTeXForce[%1]> %2").arg(deferred.id).arg(deferred.latex));
    m_resultTabs->setCurrentWidget(m_mathView);
    QTimer::singleShot(0, this, [this]() {
        m_mathView->scrollToBottom();
        m_input->setFocus(Qt::OtherFocusReason);
    });
    statusBar()->showMessage(QStringLiteral("Math タブの強制レンダリングを開始しました。"), 3000);
}

void MainWindow::handleError(const QString &message)
{
    if (!m_pendingCommands.isEmpty()) {
        m_pendingCommands.takeFirst();
    }
    appendTranscriptLine(message);
    appendTranscriptLine(QString());
    appendLogLine(QStringLiteral("Error> %1").arg(message));
    if (m_interruptAction) {
        m_interruptAction->setEnabled(!m_pendingCommands.isEmpty());
    }
    m_input->setInterruptEnabled(!m_pendingCommands.isEmpty());
    statusBar()->showMessage(message, 4000);
}

void MainWindow::interruptComputation()
{
    if (!m_interruptAction || !m_interruptAction->isEnabled()) {
        return;
    }
    const QMessageBox::StandardButton answer = QMessageBox::question(this,
        QStringLiteral("Interrupt"),
        QStringLiteral("Abort this computation?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (answer != QMessageBox::Yes) {
        return;
    }
    m_engine->interruptCurrent();
    statusBar()->showMessage(QStringLiteral("中断要求を送信しました。"), 2000);
}

void MainWindow::setTextOnlyMode()
{
    updateDisplayMode(DisplayMode::TextOnly);
}

void MainWindow::setMathOnlyMode()
{
    updateDisplayMode(DisplayMode::MathOnly);
}

void MainWindow::setSplitMode()
{
    updateDisplayMode(DisplayMode::Split);
}

void MainWindow::toggleLogging(bool enabled)
{
    if (enabled) {
        const QString path = QFileDialog::getSaveFileName(
            this,
            QStringLiteral("ログ保存先"),
            QStringLiteral("asirgui.log"),
            QStringLiteral("Text Files (*.log *.txt);;All Files (*)"));
        if (path.isEmpty()) {
            m_loggingAction->setChecked(false);
            return;
        }

        delete m_logFile;
        m_logFile = new QFile(path, this);
        if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            delete m_logFile;
            m_logFile = nullptr;
            m_loggingAction->setChecked(false);
            handleError(QStringLiteral("ログファイルを開けませんでした。"));
            return;
        }
        statusBar()->showMessage(QStringLiteral("ログ記録を開始しました。"), 2000);
        return;
    }

    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
    statusBar()->showMessage(QStringLiteral("ログ記録を停止しました。"), 2000);
}

void MainWindow::openScript()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Asir スクリプトを開く"),
        QString(),
        QStringLiteral("Asir Files (*.rr *.asir);;All Files (*)"));
    if (path.isEmpty()) {
        return;
    }

    const QString command = QStringLiteral("load(\"%1\");").arg(escapeAsirPath(path));
    submitCommand(command);
}

void MainWindow::changeOutputFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, m_output->font(), this, QStringLiteral("上部ペインのフォント"));
    if (!ok) {
        return;
    }
    m_output->setFont(font);
}

void MainWindow::changeInputFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, m_input->font(), this, QStringLiteral("入力ペインのフォント"));
    if (!ok) {
        return;
    }
    m_input->applyEditorFont(font);
}

void MainWindow::changePlotForeground()
{
    const QColor color = QColorDialog::getColor(
        m_plotView->foregroundColor(),
        this,
        QStringLiteral("プロット線色"));
    if (!color.isValid()) {
        return;
    }
    m_plotView->setForegroundColor(color);
}

void MainWindow::changePlotBackground()
{
    const QColor color = QColorDialog::getColor(
        m_plotView->backgroundColor(),
        this,
        QStringLiteral("プロット背景色"));
    if (!color.isValid()) {
        return;
    }
    m_plotView->setBackgroundColor(color);
}

void MainWindow::showEditorHelp()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Editor Help"));
    dialog.resize(760, 560);

    auto *layout = new QVBoxLayout(&dialog);
    auto *browser = new QTextBrowser(&dialog);
    browser->setOpenExternalLinks(true);
    browser->setReadOnly(true);
    browser->setHtml(editorHelpHtml());
    browser->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    layout->addWidget(browser);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttons);

    dialog.exec();
}

void MainWindow::openUserManual()
{
    openExternalUrl(QStringLiteral("User's Manual"),
        QStringLiteral("http://www.math.kobe-u.ac.jp/OpenXM/Current/doc/index-doc-asir-ja.html"));
}

void MainWindow::openOfficialSite()
{
    openExternalUrl(QStringLiteral("Official Site"),
        QStringLiteral("http://www.math.kobe-u.ac.jp/Asir/asir-ja.html"));
}

void MainWindow::createUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    m_input = new InputWidget(&m_history, central);
    m_output = new QPlainTextEdit(central);
    m_mathView = new MathView(central);
    m_plotView = new PlotView(central);
    m_resultTabs = new QTabWidget(central);
    m_displaySplitter = new QSplitter(Qt::Vertical, central);
    m_mainSplitter = new QSplitter(Qt::Vertical, central);

    m_output->setReadOnly(true);
    m_output->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_output->setPlaceholderText(QStringLiteral("Asir の文字出力"));

    m_resultTabs->addTab(m_mathView, QStringLiteral("Math"));
    m_resultTabs->addTab(m_plotView, QStringLiteral("Plot"));

    m_displaySplitter->addWidget(m_output);
    m_displaySplitter->addWidget(m_resultTabs);
    m_displaySplitter->setStretchFactor(0, 1);
    m_displaySplitter->setStretchFactor(1, 1);
    m_displaySplitter->setChildrenCollapsible(false);
    connect(m_displaySplitter, &QSplitter::splitterMoved, this, [this](int, int) {
        saveConfig();
    });

    m_mainSplitter->addWidget(m_displaySplitter);
    m_mainSplitter->addWidget(m_input);
    m_mainSplitter->setStretchFactor(0, 5);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setChildrenCollapsible(false);
    m_mainSplitter->setSizes({560, 160});

    layout->addWidget(m_mainSplitter, 1);
    setCentralWidget(central);
    statusBar()->showMessage(QStringLiteral("Asir エンジンを初期化しています..."));
    updateDisplayMode(DisplayMode::Split);
}

void MainWindow::applyConfig()
{
    if (!m_config || !m_plotView) {
        return;
    }

    const QList<int> displaySizes = m_config->displaySplitterSizes.size() == 2
        ? m_config->displaySplitterSizes
        : QList<int>{1, 1};
    QTimer::singleShot(0, this, [this, displaySizes]() {
        if (m_displaySplitter) {
            m_displaySplitter->setSizes(displaySizes);
        }
    });

    if (!m_config->outputFont.family().isEmpty()) {
        m_output->setFont(m_config->outputFont);
    }
    if (!m_config->inputFont.family().isEmpty()) {
        m_input->applyEditorFont(m_config->inputFont);
    }

    m_plotView->setForegroundColor(m_config->plotForegroundColor);
    m_plotView->setBackgroundColor(m_config->plotBackgroundColor);
    m_plotView->setLineThickness(m_config->plotLineThickness);
    if (m_config->plotFitToWidth) {
        m_plotView->setFitToWidth(true);
    } else {
        m_plotView->setZoomFactor(m_config->plotZoomFactor);
    }
    m_plotView->setShowAxes(m_config->plotShowAxes);
    m_plotView->setShowAxisLabels(m_config->plotShowAxisLabels);
}

void MainWindow::saveConfig()
{
    if (!m_config || !m_plotView) {
        return;
    }

    m_config->inputFont = m_input->font();
    m_config->outputFont = m_output->font();
    m_config->displaySplitterSizes = m_displaySplitter->sizes();
    m_config->plotForegroundColor = m_plotView->foregroundColor();
    m_config->plotBackgroundColor = m_plotView->backgroundColor();
    m_config->plotLineThickness = m_plotView->lineThickness();
    m_config->plotZoomFactor = m_plotView->zoomFactor();
    m_config->plotFitToWidth = m_plotView->fitToWidthEnabled();
    m_config->plotShowAxes = m_plotView->showAxes();
    m_config->plotShowAxisLabels = m_plotView->showAxisLabels();
    m_config->save();
}

void MainWindow::createMenus()
{
    auto *fileMenu = menuBar()->addMenu(QStringLiteral("File"));
    fileMenu->addAction(QStringLiteral("Open"), this, &MainWindow::openScript, QKeySequence::Open);

    m_loggingAction = fileMenu->addAction(QStringLiteral("Logging"));
    m_loggingAction->setCheckable(true);
    connect(m_loggingAction, &QAction::toggled, this, &MainWindow::toggleLogging);

    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("Exit"), qApp, &QApplication::quit, QKeySequence::Quit);

    auto *runMenu = menuBar()->addMenu(QStringLiteral("Run"));
    m_interruptAction = runMenu->addAction(QStringLiteral("Interrupt"),
        this, &MainWindow::interruptComputation, QKeySequence(QStringLiteral("Ctrl+C")));
    m_interruptAction->setEnabled(false);
    m_interruptAction->setShortcutContext(Qt::ApplicationShortcut);

    auto *viewMenu = menuBar()->addMenu(QStringLiteral("View"));
    auto *viewGroup = new QActionGroup(this);
    viewGroup->setExclusive(true);

    QAction *textOnly = viewMenu->addAction(QStringLiteral("Text Only"), this, &MainWindow::setTextOnlyMode);
    QAction *mathOnly = viewMenu->addAction(QStringLiteral("Math Only"), this, &MainWindow::setMathOnlyMode);
    QAction *split = viewMenu->addAction(QStringLiteral("Split View"), this, &MainWindow::setSplitMode);
    textOnly->setCheckable(true);
    mathOnly->setCheckable(true);
    split->setCheckable(true);
    split->setChecked(true);
    viewGroup->addAction(textOnly);
    viewGroup->addAction(mathOnly);
    viewGroup->addAction(split);
    viewMenu->addSeparator();
    viewMenu->addAction(QStringLiteral("Output Font..."), this, &MainWindow::changeOutputFont);
    viewMenu->addAction(QStringLiteral("Input Font..."), this, &MainWindow::changeInputFont);

    auto *plotMenu = menuBar()->addMenu(QStringLiteral("Plot"));
    plotMenu->addAction(QStringLiteral("Foreground..."), this, &MainWindow::changePlotForeground);
    plotMenu->addAction(QStringLiteral("Background..."), this, &MainWindow::changePlotBackground);
    plotMenu->addSeparator();

    auto *thicknessGroup = new QActionGroup(this);
    thicknessGroup->setExclusive(true);

    QAction *thin = plotMenu->addAction(QStringLiteral("Thin"));
    QAction *medium = plotMenu->addAction(QStringLiteral("Medium"));
    QAction *thick = plotMenu->addAction(QStringLiteral("Thick"));
    thin->setCheckable(true);
    medium->setCheckable(true);
    thick->setCheckable(true);
    thin->setChecked(true);
    thicknessGroup->addAction(thin);
    thicknessGroup->addAction(medium);
    thicknessGroup->addAction(thick);
    connect(thin, &QAction::triggered, this, [this]() { m_plotView->setLineThickness(1); });
    connect(medium, &QAction::triggered, this, [this]() { m_plotView->setLineThickness(2); });
    connect(thick, &QAction::triggered, this, [this]() { m_plotView->setLineThickness(3); });

    plotMenu->addSeparator();
    QAction *showAxes = plotMenu->addAction(QStringLiteral("Show Axes"));
    showAxes->setCheckable(true);
    showAxes->setChecked(m_plotView->showAxes());
    connect(showAxes, &QAction::toggled, m_plotView, &PlotView::setShowAxes);

    QAction *showAxisLabels = plotMenu->addAction(QStringLiteral("Show Axis Labels"));
    showAxisLabels->setCheckable(true);
    showAxisLabels->setChecked(m_plotView->showAxisLabels());
    connect(showAxisLabels, &QAction::toggled, m_plotView, &PlotView::setShowAxisLabels);

    plotMenu->addSeparator();
    QAction *fitWidth = plotMenu->addAction(QStringLiteral("Fit Width"));
    fitWidth->setCheckable(true);
    fitWidth->setChecked(m_plotView->fitToWidthEnabled());
    connect(fitWidth, &QAction::toggled, m_plotView, &PlotView::setFitToWidth);

    QAction *zoomInAction = plotMenu->addAction(QStringLiteral("Zoom In"));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, [this, fitWidth]() {
        fitWidth->setChecked(false);
        m_plotView->zoomIn();
    });

    QAction *zoomOutAction = plotMenu->addAction(QStringLiteral("Zoom Out"));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, [this, fitWidth]() {
        fitWidth->setChecked(false);
        m_plotView->zoomOut();
    });

    QAction *resetZoomAction = plotMenu->addAction(QStringLiteral("Reset Zoom"));
    connect(resetZoomAction, &QAction::triggered, this, [this, fitWidth]() {
        fitWidth->setChecked(false);
        m_plotView->resetZoom();
    });

    auto *helpMenu = menuBar()->addMenu(QStringLiteral("Help"));
    helpMenu->addAction(QStringLiteral("Editor Help"), this, &MainWindow::showEditorHelp);
    helpMenu->addAction(QStringLiteral("User's Manual"), this, &MainWindow::openUserManual);
    helpMenu->addAction(QStringLiteral("Official Site"), this, &MainWindow::openOfficialSite);
    helpMenu->addSeparator();
    helpMenu->addAction(QStringLiteral("About"), this, [this]() {
        QMessageBox::about(
            this,
            QStringLiteral("About AsirGUI"),
            QStringLiteral("Qt Widgets と libasir.a を使った Linux 向け AsirGUI の初期実装です。"));
    });
}

void MainWindow::updateDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
    const bool textVisible = mode != DisplayMode::MathOnly;
    const bool mathVisible = mode != DisplayMode::TextOnly;

    m_output->setVisible(textVisible);
    m_resultTabs->setVisible(mathVisible);
}

MainWindow::PendingCommand MainWindow::prepareCommand(const QString &command, QString *engineCommand)
{
    PendingCommand pending;
    pending.id = m_nextCommandId++;
    pending.displayCommand = command.trimmed();
    pending.showResult = !isSilentCommand(command);

    QString stripped = stripCommandTerminator(command);
    stripped = expandReferences(stripped);
    pending.referenceCapable = pending.showResult && canStoreReference(stripped);

    if (pending.referenceCapable) {
        const QString refName = referenceVariableName(pending.id);
        *engineCommand = QStringLiteral("%1=(%2);%1;").arg(refName, stripped);
    } else {
        *engineCommand = stripped + QLatin1Char(';');
    }
    return pending;
}

void MainWindow::appendTranscriptLine(const QString &line)
{
    m_output->appendPlainText(line);
}

QString MainWindow::stripCommandTerminator(const QString &command)
{
    QString trimmed = command.trimmed();
    while (!trimmed.isEmpty()) {
        const QChar last = trimmed.back();
        if (last == ';' || last == '$') {
            trimmed.chop(1);
            continue;
        }
        break;
    }
    return trimmed.trimmed();
}

bool MainWindow::isExitCommand(const QString &command)
{
    const QString lowered = command.trimmed().toLower();
    return lowered == QStringLiteral("quit")
        || lowered == QStringLiteral("bye")
        || lowered == QStringLiteral("end");
}

bool MainWindow::isSilentCommand(const QString &command)
{
    const QString trimmed = command.trimmed();
    return trimmed.endsWith(QStringLiteral(";;")) || trimmed.endsWith(QLatin1Char('$'));
}

QString MainWindow::expandReferences(const QString &command) const
{
    QString expanded;
    expanded.reserve(command.size() + 32);

    bool inString = false;
    for (int i = 0; i < command.size(); ++i) {
        const QChar ch = command.at(i);
        if (ch == QLatin1Char('"') && (i == 0 || command.at(i - 1) != QLatin1Char('\\'))) {
            inString = !inString;
            expanded.append(ch);
            continue;
        }

        if (!inString && ch == QLatin1Char('@') && i + 1 < command.size()) {
            const QChar next = command.at(i + 1);
            if (next == QLatin1Char('@')) {
                if (m_lastReferenceableId > 0
                    && m_referenceableIds.contains(m_lastReferenceableId)) {
                    expanded.append(referenceVariableName(m_lastReferenceableId));
                } else {
                    expanded.append(QStringLiteral("@@"));
                }
                ++i;
                continue;
            }

            if (next.isDigit()) {
                int j = i + 1;
                while (j < command.size() && command.at(j).isDigit()) {
                    ++j;
                }
                const QString capturedId = command.mid(i + 1, j - i - 1);
                const int id = capturedId.toInt();
                if (m_referenceableIds.contains(id)) {
                    expanded.append(referenceVariableName(id));
                } else {
                    expanded.append(QLatin1Char('@'));
                    expanded.append(capturedId);
                }
                i = j - 1;
                continue;
            }
        }

        expanded.append(ch);
    }
    return expanded;
}

bool MainWindow::canStoreReference(const QString &command)
{
    const QString lowered = command.trimmed().toLower();
    if (lowered.isEmpty() || lowered.contains('\n') || lowered.contains('=')) {
        return false;
    }

    static const QStringList blockedPrefixes = {
        QStringLiteral("if"),
        QStringLiteral("for"),
        QStringLiteral("while"),
        QStringLiteral("def"),
        QStringLiteral("ifplot"),
        QStringLiteral("conplot"),
        QStringLiteral("plot"),
        QStringLiteral("polarplot"),
        QStringLiteral("memory_ifplot"),
        QStringLiteral("memory_conplot"),
        QStringLiteral("memory_plot"),
        QStringLiteral("memory_polarplot"),
        QStringLiteral("ox_"),
        QStringLiteral("pari"),
        QStringLiteral("load"),
        QStringLiteral("import"),
        QStringLiteral("quit"),
        QStringLiteral("bye"),
        QStringLiteral("end"),
        QStringLiteral("extern"),
        QStringLiteral("return"),
        QStringLiteral("cputime")
    };

    for (const QString &prefix : blockedPrefixes) {
        if (lowered == prefix || lowered.startsWith(prefix + QLatin1Char('(')) || lowered.startsWith(prefix + QLatin1Char(' '))) {
            return false;
        }
        if (prefix.endsWith(QLatin1Char('_')) && lowered.startsWith(prefix)) {
            return false;
        }
    }
    return true;
}

void MainWindow::appendLogLine(const QString &line)
{
    if (!m_logFile) {
        return;
    }

    QTextStream out(m_logFile);
    out << QDateTime::currentDateTime().toString(Qt::ISODate) << ' ' << line << '\n';
    out.flush();
}

void MainWindow::openExternalUrl(const QString &title, const QString &url)
{
    const QUrl targetUrl = QUrl::fromUserInput(url);
    const QString encodedUrl = targetUrl.toString(QUrl::FullyEncoded);
    bool opened = false;

#ifdef Q_OS_WIN
    opened = QDesktopServices::openUrl(targetUrl);
    if (!opened) {
        opened = QProcess::startDetached(QStringLiteral("rundll32.exe"),
            {QStringLiteral("url.dll,FileProtocolHandler"), encodedUrl});
    }
#elif defined(Q_OS_MACOS)
    opened = QProcess::startDetached(QStringLiteral("open"), {encodedUrl});
#else
    if (qEnvironmentVariableIsSet("WSL_DISTRO_NAME")) {
        const QString powerShell = QStringLiteral("/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe");
        const QString cmdExe = QStringLiteral("/mnt/c/Windows/System32/cmd.exe");

        if (QFileInfo::exists(powerShell)) {
            opened = QProcess::startDetached(powerShell,
                {QStringLiteral("-NoProfile"),
                 QStringLiteral("-Command"),
                 QStringLiteral("Start-Process '%1'").arg(encodedUrl)});
        }
        if (!opened && QFileInfo::exists(cmdExe)) {
            opened = QProcess::startDetached(cmdExe,
                {QStringLiteral("/c"), QStringLiteral("start"), QStringLiteral(""), encodedUrl});
        }
    }
    if (!opened) {
        opened = QDesktopServices::openUrl(targetUrl);
    }
    if (!opened) {
        opened = QProcess::startDetached(QStringLiteral("xdg-open"), {encodedUrl});
    }
#endif

    if (opened) {
        return;
    }

    QMessageBox box(this);
    box.setWindowTitle(title);
    box.setIcon(QMessageBox::Warning);
    box.setText(QStringLiteral("ブラウザで URL を開けませんでした。"));
    box.setInformativeText(url);
    box.setTextInteractionFlags(Qt::TextSelectableByMouse);
    box.exec();
}

QString MainWindow::historyPath() const
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return QDir(base).filePath(QStringLiteral("history.txt"));
}

QString MainWindow::escapeAsirPath(const QString &path)
{
    QString escaped = path;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    return escaped;
}

bool MainWindow::isHighRiskMathCommand(const QString &command)
{
    return kHighRiskMathPattern.match(command).hasMatch();
}

bool MainWindow::shouldSkipMathRendering(const QString &command, const AsirResult &result,
    bool *allowForce, QString *details)
{
    const int latexLength = result.latex.size();
    const int textLength = result.text.size();
    const bool highRisk = isHighRiskMathCommand(command);
    const bool exceedsHardLimit = latexLength > kLatexHardLimit || textLength > kTextHardLimit;
    const bool exceedsSoftLimit = highRisk
        ? (latexLength > kHighRiskLatexSoftLimit || textLength > kHighRiskTextSoftLimit)
        : false;

    if (allowForce) {
        *allowForce = exceedsSoftLimit && !exceedsHardLimit;
    }
    if (details) {
        QStringList reasons;
        if (highRisk) {
            reasons.append(QStringLiteral("高リスク関数"));
        }
        reasons.append(exceedsHardLimit
                ? QStringLiteral("強制スキップ閾値超過")
                : QStringLiteral("サイズ超過"));
        *details = QStringLiteral("LaTeX: %1 文字\nText: %2 文字\n判定: %3")
            .arg(latexLength)
            .arg(textLength)
            .arg(reasons.join(QStringLiteral(" + ")));
    }
    return exceedsHardLimit || exceedsSoftLimit;
}

QString MainWindow::storeDeferredMathRender(int id, const QString &title, const QString &latex)
{
    const QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_deferredMathRenders.insert(token, DeferredMathRender{id, title, latex});
    return token;
}
