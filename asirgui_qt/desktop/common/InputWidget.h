#pragma once

#include "core/CommandHistory.h"

#include <QPlainTextEdit>

class InputWidget : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit InputWidget(CommandHistory *history, QWidget *parent = nullptr);
    void applyEditorFont(const QFont &font);
    void setInterruptEnabled(bool enabled) { m_interruptEnabled = enabled; }

signals:
    void commandSubmitted(const QString &command);
    void interruptRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void submitCurrentCommand();
    void setCurrentCommand(const QString &command);
    void insertIndentedNewline();
    bool handleClosingBraceIndent();
    QString currentLineIndentation() const;
    bool moveHistoryPrevious();
    bool moveHistoryNext();
    bool deleteToLineStart();
    bool deletePreviousWord();
    bool moveCursorToPreviousWord();
    bool moveCursorToNextWord();
    bool completeIdentifier();
    QString identifierPrefixAtCursor(int *startPosition = nullptr) const;
    QStringList completionCandidates() const;
    QString expandHistoryDirective(const QString &command, bool *previewOnly, bool *ok,
        QString *historyPrefix = nullptr, int *matchedIndex = nullptr) const;
    void setHistoryNavigationMode(bool enabled, const QString &filterPrefix = QString(), int browseIndex = -1);

    CommandHistory *m_history = nullptr;
    bool m_historyNavigationMode = false;
    QString m_historyFilterPrefix;
    int m_historyBrowseIndex = -1;
    bool m_interruptEnabled = false;
};
