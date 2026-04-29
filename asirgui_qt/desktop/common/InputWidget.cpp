#include "desktop/common/InputWidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QSet>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextOption>

namespace {

const QString kIndentUnit = QString(4, QLatin1Char(' '));
const QRegularExpression kIdentifierAtEndPattern(QStringLiteral("([A-Za-z_][A-Za-z0-9_]*)$"));
const QRegularExpression kIdentifierPattern(QStringLiteral("\\b([A-Za-z_][A-Za-z0-9_]*)\\b"));
const QStringList kAsirBuiltins = {
    QStringLiteral("abs"),
    QStringLiteral("append"),
    QStringLiteral("car"),
    QStringLiteral("cdr"),
    QStringLiteral("clear_canvas"),
    QStringLiteral("coeff"),
    QStringLiteral("conplot"),
    QStringLiteral("def"),
    QStringLiteral("deval"),
    QStringLiteral("draw_obj"),
    QStringLiteral("draw_string"),
    QStringLiteral("fctr"),
    QStringLiteral("for"),
    QStringLiteral("gb"),
    QStringLiteral("gr"),
    QStringLiteral("if"),
    QStringLiteral("ifplot"),
    QStringLiteral("import"),
    QStringLiteral("load"),
    QStringLiteral("map"),
    QStringLiteral("nd_f4"),
    QStringLiteral("nd_gr"),
    QStringLiteral("open_canvas"),
    QStringLiteral("plot"),
    QStringLiteral("polarplot"),
    QStringLiteral("print"),
    QStringLiteral("print_tex_form"),
    QStringLiteral("quit"),
    QStringLiteral("return"),
    QStringLiteral("subst"),
    QStringLiteral("vars"),
    QStringLiteral("while")
};

QString longestCommonPrefix(const QStringList &values)
{
    if (values.isEmpty()) {
        return {};
    }

    QString prefix = values.constFirst();
    for (int i = 1; i < values.size() && !prefix.isEmpty(); ++i) {
        const QString &candidate = values.at(i);
        int length = 0;
        const int limit = qMin(prefix.size(), candidate.size());
        while (length < limit && prefix.at(length) == candidate.at(length)) {
            ++length;
        }
        prefix.truncate(length);
    }
    return prefix;
}

bool isModifierOnlyKey(int key)
{
    return key == Qt::Key_Control
        || key == Qt::Key_Shift
        || key == Qt::Key_Alt
        || key == Qt::Key_Meta;
}

}

InputWidget::InputWidget(CommandHistory *history, QWidget *parent)
    : QPlainTextEdit(parent)
    , m_history(history)
{
    setPlaceholderText(QStringLiteral("Asir コマンドエディタ\nEnter: 改行   Shift+Enter: 実行"));
    setTabChangesFocus(false);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setMinimumHeight(fontMetrics().lineSpacing() * 4 + 20);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setTabStopDistance(fontMetrics().horizontalAdvance(kIndentUnit));
}

void InputWidget::applyEditorFont(const QFont &font)
{
    setFont(font);
    setMinimumHeight(fontMetrics().lineSpacing() * 4 + 20);
}

void InputWidget::keyPressEvent(QKeyEvent *event)
{
    const Qt::KeyboardModifiers mods = event->modifiers();
    QTextCursor cursor = textCursor();
    const bool isHistoryModeKey =
        (mods == Qt::NoModifier && (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down))
        || (mods == Qt::ControlModifier && (event->key() == Qt::Key_P || event->key() == Qt::Key_N));
    const bool isBraceRightInput =
        !mods.testFlag(Qt::ControlModifier)
        && !mods.testFlag(Qt::AltModifier)
        && !mods.testFlag(Qt::MetaModifier)
        && event->text() == QStringLiteral("}");

    if (!isHistoryModeKey && !isModifierOnlyKey(event->key())) {
        setHistoryNavigationMode(false);
    }

    if (isBraceRightInput && handleClosingBraceIndent()) {
        return;
    }

    if (mods == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_C:
            if (m_interruptEnabled) {
                emit interruptRequested();
                return;
            }
            break;
        case Qt::Key_A:
            cursor.movePosition(QTextCursor::StartOfLine);
            setTextCursor(cursor);
            return;
        case Qt::Key_E:
            cursor.movePosition(QTextCursor::EndOfLine);
            setTextCursor(cursor);
            return;
        case Qt::Key_B:
            cursor.movePosition(QTextCursor::Left);
            setTextCursor(cursor);
            return;
        case Qt::Key_F:
            cursor.movePosition(QTextCursor::Right);
            setTextCursor(cursor);
            return;
        case Qt::Key_D:
            cursor.deleteChar();
            setTextCursor(cursor);
            return;
        case Qt::Key_H:
        case Qt::Key_Backspace:
            cursor.deletePreviousChar();
            setTextCursor(cursor);
            return;
        case Qt::Key_K:
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setTextCursor(cursor);
            return;
        case Qt::Key_U:
            deleteToLineStart();
            return;
        case Qt::Key_W:
            if (!deletePreviousWord()) {
                QApplication::beep();
            }
            return;
        case Qt::Key_P:
            if (!m_historyNavigationMode) {
                setHistoryNavigationMode(true);
            }
            if (!moveHistoryPrevious()) {
                QApplication::beep();
            }
            return;
        case Qt::Key_N:
            if (!m_historyNavigationMode) {
                setHistoryNavigationMode(true);
            }
            if (!moveHistoryNext()) {
                QApplication::beep();
            }
            return;
        default:
            break;
        }
    }

    if (mods == Qt::AltModifier) {
        switch (event->key()) {
        case Qt::Key_B:
            if (!moveCursorToPreviousWord()) {
                QApplication::beep();
            }
            return;
        case Qt::Key_F:
            if (!moveCursorToNextWord()) {
                QApplication::beep();
            }
            return;
        case Qt::Key_Backspace:
            if (!deletePreviousWord()) {
                QApplication::beep();
            }
            return;
        default:
            break;
        }
    }

    if (mods == Qt::ShiftModifier) {
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            submitCurrentCommand();
            return;
        default:
            break;
        }
    }

    if (mods == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_Up:
            if (m_historyNavigationMode) {
                if (!moveHistoryPrevious()) {
                    QApplication::beep();
                }
                return;
            }
            if (toPlainText().trimmed().isEmpty()) {
                setHistoryNavigationMode(true);
                if (!moveHistoryPrevious()) {
                    setHistoryNavigationMode(false);
                } else {
                    return;
                }
            }
            break;
        case Qt::Key_Down:
            if (m_historyNavigationMode) {
                if (!moveHistoryNext()) {
                    QApplication::beep();
                }
                return;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            insertIndentedNewline();
            return;
        case Qt::Key_Tab:
            if (!completeIdentifier()) {
                textCursor().insertText(kIndentUnit);
            }
            return;
        default:
            break;
        }
    }

    QPlainTextEdit::keyPressEvent(event);
}

void InputWidget::submitCurrentCommand()
{
    const QString command = toPlainText().trimmed();
    if (command.isEmpty()) {
        return;
    }

    bool previewOnly = false;
    bool ok = false;
    QString historyPrefix;
    int matchedIndex = -1;
    const QString expanded = expandHistoryDirective(command, &previewOnly, &ok, &historyPrefix, &matchedIndex);
    if (!ok) {
        QApplication::beep();
        return;
    }
    if (previewOnly) {
        setCurrentCommand(expanded);
        setHistoryNavigationMode(true, historyPrefix, matchedIndex);
        return;
    }

    emit commandSubmitted(expanded);
    clear();
    setHistoryNavigationMode(false);
}

void InputWidget::setCurrentCommand(const QString &command)
{
    setPlainText(command);
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void InputWidget::insertIndentedNewline()
{
    QTextCursor cursor = textCursor();
    const QString lineText = cursor.block().text();
    const QString trimmed = lineText.trimmed();
    QString indentation = currentLineIndentation();

    if (trimmed.endsWith(QLatin1Char('{'))) {
        indentation += kIndentUnit;
    }

    cursor.insertText(QStringLiteral("\n") + indentation);
    setTextCursor(cursor);
    ensureCursorVisible();
}

bool InputWidget::handleClosingBraceIndent()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        return false;
    }

    const QString lineText = cursor.block().text();
    const QString left = lineText.left(cursor.positionInBlock());
    if (!left.trimmed().isEmpty()) {
        return false;
    }

    int removeCount = 0;
    if (left.endsWith(kIndentUnit)) {
        removeCount = 4;
    } else {
        while (removeCount < left.size()
            && left.at(left.size() - removeCount - 1) == QLatin1Char(' ')) {
            ++removeCount;
        }
        removeCount = qMin(removeCount, 4);
    }

    for (int i = 0; i < removeCount; ++i) {
        cursor.deletePreviousChar();
    }

    cursor.insertText(QStringLiteral("}"));
    setTextCursor(cursor);
    return true;
}

QString InputWidget::currentLineIndentation() const
{
    const QString text = textCursor().block().text();
    int i = 0;
    while (i < text.size() && text.at(i) == QLatin1Char(' ')) {
        ++i;
    }
    return text.left(i);
}

bool InputWidget::moveHistoryPrevious()
{
    if (!m_history) {
        return false;
    }

    const QStringList &entries = m_history->entries();
    if (entries.isEmpty()) {
        return false;
    }

    int index = m_historyBrowseIndex < 0 ? entries.size() - 1 : m_historyBrowseIndex - 1;
    while (index >= 0) {
        if (m_historyFilterPrefix.isEmpty() || entries.at(index).startsWith(m_historyFilterPrefix)) {
            m_historyBrowseIndex = index;
            setCurrentCommand(entries.at(index));
            return true;
        }
        --index;
    }
    return false;
}

bool InputWidget::moveHistoryNext()
{
    if (!m_history) {
        return false;
    }

    const QStringList &entries = m_history->entries();
    if (entries.isEmpty()) {
        return false;
    }

    if (m_historyBrowseIndex < 0) {
        return false;
    }

    int index = m_historyBrowseIndex + 1;
    while (index < entries.size()) {
        if (m_historyFilterPrefix.isEmpty() || entries.at(index).startsWith(m_historyFilterPrefix)) {
            m_historyBrowseIndex = index;
            setCurrentCommand(entries.at(index));
            return true;
        }
        ++index;
    }

    m_historyBrowseIndex = -1;
    setCurrentCommand(QString());
    return true;
}

void InputWidget::setHistoryNavigationMode(bool enabled, const QString &filterPrefix, int browseIndex)
{
    m_historyNavigationMode = enabled;
    m_historyFilterPrefix = enabled ? filterPrefix : QString();
    m_historyBrowseIndex = enabled ? browseIndex : -1;
}

bool InputWidget::deleteToLineStart()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    if (!cursor.hasSelection()) {
        return false;
    }
    cursor.removeSelectedText();
    setTextCursor(cursor);
    return true;
}

bool InputWidget::deletePreviousWord()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
        setTextCursor(cursor);
        return true;
    }
    if (cursor.atStart()) {
        return false;
    }

    const int originalPosition = cursor.position();
    cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
    if (!cursor.hasSelection() || cursor.position() == originalPosition) {
        return false;
    }

    cursor.removeSelectedText();
    setTextCursor(cursor);
    return true;
}

bool InputWidget::moveCursorToPreviousWord()
{
    QTextCursor cursor = textCursor();
    const int originalPosition = cursor.position();
    cursor.movePosition(QTextCursor::PreviousWord);
    if (cursor.position() == originalPosition) {
        return false;
    }
    setTextCursor(cursor);
    return true;
}

bool InputWidget::moveCursorToNextWord()
{
    QTextCursor cursor = textCursor();
    const int originalPosition = cursor.position();
    cursor.movePosition(QTextCursor::NextWord);
    if (cursor.position() == originalPosition) {
        return false;
    }
    setTextCursor(cursor);
    return true;
}

bool InputWidget::completeIdentifier()
{
    int startPosition = -1;
    const QString prefix = identifierPrefixAtCursor(&startPosition);
    if (prefix.isEmpty()) {
        return false;
    }

    QStringList matches;
    const QStringList candidates = completionCandidates();
    for (const QString &candidate : candidates) {
        if (candidate.startsWith(prefix) && candidate != prefix) {
            matches.append(candidate);
        }
    }

    if (matches.isEmpty()) {
        QApplication::beep();
        return true;
    }

    matches.sort();
    const QString replacement = matches.size() == 1 ? matches.constFirst() : longestCommonPrefix(matches);
    if (replacement.size() <= prefix.size()) {
        QApplication::beep();
        return true;
    }

    QTextCursor cursor = textCursor();
    cursor.setPosition(startPosition);
    cursor.setPosition(startPosition + prefix.size(), QTextCursor::KeepAnchor);
    cursor.insertText(replacement);
    setTextCursor(cursor);
    return true;
}

QString InputWidget::identifierPrefixAtCursor(int *startPosition) const
{
    QTextCursor cursor = textCursor();
    const QString left = cursor.block().text().left(cursor.positionInBlock());
    const QRegularExpressionMatch match = kIdentifierAtEndPattern.match(left);
    if (!match.hasMatch()) {
        if (startPosition) {
            *startPosition = -1;
        }
        return {};
    }
    if (startPosition) {
        *startPosition = cursor.block().position() + match.capturedStart(1);
    }
    return match.captured(1);
}

QStringList InputWidget::completionCandidates() const
{
    QSet<QString> names;
    names.reserve(kAsirBuiltins.size() + 64);
    for (const QString &name : kAsirBuiltins) {
        names.insert(name);
    }

    auto appendIdentifiers = [&names](const QString &text) {
        QRegularExpressionMatchIterator it = kIdentifierPattern.globalMatch(text);
        while (it.hasNext()) {
            names.insert(it.next().captured(1));
        }
    };

    appendIdentifiers(toPlainText());
    if (m_history) {
        for (const QString &entry : m_history->entries()) {
            appendIdentifiers(entry);
        }
    }

    QStringList result = names.values();
    result.sort();
    return result;
}

QString InputWidget::expandHistoryDirective(const QString &command, bool *previewOnly, bool *ok,
    QString *historyPrefix, int *matchedIndex) const
{
    if (previewOnly) {
        *previewOnly = false;
    }
    if (ok) {
        *ok = true;
    }
    if (historyPrefix) {
        historyPrefix->clear();
    }
    if (matchedIndex) {
        *matchedIndex = -1;
    }

    const QString trimmed = command.trimmed();
    if (!trimmed.startsWith(QLatin1Char('!'))) {
        return trimmed;
    }
    if (!m_history || m_history->entries().isEmpty()) {
        if (ok) {
            *ok = false;
        }
        return {};
    }

    if (trimmed == QStringLiteral("!!")) {
        if (matchedIndex) {
            *matchedIndex = m_history->entries().size() - 1;
        }
        return m_history->entries().constLast();
    }

    QString token = trimmed.mid(1);
    bool localPreviewOnly = false;
    if (token.endsWith(QStringLiteral(":p"))) {
        token.chop(2);
        localPreviewOnly = true;
    }
    if (token.isEmpty()) {
        if (ok) {
            *ok = false;
        }
        return {};
    }

    const QStringList &entries = m_history->entries();
    for (int index = entries.size() - 1; index >= 0; --index) {
        if (entries.at(index).startsWith(token)) {
            if (previewOnly) {
                *previewOnly = localPreviewOnly;
            }
            if (historyPrefix) {
                *historyPrefix = token;
            }
            if (matchedIndex) {
                *matchedIndex = index;
            }
            return entries.at(index);
        }
    }

    if (ok) {
        *ok = false;
    }
    return {};
}
