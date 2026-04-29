#include "core/LocalAsirEngine.h"

#include <QByteArray>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QImage>
#include <QMetaObject>
#include <QPainter>
#include <QPointF>
#include <QRegularExpression>
#include <QSet>
#include <QFile>
#include <QtEndian>

#include <cmath>
#include <cstdio>
#include <ctime>
#if !defined(_WIN32)
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#endif
#include <gc/gc.h>
#include <ox/cmotag.h>
#include <limits>

extern "C" __attribute__((weak)) char *find_asirrc()
{
    return nullptr;
}

extern "C" {
int asir_ox_init(int byteorder);
void asir_ox_execute_string(char *command);
int asir_ox_pop_cmo(void *cmo, int limit);
int asir_ox_peek_cmo_size(void);
int asir_ox_pop_string(void *string, int limit);
int asir_ox_peek_cmo_string_length(void);
int asir_ox_shutdown_pari(void);
void reset_io(void);
extern char LastError[];
extern int no_debug_on_error;
}

namespace {

constexpr int kDefaultPlotSize = 300;
constexpr int kFallbackStringBufferSize = 4096;

const QRegularExpression kDirectPlotPattern(
    QStringLiteral("^\\s*(memory_)?(ifplot|conplot|plot|polarplot)\\s*\\((.*)\\)\\s*$"),
    QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
const QRegularExpression kAssignmentPlotPattern(
    QStringLiteral("^\\s*([A-Za-z][A-Za-z0-9_]*)\\s*=\\s*(memory_)?(ifplot|conplot|plot|polarplot)\\s*\\((.*)\\)\\s*$"),
    QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
const QRegularExpression kWrappedPattern(
    QStringLiteral("^AsirGuiRef\\d+=\\((.*)\\);\\s*AsirGuiRef\\d+$"),
    QRegularExpression::DotMatchesEverythingOption);
const QRegularExpression kPariPattern(
    QStringLiteral("^\\s*(?:[A-Za-z][A-Za-z0-9_]*\\s*=\\s*)?pari\\s*\\("),
    QRegularExpression::CaseInsensitiveOption);
const QRegularExpression kCpuTimePattern(
    QStringLiteral("^\\s*cputime\\s*\\(\\s*([+-]?\\d+)\\s*\\)\\s*[;$]?\\s*$"),
    QRegularExpression::CaseInsensitiveOption);

bool looksLikeAsirError(const QString &text)
{
    const QString trimmed = text.trimmed();
    return trimmed.startsWith(QStringLiteral("return to toplevel"), Qt::CaseInsensitive)
        || trimmed.startsWith(QStringLiteral("syntax error"), Qt::CaseInsensitive)
        || trimmed.startsWith(QStringLiteral("undefined"), Qt::CaseInsensitive)
        || trimmed.startsWith(QStringLiteral("error("), Qt::CaseInsensitive)
        || trimmed.contains(QStringLiteral("detected during parsing"), Qt::CaseInsensitive);
}

QString normalizedAsirErrorText(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.contains(QStringLiteral("syntax error"), Qt::CaseInsensitive)) {
        return QStringLiteral("syntax error");
    }
    return trimmed;
}

const QRegularExpression kLibmodePrefix(
    QStringLiteral("^\"string\", near line \\d+: "));

QString extractParseDiagnostic(const QString &text)
{
    const QStringList lines = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    QStringList matched;
    for (const QString &rawLine : lines) {
        QString trimmed = rawLine.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        trimmed.replace(kLibmodePrefix, QString{});
        if (trimmed.contains(QStringLiteral("syntax error"), Qt::CaseInsensitive)
            || trimmed.contains(QStringLiteral("undeclared"), Qt::CaseInsensitive)
            || trimmed.contains(QStringLiteral("conflicting declarations"), Qt::CaseInsensitive)) {
            matched.append(trimmed);
        }
    }
    return matched.join(QLatin1Char('\n'));
}

// プロセス全体の stderr を一時的に tmpfile へ差し替える。
// m_mutex 保護区間でのみ使い、同区間に他スレッドが stderr を使うと、その出力も一緒に捕捉・破棄される。
class ScopedStderrCapture {
public:
    bool begin()
    {
#if defined(_WIN32)
        return false;
#else
        flush();
        m_savedFd = dup(fileno(stderr));
        if (m_savedFd < 0) {
            return false;
        }

        m_file = tmpfile();
        if (!m_file) {
            close(m_savedFd);
            m_savedFd = -1;
            return false;
        }

        if (dup2(fileno(m_file), fileno(stderr)) < 0) {
            fclose(m_file);
            m_file = nullptr;
            close(m_savedFd);
            m_savedFd = -1;
            return false;
        }
        return true;
#endif
    }

    QString end()
    {
#if defined(_WIN32)
        return {};
#else
        if (m_savedFd < 0) {
            return {};
        }

        flush();
        dup2(m_savedFd, fileno(stderr));
        close(m_savedFd);
        m_savedFd = -1;

        QByteArray buffer;
        if (m_file) {
            flush(m_file);
            if (std::fseek(m_file, 0, SEEK_SET) == 0) {
                char chunk[256];
                while (true) {
                    const size_t read = std::fread(chunk, 1, sizeof(chunk), m_file);
                    if (read == 0) {
                        break;
                    }
                    buffer.append(chunk, static_cast<int>(read));
                }
            }
            fclose(m_file);
            m_file = nullptr;
        }
        return QString::fromLocal8Bit(buffer).trimmed();
#endif
    }

    ~ScopedStderrCapture()
    {
#if !defined(_WIN32)
        if (m_savedFd >= 0) {
            flush();
            dup2(m_savedFd, fileno(stderr));
            close(m_savedFd);
        }
        if (m_file) {
            fclose(m_file);
        }
#endif
    }

private:
    static void flush(FILE *file = stderr)
    {
        if (file) {
            std::fflush(file);
        }
    }

#if !defined(_WIN32)
    int m_savedFd = -1;
    FILE *m_file = nullptr;
#endif
};

double currentCpuSeconds()
{
#if defined(CLOCK_THREAD_CPUTIME_ID)
    timespec ts {};
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) == 0) {
        return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1000000000.0;
    }
#endif
    return static_cast<double>(std::clock()) / static_cast<double>(CLOCKS_PER_SEC);
}

class CmoReader {
public:
    explicit CmoReader(const QByteArray &buffer)
        : m_buffer(buffer)
    {
    }

    bool readInt(qint32 *value)
    {
        if (m_offset + 4 > m_buffer.size()) {
            return false;
        }
        *value = qFromBigEndian<qint32>(reinterpret_cast<const uchar *>(m_buffer.constData() + m_offset));
        m_offset += 4;
        return true;
    }

    bool readRaw(int length, QByteArray *value)
    {
        if (length < 0 || m_offset + length > m_buffer.size()) {
            return false;
        }
        *value = m_buffer.mid(m_offset, length);
        m_offset += length;
        return true;
    }

private:
    const QByteArray &m_buffer;
    int m_offset = 0;
};

bool readCmoInteger(CmoReader *reader, int *value)
{
    qint32 tag = 0;
    if (!reader->readInt(&tag)) {
        return false;
    }

    if (tag == CMO_INT32) {
        qint32 number = 0;
        if (!reader->readInt(&number)) {
            return false;
        }
        *value = number;
        return true;
    }

    if (tag == CMO_ZERO) {
        *value = 0;
        return true;
    }

    if (tag != CMO_ZZ) {
        return false;
    }

    qint32 words = 0;
    if (!reader->readInt(&words)) {
        return false;
    }
    if (words == 0) {
        *value = 0;
        return true;
    }

    const int sign = words < 0 ? -1 : 1;
    const int count = qAbs(words);
    qint64 result = 0;
    for (int i = 0; i < count; ++i) {
        qint32 part = 0;
        if (!reader->readInt(&part)) {
            return false;
        }
        if (i >= 2) {
            return false;
        }
        result |= (static_cast<quint64>(static_cast<quint32>(part)) << (32 * i));
    }
    result *= sign;
    if (result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()) {
        return false;
    }
    *value = static_cast<int>(result);
    return true;
}

bool readCmoByteArray(CmoReader *reader, QByteArray *value)
{
    qint32 tag = 0;
    qint32 length = 0;
    if (!reader->readInt(&tag) || tag != CMO_DATUM || !reader->readInt(&length)) {
        return false;
    }
    return reader->readRaw(length, value);
}

bool sendInterruptSignalToThread(unsigned long threadId)
{
#if !defined(_WIN32)
    if (!threadId) {
        return false;
    }
    return pthread_kill(static_cast<pthread_t>(threadId), SIGUSR1) == 0;
#else
    Q_UNUSED(threadId);
    return false;
#endif
}

} // namespace

AsirWorker::AsirWorker(AppConfig config, QObject *parent)
    : QObject(parent)
    , m_config(std::move(config))
{
}

void AsirWorker::init()
{
    m_nativeThreadId.store(
#if !defined(_WIN32)
        static_cast<unsigned long>(pthread_self()),
#else
        0UL,
#endif
        std::memory_order_relaxed);
    {
        QMutexLocker locker(&m_mutex);
        if (m_initialized) {
            emit initialized();
            return;
        }

        qputenv("OpenXM_HOME", QFileInfo(m_config.openXmHome).absoluteFilePath().toUtf8());
        qputenv("ASIR_LIBDIR", QFileInfo(m_config.asirLibDir).absoluteFilePath().toUtf8());
        qputenv("ASIR_CONTRIB_DIR", QFileInfo(m_config.asirContribDir).absoluteFilePath().toUtf8());

        GC_INIT();
        asir_ox_init(1);
        no_debug_on_error = 1;
    }

    const QString namesPath = QFileInfo(QDir(m_config.asirContribDir).filePath(QStringLiteral("names.rr"))).absoluteFilePath();
    if (QFileInfo::exists(namesPath)) {
        const QString escapedPath = QString(namesPath).replace(QLatin1Char('\\'), QStringLiteral("\\\\"))
                                        .replace(QLatin1Char('"'), QStringLiteral("\\\""));
        const StringExecutionResult loadResult =
            executeStringResult(QStringLiteral("load(\"%1\");").arg(escapedPath));
        m_printTexFormAvailable = !loadResult.isError;
    }

    {
        QMutexLocker locker(&m_mutex);
        m_initialized = true;
    }
    emit initialized();
}

void AsirWorker::shutdownWorker()
{
    QMutexLocker locker(&m_mutex);
    if (m_initialized) {
        asir_ox_shutdown_pari();
        m_initialized = false;
    }
    m_printTexFormAvailable = false;
    m_cpuTimeEnabled = false;
    m_cancelRequested.store(false, std::memory_order_relaxed);
    m_userInterruptRequested.store(false, std::memory_order_relaxed);
    m_executing.store(false, std::memory_order_relaxed);
    m_pariActive.store(false, std::memory_order_relaxed);
    m_nativeThreadId.store(0UL, std::memory_order_relaxed);
}

void AsirWorker::execute(const QString &command, bool producePresentation)
{
    {
        QMutexLocker locker(&m_mutex);
        if (!m_initialized) {
            emit errorOccurred(QStringLiteral("Asir エンジンが初期化されていません。"));
            return;
        }
    }

    const QString normalized = normalizeCommand(command);
    if (normalized.isEmpty()) {
        return;
    }
    m_executing.store(true, std::memory_order_relaxed);
    m_pariActive.store(isPariCommand(normalized), std::memory_order_relaxed);

    bool cpuTimeCommandEnabled = false;
    const bool isCpuTimeCommand = isCpuTimeToggleCommand(normalized, &cpuTimeCommandEnabled);
    const bool showTimingForThisCommand = isCpuTimeCommand ? cpuTimeCommandEnabled : m_cpuTimeEnabled;
    QElapsedTimer wallTimer;
    wallTimer.start();
    const double cpuStart = currentCpuSeconds();

    const PlotCommand plotCommand = buildPlotCommand(normalized);
    if (plotCommand.matched) {
        clearCancel();
        const PlotExecutionResult plotResult = executePlotCommand(plotCommand);
        if (isCancelled()) {
            const bool interrupted = isUserInterruptRequested();
            clearCancel();
            clearUserInterrupt();
            m_executing.store(false, std::memory_order_relaxed);
            m_pariActive.store(false, std::memory_order_relaxed);
            if (interrupted) {
                AsirResult result;
                result.command = normalized;
                result.text = QStringLiteral("return to toplevel");
                result.isError = true;
                emit resultReady(result);
            }
            return;
        }
        AsirResult result;
        result.command = normalized;
        result.text = plotResult.text;
        result.plotImage = plotResult.image;
        result.plotSummary = plotResult.summary;
        result.isError = plotResult.isError;
        if (showTimingForThisCommand) {
            const double cpuEnd = currentCpuSeconds();
            result.timingText = formatTimingText(cpuEnd - cpuStart,
                static_cast<double>(wallTimer.nsecsElapsed()) / 1000000000.0);
        }
        result.axes = {plotResult.plotAxesValid, plotResult.plotXMin, plotResult.plotXMax,
                       plotResult.plotYMin, plotResult.plotYMax};
        if (isCpuTimeCommand) {
            m_cpuTimeEnabled = cpuTimeCommandEnabled;
        }
        m_executing.store(false, std::memory_order_relaxed);
        m_pariActive.store(false, std::memory_order_relaxed);
        emit resultReady(result);
        return;
    }

    const StringExecutionResult textResult = executeStringResult(normalized);
    QString latexResult;
    QString mathPlainTextResult;
    if (producePresentation && !textResult.isError) {
        if (isPariCommand(normalized)) {
            // pari の結果は KaTeX ではなく等幅テキストで Math タブに表示する
            mathPlainTextResult = textResult.text;
        } else {
            const QString latexCommand = buildLatexCommand(normalized);
            if (!latexCommand.isEmpty()) {
                const StringExecutionResult latexExecution = executeStringResult(latexCommand);
                latexResult = latexExecution.text;
                if (latexResult.isEmpty() || latexExecution.isError) {
                    latexResult = textResult.text;
                }
            }
        }
    }

    AsirResult result;
    result.command = normalized;
    result.text = textResult.text;
    result.latex = latexResult;
    result.mathPlainText = mathPlainTextResult;
    result.isError = textResult.isError;
    if (showTimingForThisCommand) {
        const double cpuEnd = currentCpuSeconds();
        result.timingText = formatTimingText(cpuEnd - cpuStart,
            static_cast<double>(wallTimer.nsecsElapsed()) / 1000000000.0);
    }
    if (isCpuTimeCommand) {
        m_cpuTimeEnabled = cpuTimeCommandEnabled;
    }
    m_executing.store(false, std::memory_order_relaxed);
    m_pariActive.store(false, std::memory_order_relaxed);
    emit resultReady(result);
}

AsirWorker::StringExecutionResult AsirWorker::executeStringResult(const QString &command)
{
    QByteArray utf8 = command.toUtf8();
    if (!utf8.endsWith('\n')) {
        utf8.append('\n');
    }
    QString text;
    QString sideError;
    QString stderrText;
    bool interrupted = isUserInterruptRequested();
    {
        QMutexLocker locker(&m_mutex);
        ScopedStderrCapture stderrCapture;
        const bool capturingStderr = stderrCapture.begin();
        LastError[0] = '\0';
        asir_ox_execute_string(utf8.data());
        if (capturingStderr) {
            stderrText = stderrCapture.end();
        }
        interrupted = interrupted || isUserInterruptRequested();
        const QString parseDiag = extractParseDiagnostic(stderrText);
        if (!parseDiag.isEmpty()) {
            sideError = parseDiag;
        } else {
            sideError = normalizedAsirErrorText(QString::fromLocal8Bit(LastError));
        }

        if (interrupted) {
            reset_io();
        } else {
            int capacity = kFallbackStringBufferSize;
            const int length = asir_ox_peek_cmo_string_length();
            if (length > 0) {
                capacity = length + 1;
            }
            QByteArray buffer(capacity, '\0');
            const int written = asir_ox_pop_string(buffer.data(), buffer.size());
            if (written >= 0) {
                text = QString::fromUtf8(buffer.constData()).trimmed();
            }
        }
    }
    if (interrupted) {
        text = QStringLiteral("return to toplevel");
        clearUserInterrupt();
        clearCancel();
        return {text, true};
    }

    if (!sideError.isEmpty() && (text.isEmpty() || text == QStringLiteral("0"))) {
        text = text.isEmpty() ? sideError : QStringLiteral("%1\n%2").arg(sideError, text);
    }
    return {text, !sideError.isEmpty() || looksLikeAsirError(text)};
}

QString AsirWorker::executeToString(const QString &command)
{
    return executeStringResult(command).text;
}

bool AsirWorker::isCpuTimeToggleCommand(const QString &command, bool *enabled)
{
    const QRegularExpressionMatch match = kCpuTimePattern.match(command);
    if (!match.hasMatch()) {
        return false;
    }
    bool ok = false;
    const int value = match.captured(1).toInt(&ok);
    if (!ok) {
        return false;
    }
    if (enabled) {
        *enabled = value != 0;
    }
    return true;
}

bool AsirWorker::isPariCommand(const QString &command)
{
    return kPariPattern.match(command).hasMatch();
}

QString AsirWorker::formatTimingText(double cpuSeconds, double wallSeconds)
{
    return QStringLiteral("%1sec(%2sec)")
        .arg(QString::number(cpuSeconds, 'g', 4), QString::number(wallSeconds, 'g', 4));
}

AsirWorker::CmoExecutionResult AsirWorker::executeToCmo(const QString &command)
{
    QByteArray utf8 = command.toUtf8();
    if (!utf8.endsWith('\n')) {
        utf8.append('\n');
    }
    CmoExecutionResult result;
    QString stderrText;
    {
        QMutexLocker locker(&m_mutex);
        ScopedStderrCapture stderrCapture;
        const bool capturingStderr = stderrCapture.begin();
        LastError[0] = '\0';
        asir_ox_execute_string(utf8.data());
        if (capturingStderr) {
            stderrText = stderrCapture.end();
        }

        const QString parseDiag = extractParseDiagnostic(stderrText);
        const QString sideError = !parseDiag.isEmpty()
            ? parseDiag
            : normalizedAsirErrorText(QString::fromLocal8Bit(LastError));
        if (!sideError.isEmpty()) {
            result.isError = true;
            result.errorText = sideError;
            return result;
        }

        const int length = asir_ox_peek_cmo_size();
        if (length <= 0) {
            return result;
        }

        QByteArray buffer(length, '\0');
        const int written = asir_ox_pop_cmo(buffer.data(), buffer.size());
        if (written < 0) {
            return result;
        }
        result.bytes = buffer;
    }
    return result;
}

QString AsirWorker::buildLatexCommand(const QString &command) const
{
    if (!m_printTexFormAvailable) {
        return {};
    }
    const QString expression = extractLatexSource(command).trimmed();
    if (expression.isEmpty()) {
        return {};
    }
    const QString lowered = expression.toLower();
    if (expression.contains('\n') || expression.contains('=')
        || lowered.startsWith(QStringLiteral("ox_"))
        || lowered.startsWith(QStringLiteral("pari("))) {
        return {};
    }
    return QStringLiteral("print_tex_form((%1));").arg(expression);
}

AsirWorker::PlotCommand AsirWorker::buildPlotCommand(const QString &command) const
{
    const QString stripped = stripCommandTerminator(command).trimmed();

    if (const QRegularExpressionMatch match = kAssignmentPlotPattern.match(stripped); match.hasMatch()) {
        const QString variable = match.captured(1).trimmed();
        const QString function = match.captured(3).trimmed().toLower();
        const QString args = normalizedPlotArguments(function, match.captured(4).trimmed());
        return {
            function,
            QStringLiteral("%1=memory_%2(%3);%1;").arg(variable, function, args),
            splitTopLevelArguments(args),
            true
        };
    }

    if (const QRegularExpressionMatch match = kDirectPlotPattern.match(stripped); match.hasMatch()) {
        const QString function = match.captured(2).trimmed().toLower();
        const QString args = normalizedPlotArguments(function, match.captured(3).trimmed());
        return {
            function,
            QStringLiteral("memory_%1(%2);").arg(function, args),
            splitTopLevelArguments(args),
            true
        };
    }

    return {};
}

AsirWorker::PlotImageResult AsirWorker::parsePlotCmo(const QByteArray &cmo) const
{
    CmoReader reader(cmo);

    qint32 tag = 0;
    qint32 count = 0;
    if (!reader.readInt(&tag) || tag != CMO_LIST || !reader.readInt(&count) || count != 3) {
        return {};
    }

    int width = 0;
    int height = 0;
    QByteArray bytes;
    if (!readCmoInteger(&reader, &width) || !readCmoInteger(&reader, &height) || !readCmoByteArray(&reader, &bytes)) {
        return {};
    }
    if (width <= 0 || height <= 0) {
        return {};
    }

    const int scanLength = (width + 7) / 8;
    if (bytes.size() != scanLength * height) {
        return {};
    }

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    for (int y = 0; y < height; ++y) {
        const uchar *row = reinterpret_cast<const uchar *>(bytes.constData() + y * scanLength);
        for (int x = 0; x < width; ++x) {
            if (row[x >> 3] & (1u << (x & 7))) {
                image.setPixelColor(x, y, QColor(0, 0, 0, 255));
            }
        }
    }

    return {image, width, height, static_cast<int>(bytes.size()), true};
}

AsirWorker::PlotExecutionResult AsirWorker::executePlotCommand(const PlotCommand &plotCommand)
{
    const PlotExecutionResult axes = axesForPlotCommand(plotCommand);
    const CmoExecutionResult cmoResult = executeToCmo(plotCommand.command);
    if (cmoResult.isError) {
        PlotExecutionResult result;
        result.text = QStringLiteral("%1\n0").arg(cmoResult.errorText);
        result.isError = true;
        return result;
    }
    const QByteArray cmo = cmoResult.bytes;
    if (!cmo.isEmpty()) {
        const PlotImageResult plotImage = parsePlotCmo(cmo);
        if (plotImage.valid) {
            const QString summary = buildPlotSummary(plotCommand, plotImage.width, plotImage.height);
            PlotExecutionResult result;
            result.text = summary;
            result.summary = summary;
            result.image = plotImage.image;
            result.plotAxesValid = axes.plotAxesValid;
            result.plotXMin = axes.plotXMin;
            result.plotXMax = axes.plotXMax;
            result.plotYMin = axes.plotYMin;
            result.plotYMax = axes.plotYMax;
            result.valid = true;
            return result;
        }
    }

    if (plotCommand.functionName == QStringLiteral("plot")) {
        return renderFunctionPlot(plotCommand);
    }
    if (plotCommand.functionName == QStringLiteral("ifplot")) {
        return renderImplicitPlot(plotCommand, false);
    }
    if (plotCommand.functionName == QStringLiteral("conplot")) {
        return renderImplicitPlot(plotCommand, true);
    }
    if (plotCommand.functionName == QStringLiteral("polarplot")) {
        return renderPolarPlot(plotCommand);
    }
    return {QStringLiteral("未対応の plot 関数です。"), QString(), QImage(), false};
}

QString AsirWorker::buildPlotSummary(const PlotCommand &plotCommand, int width, int height)
{
    QStringList details;
    details.append(QStringLiteral("%1x%2").arg(width).arg(height));

    for (int i = 1; i < plotCommand.arguments.size(); ++i) {
        const QString argument = plotCommand.arguments.at(i).trimmed();
        if (isVariableRangeArgument(argument)) {
            const QStringList parts = splitTopLevelArguments(argument.mid(1, argument.size() - 2));
            if (parts.size() >= 3) {
                details.append(QStringLiteral("%1 <= %2 <= %3")
                    .arg(parts.at(1).trimmed(), parts.at(0).trimmed(), parts.at(2).trimmed()));
            }
        }
    }

    const QString expression = plotCommand.arguments.value(0).trimmed();
    return QStringLiteral("%1 %2 -> %3")
        .arg(plotCommand.functionName, expression, details.join(QStringLiteral(", ")));
}

AsirWorker::PlotExecutionResult AsirWorker::axesForPlotCommand(const PlotCommand &plotCommand)
{
    PlotExecutionResult axes;
    if (plotCommand.functionName == QStringLiteral("ifplot") || plotCommand.functionName == QStringLiteral("conplot")) {
        QStringList rangeArguments;
        for (int i = 1; i < plotCommand.arguments.size(); ++i) {
            const QString argument = plotCommand.arguments.at(i).trimmed();
            if (isVariableRangeArgument(argument)) {
                rangeArguments.append(argument);
            }
        }
        if (rangeArguments.size() >= 2) {
            const QStringList xParts = splitTopLevelArguments(rangeArguments.at(0).mid(1, rangeArguments.at(0).size() - 2));
            const QStringList yParts = splitTopLevelArguments(rangeArguments.at(1).mid(1, rangeArguments.at(1).size() - 2));
            if (xParts.size() >= 3 && yParts.size() >= 3
                && evaluateNumericExpression(xParts.at(1), {}, &axes.plotXMin)
                && evaluateNumericExpression(xParts.at(2), {}, &axes.plotXMax)
                && evaluateNumericExpression(yParts.at(1), {}, &axes.plotYMin)
                && evaluateNumericExpression(yParts.at(2), {}, &axes.plotYMax)) {
                axes.plotAxesValid = true;
            }
        }
        return axes;
    }

    if (plotCommand.functionName == QStringLiteral("plot")) {
        QString rangeArgument;
        QString geometryArgument;
        for (int i = 1; i < plotCommand.arguments.size(); ++i) {
            const QString argument = plotCommand.arguments.at(i).trimmed();
            if (geometryArgument.isEmpty() && isGeometryArgument(argument)) {
                geometryArgument = argument;
            } else if (rangeArgument.isEmpty() && isVariableRangeArgument(argument)) {
                rangeArgument = argument;
            }
        }
        if (rangeArgument.isEmpty()) {
            return axes;
        }
        const QStringList rangeParts = splitTopLevelArguments(rangeArgument.mid(1, rangeArgument.size() - 2));
        if (rangeParts.size() < 3) {
            return axes;
        }
        const QString variable = rangeParts.at(0).trimmed();
        if (!evaluateNumericExpression(rangeParts.at(1), {}, &axes.plotXMin)
            || !evaluateNumericExpression(rangeParts.at(2), {}, &axes.plotXMax)) {
            return axes;
        }

        int width = kDefaultPlotSize;
        if (!geometryArgument.isEmpty()) {
            const QStringList geometryParts = splitTopLevelArguments(geometryArgument.mid(1, geometryArgument.size() - 2));
            bool ok = false;
            const int parsed = geometryParts.value(0).trimmed().toInt(&ok);
            if (ok && parsed > 1) {
                width = parsed;
            }
        }

        bool found = false;
        const double xstep = width > 1 ? (axes.plotXMax - axes.plotXMin) / static_cast<double>(width - 1) : 0.0;
        for (int ix = 0; ix < width; ++ix) {
            const double x = axes.plotXMin + xstep * static_cast<double>(ix);
            double y = 0.0;
            if (!evaluateNumericExpression(plotCommand.arguments.at(0), {{variable, x}}, &y)) {
                continue;
            }
            if (!found) {
                axes.plotYMin = axes.plotYMax = y;
                found = true;
            } else {
                axes.plotYMin = std::min(axes.plotYMin, y);
                axes.plotYMax = std::max(axes.plotYMax, y);
            }
        }
        if (found) {
            if (qFuzzyCompare(axes.plotYMin, axes.plotYMax)) {
                axes.plotYMin -= 1.0;
                axes.plotYMax += 1.0;
            }
            axes.plotAxesValid = true;
        }
        return axes;
    }

    if (plotCommand.functionName == QStringLiteral("polarplot")) {
        QString rangeArgument;
        for (int i = 1; i < plotCommand.arguments.size(); ++i) {
            const QString argument = plotCommand.arguments.at(i).trimmed();
            if (rangeArgument.isEmpty() && isVariableRangeArgument(argument)) {
                rangeArgument = argument;
            }
        }
        if (rangeArgument.isEmpty()) {
            return axes;
        }
        const QStringList rangeParts = splitTopLevelArguments(rangeArgument.mid(1, rangeArgument.size() - 2));
        if (rangeParts.size() < 3) {
            return axes;
        }
        const QString variable = rangeParts.at(0).trimmed();
        double tmin = 0.0;
        double tmax = 0.0;
        if (!evaluateNumericExpression(rangeParts.at(1), {}, &tmin)
            || !evaluateNumericExpression(rangeParts.at(2), {}, &tmax)) {
            return axes;
        }

        bool found = false;
        const int samples = qMax(kDefaultPlotSize, 180);
        const double tstep = samples > 1 ? (tmax - tmin) / static_cast<double>(samples - 1) : 0.0;
        for (int i = 0; i < samples; ++i) {
            const double t = tmin + tstep * static_cast<double>(i);
            double radius = 0.0;
            if (!evaluateNumericExpression(plotCommand.arguments.at(0), {{variable, t}}, &radius)) {
                continue;
            }
            const double x = radius * std::cos(t);
            const double y = radius * std::sin(t);
            if (!found) {
                axes.plotXMin = axes.plotXMax = x;
                axes.plotYMin = axes.plotYMax = y;
                found = true;
            } else {
                axes.plotXMin = std::min(axes.plotXMin, x);
                axes.plotXMax = std::max(axes.plotXMax, x);
                axes.plotYMin = std::min(axes.plotYMin, y);
                axes.plotYMax = std::max(axes.plotYMax, y);
            }
        }
        axes.plotAxesValid = found;
    }
    return axes;
}

QStringList AsirWorker::splitTopLevelArguments(const QString &arguments)
{
    QStringList parts;
    QString current;
    int parenDepth = 0;
    int bracketDepth = 0;
    bool inString = false;

    for (int i = 0; i < arguments.size(); ++i) {
        const QChar ch = arguments.at(i);
        if (ch == QLatin1Char('"') && (i == 0 || arguments.at(i - 1) != QLatin1Char('\\'))) {
            inString = !inString;
        }

        if (!inString) {
            if (ch == QLatin1Char('(')) {
                ++parenDepth;
            } else if (ch == QLatin1Char(')')) {
                --parenDepth;
            } else if (ch == QLatin1Char('[')) {
                ++bracketDepth;
            } else if (ch == QLatin1Char(']')) {
                --bracketDepth;
            } else if (ch == QLatin1Char(',') && parenDepth == 0 && bracketDepth == 0) {
                parts.append(current.trimmed());
                current.clear();
                continue;
            }
        }

        current.append(ch);
    }

    if (!current.trimmed().isEmpty()) {
        parts.append(current.trimmed());
    }
    return parts;
}

bool AsirWorker::isGeometryArgument(const QString &argument)
{
    const QString trimmed = argument.trimmed();
    if (!trimmed.startsWith(QLatin1Char('[')) || !trimmed.endsWith(QLatin1Char(']'))) {
        return false;
    }

    const QStringList parts = splitTopLevelArguments(trimmed.mid(1, trimmed.size() - 2));
    if (parts.size() != 2) {
        return false;
    }

    static const QRegularExpression integerPattern(QStringLiteral("^[+-]?\\d+$"));
    return integerPattern.match(parts.at(0).trimmed()).hasMatch()
        && integerPattern.match(parts.at(1).trimmed()).hasMatch();
}

bool AsirWorker::isVariableRangeArgument(const QString &argument)
{
    const QString trimmed = argument.trimmed();
    if (!trimmed.startsWith(QLatin1Char('[')) || !trimmed.endsWith(QLatin1Char(']'))) {
        return false;
    }
    const QStringList parts = splitTopLevelArguments(trimmed.mid(1, trimmed.size() - 2));
    if (parts.size() < 3) {
        return false;
    }
    static const QRegularExpression variablePattern(QStringLiteral("^[A-Za-z][A-Za-z0-9_]*$"));
    return variablePattern.match(parts.constFirst().trimmed()).hasMatch();
}

QStringList AsirWorker::extractVariableNames(const QString &expression)
{
    static const QSet<QString> blocked = {
        QStringLiteral("sin"), QStringLiteral("cos"), QStringLiteral("tan"),
        QStringLiteral("asin"), QStringLiteral("acos"), QStringLiteral("atan"),
        QStringLiteral("exp"), QStringLiteral("log"), QStringLiteral("sqrt"),
        QStringLiteral("abs"), QStringLiteral("pi"), QStringLiteral("e"),
        QStringLiteral("floor"), QStringLiteral("ceil"), QStringLiteral("round")
    };

    QStringList names;
    static const QRegularExpression tokenPattern(QStringLiteral("\\b([A-Za-z][A-Za-z0-9_]*)\\b"));
    QRegularExpressionMatchIterator it = tokenPattern.globalMatch(expression);
    while (it.hasNext()) {
        const QString candidate = it.next().captured(1);
        const QString lowered = candidate.toLower();
        if (blocked.contains(lowered) || names.contains(candidate)) {
            continue;
        }
        names.append(candidate);
        if (names.size() == 2) {
            break;
        }
    }
    return names;
}

QString AsirWorker::normalizedPlotArguments(const QString &functionName, const QString &arguments)
{
    QStringList parts = splitTopLevelArguments(arguments);
    if (parts.isEmpty()) {
        return arguments;
    }

    int geometryIndex = -1;
    for (int i = 1; i < parts.size(); ++i) {
        if (isGeometryArgument(parts.at(i))) {
            geometryIndex = i;
            break;
        }
    }
    const bool hasGeometry = geometryIndex >= 0;
    const QString geometry = hasGeometry ? parts.takeAt(geometryIndex) : QString();

    int variableRangeCount = 0;
    for (int i = 1; i < parts.size(); ++i) {
        if (isVariableRangeArgument(parts.at(i))) {
            ++variableRangeCount;
        }
    }

    if (functionName == QStringLiteral("plot") && variableRangeCount == 0) {
        QStringList variableNames = extractVariableNames(parts.first());
        const QString variable = variableNames.isEmpty() ? QStringLiteral("x") : variableNames.constFirst();
        QStringList normalized;
        normalized.append(parts.first());
        normalized.append(QStringLiteral("[%1,-2,2]").arg(variable));
        if (hasGeometry) {
            normalized.append(geometry);
        } else {
            normalized.append(QStringLiteral("[%1,%1]").arg(kDefaultPlotSize));
        }
        parts = normalized;
    }
    if ((functionName == QStringLiteral("ifplot") || functionName == QStringLiteral("conplot"))
        && variableRangeCount < 2) {
        QStringList variableNames = extractVariableNames(parts.first());
        if (variableNames.size() < 2) {
            variableNames = {QStringLiteral("x"), QStringLiteral("y")};
        }

        QSet<QString> existingVariables;
        for (int i = 1; i < parts.size(); ++i) {
            const QString argument = parts.at(i).trimmed();
            if (!isVariableRangeArgument(argument)) {
                continue;
            }
            const QStringList rangeParts = splitTopLevelArguments(argument.mid(1, argument.size() - 2));
            if (!rangeParts.isEmpty()) {
                existingVariables.insert(rangeParts.constFirst().trimmed());
            }
        }

        for (const QString &variable : variableNames) {
            if (existingVariables.contains(variable)) {
                continue;
            }
            parts.append(QStringLiteral("[%1,-2,2]").arg(variable));
            ++variableRangeCount;
            if (variableRangeCount >= 2) {
                break;
            }
        }
    }

    if (functionName == QStringLiteral("polarplot") && variableRangeCount == 0) {
        QStringList variableNames = extractVariableNames(parts.first());
        const QString variable = variableNames.isEmpty() ? QStringLiteral("t") : variableNames.constFirst();
        parts.append(QStringLiteral("[%1,0,6.283185307179586]").arg(variable));
    }

    if (parts.isEmpty() || !isGeometryArgument(parts.constLast())) {
        parts.append(hasGeometry ? geometry : QStringLiteral("[%1,%1]").arg(kDefaultPlotSize));
    }
    return parts.join(QStringLiteral(","));
}

bool AsirWorker::parseDoubleText(const QString &text, double *value)
{
    static const QRegularExpression numberPattern(
        QStringLiteral("([+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)"));
    const QRegularExpressionMatch match = numberPattern.match(text);
    if (!match.hasMatch()) {
        return false;
    }
    bool ok = false;
    const double parsed = match.captured(1).toDouble(&ok);
    if (!ok || !std::isfinite(parsed)) {
        return false;
    }
    *value = parsed;
    return true;
}

QString AsirWorker::formatAsirDouble(double value)
{
    if (!std::isfinite(value)) {
        return QStringLiteral("0.0");
    }
    QString text = QString::number(value, 'g', 17);
    if (!text.contains(QLatin1Char('.')) && !text.contains(QLatin1Char('e')) && !text.contains(QLatin1Char('E'))) {
        text += QStringLiteral(".0");
    }
    return text;
}

bool AsirWorker::evaluateNumericExpression(const QString &expression,
    const QList<QPair<QString, double>> &substitutions, double *value)
{
    QString substituted = QStringLiteral("(%1)").arg(expression);
    for (const auto &substitution : substitutions) {
        substituted = QStringLiteral("subst(%1,%2,%3)")
            .arg(substituted, substitution.first, formatAsirDouble(substitution.second));
    }

    const QString text = executeToString(QStringLiteral("deval(%1);").arg(substituted));
    if (text.isEmpty() || looksLikeAsirError(text)) {
        return false;
    }
    return parseDoubleText(text, value);
}

AsirWorker::PlotExecutionResult AsirWorker::renderFunctionPlot(const PlotCommand &plotCommand)
{
    if (plotCommand.arguments.size() < 2) {
        return {QStringLiteral("plot の引数を解釈できませんでした。"), QString(), QImage(), false};
    }

    const QString expression = plotCommand.arguments.at(0);
    QString rangeArgument;
    QString geometryArgument;
    for (int i = 1; i < plotCommand.arguments.size(); ++i) {
        const QString argument = plotCommand.arguments.at(i).trimmed();
        if (geometryArgument.isEmpty() && isGeometryArgument(argument)) {
            geometryArgument = argument;
        } else if (rangeArgument.isEmpty() && isVariableRangeArgument(argument)) {
            rangeArgument = argument;
        }
    }

    const QStringList rangeParts = rangeArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(rangeArgument.mid(1, rangeArgument.size() - 2));
    const QStringList geometryParts = geometryArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(geometryArgument.mid(1, geometryArgument.size() - 2));
    if (rangeParts.size() < 3 || geometryParts.size() != 2) {
        return {QStringLiteral("plot の範囲またはサイズが不正です。"), QString(), QImage(), false};
    }

    const QString variable = rangeParts.at(0).trimmed();
    double xmin = 0.0;
    double xmax = 0.0;
    if (!evaluateNumericExpression(rangeParts.at(1), {}, &xmin)
        || !evaluateNumericExpression(rangeParts.at(2), {}, &xmax)) {
        return {QStringLiteral("plot の x 範囲を数値化できませんでした。"), QString(), QImage(), false};
    }

    bool okWidth = false;
    bool okHeight = false;
    const int width = geometryParts.at(0).trimmed().toInt(&okWidth);
    const int height = geometryParts.at(1).trimmed().toInt(&okHeight);
    if (!okWidth || !okHeight || width < 2 || height < 2) {
        return {QStringLiteral("plot の画像サイズが不正です。"), QString(), QImage(), false};
    }

    QVector<double> values(width);
    double ymin = 0.0;
    double ymax = 0.0;
    bool foundValue = false;
    const double xstep = width > 1 ? (xmax - xmin) / static_cast<double>(width - 1) : 0.0;
    for (int ix = 0; ix < width; ++ix) {
        if (isCancelled()) return {};
        const double x = xmin + xstep * static_cast<double>(ix);
        double y = 0.0;
        if (!evaluateNumericExpression(expression, {{variable, x}}, &y)) {
            values[ix] = std::numeric_limits<double>::quiet_NaN();
            continue;
        }
        values[ix] = y;
        if (!foundValue) {
            ymin = ymax = y;
            foundValue = true;
        } else {
            ymin = std::min(ymin, y);
            ymax = std::max(ymax, y);
        }
    }

    if (!foundValue) {
        return {QStringLiteral("plot の数値評価に失敗しました。"), QString(), QImage(), false};
    }
    if (qFuzzyCompare(ymin, ymax)) {
        ymin -= 1.0;
        ymax += 1.0;
    }

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(0, 0, 0, 255), 1));

    QPointF previous;
    bool hasPrevious = false;
    for (int ix = 0; ix < width; ++ix) {
        const double y = values.at(ix);
        if (!std::isfinite(y)) {
            hasPrevious = false;
            continue;
        }
        const double normalized = (ymax - y) / (ymax - ymin);
        const QPointF current(ix, normalized * static_cast<double>(height - 1));
        if (hasPrevious) {
            painter.drawLine(previous, current);
        }
        previous = current;
        hasPrevious = true;
    }
    painter.end();

    const QString summary = buildPlotSummary(plotCommand, width, height);
    PlotExecutionResult result;
    result.text = summary;
    result.summary = summary;
    result.image = image;
    result.plotAxesValid = true;
    result.plotXMin = xmin;
    result.plotXMax = xmax;
    result.plotYMin = ymin;
    result.plotYMax = ymax;
    result.valid = true;
    return result;
}

AsirWorker::PlotExecutionResult AsirWorker::renderImplicitPlot(const PlotCommand &plotCommand, bool contour)
{
    if (plotCommand.arguments.size() < 2) {
        return {QStringLiteral("%1 の引数を解釈できませんでした。")
                    .arg(contour ? QStringLiteral("conplot") : QStringLiteral("ifplot")),
            QString(), QImage(), false};
    }

    const QString expression = plotCommand.arguments.at(0);
    QStringList rangeArguments;
    QString geometryArgument;
    QString levelArgument;
    for (int i = 1; i < plotCommand.arguments.size(); ++i) {
        const QString argument = plotCommand.arguments.at(i).trimmed();
        if (geometryArgument.isEmpty() && isGeometryArgument(argument)) {
            geometryArgument = argument;
        } else if (isVariableRangeArgument(argument)) {
            rangeArguments.append(argument);
        } else if (levelArgument.isEmpty() && argument.startsWith(QLatin1Char('[')) && argument.endsWith(QLatin1Char(']'))) {
            levelArgument = argument;
        }
    }

    const QString xrangeArgument = rangeArguments.value(0);
    const QString yrangeArgument = rangeArguments.value(1);

    const QStringList xrangeParts = xrangeArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(xrangeArgument.mid(1, xrangeArgument.size() - 2));
    const QStringList yrangeParts = yrangeArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(yrangeArgument.mid(1, yrangeArgument.size() - 2));
    const QStringList geometryParts = geometryArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(geometryArgument.mid(1, geometryArgument.size() - 2));
    if (xrangeParts.size() < 3 || yrangeParts.size() < 3 || geometryParts.size() != 2) {
        return {QStringLiteral("%1 の範囲またはサイズが不正です。")
                    .arg(contour ? QStringLiteral("conplot") : QStringLiteral("ifplot")),
            QString(), QImage(), false};
    }

    const QString xVariable = xrangeParts.at(0).trimmed();
    const QString yVariable = yrangeParts.at(0).trimmed();
    double xmin = 0.0;
    double xmax = 0.0;
    double ymin = 0.0;
    double ymax = 0.0;
    if (!evaluateNumericExpression(xrangeParts.at(1), {}, &xmin)
        || !evaluateNumericExpression(xrangeParts.at(2), {}, &xmax)
        || !evaluateNumericExpression(yrangeParts.at(1), {}, &ymin)
        || !evaluateNumericExpression(yrangeParts.at(2), {}, &ymax)) {
        return {QStringLiteral("%1 の描画範囲を数値化できませんでした。")
                    .arg(contour ? QStringLiteral("conplot") : QStringLiteral("ifplot")),
            QString(), QImage(), false};
    }

    bool okWidth = false;
    bool okHeight = false;
    const int width = geometryParts.at(0).trimmed().toInt(&okWidth);
    const int height = geometryParts.at(1).trimmed().toInt(&okHeight);
    if (!okWidth || !okHeight || width < 2 || height < 2) {
        return {QStringLiteral("%1 の画像サイズが不正です。")
                    .arg(contour ? QStringLiteral("conplot") : QStringLiteral("ifplot")),
            QString(), QImage(), false};
    }

    QVector<double> values(width * height, 0.0);
    double vmin = 0.0;
    double vmax = 0.0;
    bool foundValue = false;
    const double xstep = width > 1 ? (xmax - xmin) / static_cast<double>(width - 1) : 0.0;
    const double ystep = height > 1 ? (ymax - ymin) / static_cast<double>(height - 1) : 0.0;
    for (int ix = 0; ix < width; ++ix) {
        if (isCancelled()) return {};
        const double x = xmin + xstep * static_cast<double>(ix);
        for (int iy = 0; iy < height; ++iy) {
            const double y = ymin + ystep * static_cast<double>(iy);
            double sample = 0.0;
            if (!evaluateNumericExpression(expression, {{xVariable, x}, {yVariable, y}}, &sample)) {
                sample = 0.0;
            }
            values[ix * height + iy] = sample;
            if (!foundValue) {
                vmin = vmax = sample;
                foundValue = true;
            } else {
                vmin = std::min(vmin, sample);
                vmax = std::max(vmax, sample);
            }
        }
    }

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    auto drawLevel = [&](double level) {
        for (int ix = 0; ix < width - 1; ++ix) {
            for (int iy = 0; iy < height - 1; ++iy) {
                const double p = values[ix * height + iy];
                const double p1 = values[(ix + 1) * height + iy];
                const double p2 = values[ix * height + (iy + 1)];
                if (((p >= level) && ((p1 <= level) || (p2 <= level)))
                    || ((p <= level) && ((p1 >= level) || (p2 >= level)))) {
                    image.setPixelColor(ix, height - iy - 1, QColor(0, 0, 0, 255));
                }
            }
        }
    };

    if (contour) {
        double zmin = -2.0;
        double zmax = 2.0;
        int steps = 8;
        if (!levelArgument.isEmpty()) {
            const QStringList parts = splitTopLevelArguments(levelArgument.mid(1, levelArgument.size() - 2));
            if (parts.size() >= 3) {
                evaluateNumericExpression(parts.at(1), {}, &zmin);
                evaluateNumericExpression(parts.at(2), {}, &zmax);
                if (parts.size() >= 4) {
                    bool ok = false;
                    const int parsed = parts.at(3).trimmed().toInt(&ok);
                    if (ok && parsed > 0) {
                        steps = parsed;
                    }
                }
            }
        }
        if (qFuzzyCompare(zmin, zmax)) {
            zmin = vmin;
            zmax = vmax;
        }
        steps = qMax(1, steps);
        for (int i = 0; i <= steps; ++i) {
            const double ratio = steps == 0 ? 0.0 : static_cast<double>(i) / static_cast<double>(steps);
            drawLevel(zmin + (zmax - zmin) * ratio);
        }
    } else {
        drawLevel(0.0);
    }

    const QString summary = buildPlotSummary(plotCommand, width, height);
    PlotExecutionResult result;
    result.text = summary;
    result.summary = summary;
    result.image = image;
    result.plotAxesValid = true;
    result.plotXMin = xmin;
    result.plotXMax = xmax;
    result.plotYMin = ymin;
    result.plotYMax = ymax;
    result.valid = true;
    return result;
}

AsirWorker::PlotExecutionResult AsirWorker::renderPolarPlot(const PlotCommand &plotCommand)
{
    if (plotCommand.arguments.size() < 2) {
        return {QStringLiteral("polarplot の引数を解釈できませんでした。"), QString(), QImage(), false};
    }

    const QString expression = plotCommand.arguments.at(0);
    QString rangeArgument;
    QString geometryArgument;
    for (int i = 1; i < plotCommand.arguments.size(); ++i) {
        const QString argument = plotCommand.arguments.at(i).trimmed();
        if (geometryArgument.isEmpty() && isGeometryArgument(argument)) {
            geometryArgument = argument;
        } else if (rangeArgument.isEmpty() && isVariableRangeArgument(argument)) {
            rangeArgument = argument;
        }
    }
    const QStringList rangeParts = rangeArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(rangeArgument.mid(1, rangeArgument.size() - 2));
    const QStringList geometryParts = geometryArgument.isEmpty()
        ? QStringList()
        : splitTopLevelArguments(geometryArgument.mid(1, geometryArgument.size() - 2));
    if (rangeParts.size() < 3 || geometryParts.size() != 2) {
        return {QStringLiteral("polarplot の範囲またはサイズが不正です。"), QString(), QImage(), false};
    }

    const QString variable = rangeParts.at(0).trimmed();
    double tmin = 0.0;
    double tmax = 0.0;
    if (!evaluateNumericExpression(rangeParts.at(1), {}, &tmin)
        || !evaluateNumericExpression(rangeParts.at(2), {}, &tmax)) {
        return {QStringLiteral("polarplot の範囲を数値化できませんでした。"), QString(), QImage(), false};
    }

    bool okWidth = false;
    bool okHeight = false;
    const int width = geometryParts.at(0).trimmed().toInt(&okWidth);
    const int height = geometryParts.at(1).trimmed().toInt(&okHeight);
    if (!okWidth || !okHeight || width < 2 || height < 2) {
        return {QStringLiteral("polarplot の画像サイズが不正です。"), QString(), QImage(), false};
    }

    const int samples = qMax(width, 180);
    QVector<QPointF> points;
    points.reserve(samples);
    double xmin = 0.0;
    double xmax = 0.0;
    double ymin = 0.0;
    double ymax = 0.0;
    bool foundValue = false;
    const double tstep = samples > 1 ? (tmax - tmin) / static_cast<double>(samples - 1) : 0.0;
    for (int i = 0; i < samples; ++i) {
        if (isCancelled()) return {};
        const double t = tmin + tstep * static_cast<double>(i);
        double radius = 0.0;
        if (!evaluateNumericExpression(expression, {{variable, t}}, &radius)) {
            continue;
        }
        const QPointF point(radius * std::cos(t), radius * std::sin(t));
        points.append(point);
        if (!foundValue) {
            xmin = xmax = point.x();
            ymin = ymax = point.y();
            foundValue = true;
        } else {
            xmin = std::min(xmin, point.x());
            xmax = std::max(xmax, point.x());
            ymin = std::min(ymin, point.y());
            ymax = std::max(ymax, point.y());
        }
    }

    if (!foundValue) {
        return {QStringLiteral("polarplot の数値評価に失敗しました。"), QString(), QImage(), false};
    }
    if (qFuzzyCompare(xmin, xmax)) {
        xmin -= 1.0;
        xmax += 1.0;
    }
    if (qFuzzyCompare(ymin, ymax)) {
        ymin -= 1.0;
        ymax += 1.0;
    }

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(0, 0, 0, 255), 1));

    auto mapPoint = [&](const QPointF &point) {
        const double x = (point.x() - xmin) / (xmax - xmin) * static_cast<double>(width - 1);
        const double y = (ymax - point.y()) / (ymax - ymin) * static_cast<double>(height - 1);
        return QPointF(x, y);
    };

    for (int i = 1; i < points.size(); ++i) {
        painter.drawLine(mapPoint(points.at(i - 1)), mapPoint(points.at(i)));
    }
    painter.end();

    const QString summary = buildPlotSummary(plotCommand, width, height);
    PlotExecutionResult result;
    result.text = summary;
    result.summary = summary;
    result.image = image;
    result.plotAxesValid = true;
    result.plotXMin = xmin;
    result.plotXMax = xmax;
    result.plotYMin = ymin;
    result.plotYMax = ymax;
    result.valid = true;
    return result;
}

QString AsirWorker::extractLatexSource(const QString &command) const
{
    const QString stripped = stripCommandTerminator(command).trimmed();

    const QRegularExpressionMatch match = kWrappedPattern.match(stripped);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return stripped;
}

QString AsirWorker::normalizeCommand(const QString &command) const
{
    QString normalized = command.trimmed();
    if (normalized.isEmpty()) {
        return {};
    }

    const QChar last = normalized.back();
    if (last != ';' && last != '$') {
        normalized += ';';
    }
    return normalized;
}

QString AsirWorker::stripCommandTerminator(const QString &command) const
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
    return trimmed;
}

LocalAsirEngine::LocalAsirEngine(const AppConfig &config, QObject *parent)
    : AsirEngine(parent)
    , m_config(config)
{
}

LocalAsirEngine::~LocalAsirEngine()
{
    shutdown();
}

bool LocalAsirEngine::initialize()
{
    if (m_ready) {
        return true;
    }

    qRegisterMetaType<AsirResult>("AsirResult");

    m_worker = new AsirWorker(m_config);
    m_worker->moveToThread(&m_workerThread);
    connect(m_worker, &AsirWorker::initialized, this, [this]() {
        m_ready = true;
        emit engineReady();
    });
    connect(m_worker, &AsirWorker::initFailed, this, [this](const QString &message) {
        m_ready = false;
        emit errorOccurred(message);
    });
    connect(m_worker, &AsirWorker::resultReady, this, &LocalAsirEngine::resultReady);
    connect(m_worker, &AsirWorker::errorOccurred, this, &LocalAsirEngine::errorOccurred);

    QEventLoop loop;
    bool success = false;
    bool finished = false;

    connect(m_worker, &AsirWorker::initialized, &loop, [&]() {
        success = true;
        finished = true;
        loop.quit();
    });
    connect(m_worker, &AsirWorker::initFailed, &loop, [&](const QString &) {
        finished = true;
        loop.quit();
    });

    m_workerThread.start();
    QMetaObject::invokeMethod(m_worker, &AsirWorker::init, Qt::QueuedConnection);

    if (!finished) {
        loop.exec();
    }
    return success;
}

void LocalAsirEngine::shutdown()
{
    if (!m_worker) {
        return;
    }

    m_ready = false;
    const bool workerExecuting = m_workerThread.isRunning() && m_worker->isExecuting();
    if (workerExecuting) {
        m_worker->requestCancel();
        m_worker->requestInterrupt();
        sendInterruptSignalToThread(m_worker->nativeThreadId());
    }

    if (m_workerThread.isRunning()) {
        if (!workerExecuting) {
            QMetaObject::invokeMethod(m_worker, &AsirWorker::shutdownWorker, Qt::BlockingQueuedConnection);
        } else {
            QMetaObject::invokeMethod(m_worker, &AsirWorker::shutdownWorker, Qt::QueuedConnection);
        }

        m_workerThread.quit();
        if (!m_workerThread.wait(3000)) {
            m_workerThread.quit();
        }
        if (m_workerThread.isRunning() && !m_workerThread.wait(1500)) {
            m_workerThread.terminate();
            m_workerThread.wait(1500);
        }
    }

    if (!m_workerThread.isRunning()) {
        delete m_worker;
    }
    m_worker = nullptr;
}

void LocalAsirEngine::executeString(const QString &command, bool producePresentation)
{
    if (!m_worker || !m_ready) {
        emit errorOccurred(QStringLiteral("Asir エンジンの準備が完了していません。"));
        return;
    }
    QMetaObject::invokeMethod(m_worker, "execute", Qt::QueuedConnection,
        Q_ARG(QString, command), Q_ARG(bool, producePresentation));
}

void LocalAsirEngine::interruptCurrent()
{
    if (!m_worker || !m_ready || !m_worker->isExecuting()) {
        return;
    }
    m_worker->requestInterrupt();
    sendInterruptSignalToThread(m_worker->nativeThreadId());
}

bool LocalAsirEngine::isReady() const
{
    return m_ready;
}
