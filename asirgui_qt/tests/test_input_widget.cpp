#include "desktop/common/InputWidget.h"

#include <QSignalSpy>
#include <QtTest/QTest>

class InputWidgetTest : public QObject {
    Q_OBJECT

private slots:
    void ctrlUDeletesToLineStart();
    void ctrlCRequestsInterruptOnlyWhenEnabled();
    void tabCompletesIdentifier();
    void historyExpansionPreviewAndSubmit();
    void historyExpansionPreviewSupportsFilteredUpDown();
    void upUsesHistoryOnlyWhenEditorIsEmpty();
    void upDoesNotUseHistoryWhenEditorHasContent();
    void upDownUsesHistoryOnlyAfterCtrlP();
    void ctrlPCtrlNContinueHistoryNavigation();
};

void InputWidgetTest::ctrlUDeletesToLineStart()
{
    CommandHistory history;
    InputWidget widget(&history);
    widget.show();
    widget.setPlainText(QStringLiteral("    R = R + car(A);"));

    QTextCursor cursor = widget.textCursor();
    cursor.movePosition(QTextCursor::End);
    widget.setTextCursor(cursor);

    QTest::keyClick(&widget, Qt::Key_U, Qt::ControlModifier);
    QCOMPARE(widget.toPlainText(), QString());
}

void InputWidgetTest::ctrlCRequestsInterruptOnlyWhenEnabled()
{
    CommandHistory history;
    InputWidget widget(&history);
    widget.show();

    QSignalSpy spy(&widget, &InputWidget::interruptRequested);

    QTest::keyClick(&widget, Qt::Key_C, Qt::ControlModifier);
    QCOMPARE(spy.count(), 0);

    widget.setInterruptEnabled(true);
    QTest::keyClick(&widget, Qt::Key_C, Qt::ControlModifier);
    QCOMPARE(spy.count(), 1);
}

void InputWidgetTest::tabCompletesIdentifier()
{
    CommandHistory history;
    history.add(QStringLiteral("fctr(x^4-1);"));

    InputWidget widget(&history);
    widget.show();
    widget.setPlainText(QStringLiteral("fc"));

    QTextCursor cursor = widget.textCursor();
    cursor.movePosition(QTextCursor::End);
    widget.setTextCursor(cursor);

    QTest::keyClick(&widget, Qt::Key_Tab);
    QCOMPARE(widget.toPlainText(), QStringLiteral("fctr"));
}

void InputWidgetTest::historyExpansionPreviewAndSubmit()
{
    CommandHistory history;
    history.add(QStringLiteral("fctr(x^3-1);"));
    history.add(QStringLiteral("ifplot(x^2+y^2-1,[x,-2,2],[y,-2,2]);"));

    InputWidget widget(&history);
    widget.show();

    widget.setPlainText(QStringLiteral("!fctr:p"));
    QTest::keyClick(&widget, Qt::Key_Return, Qt::ShiftModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("fctr(x^3-1);"));

    widget.setPlainText(QStringLiteral("!!"));
    QSignalSpy spy(&widget, &InputWidget::commandSubmitted);
    QTest::keyClick(&widget, Qt::Key_Return, Qt::ShiftModifier);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(), QStringLiteral("ifplot(x^2+y^2-1,[x,-2,2],[y,-2,2]);"));
    QCOMPARE(widget.toPlainText(), QString());
}

void InputWidgetTest::historyExpansionPreviewSupportsFilteredUpDown()
{
    CommandHistory history;
    history.add(QStringLiteral("ifplot(x,[x,-1,1],[y,-1,1]);"));
    history.add(QStringLiteral("fctr(x^3-1);"));
    history.add(QStringLiteral("ifplot(y,[x,-2,2],[y,-2,2]);"));

    InputWidget widget(&history);
    widget.show();

    widget.setPlainText(QStringLiteral("!if:p"));
    QTest::keyClick(&widget, Qt::Key_Return, Qt::ShiftModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("ifplot(y,[x,-2,2],[y,-2,2]);"));

    QTest::keyClick(&widget, Qt::Key_Up);
    QCOMPARE(widget.toPlainText(), QStringLiteral("ifplot(x,[x,-1,1],[y,-1,1]);"));

    QTest::keyClick(&widget, Qt::Key_Down);
    QCOMPARE(widget.toPlainText(), QStringLiteral("ifplot(y,[x,-2,2],[y,-2,2]);"));

    QTest::keyPress(&widget, Qt::Key_Control);
    QTest::keyClick(&widget, Qt::Key_P, Qt::ControlModifier);
    QTest::keyRelease(&widget, Qt::Key_Control);
    QCOMPARE(widget.toPlainText(), QStringLiteral("ifplot(x,[x,-1,1],[y,-1,1]);"));
}

void InputWidgetTest::upUsesHistoryOnlyWhenEditorIsEmpty()
{
    CommandHistory history;
    history.add(QStringLiteral("1+1;"));
    history.add(QStringLiteral("2+2;"));

    InputWidget widget(&history);
    widget.show();

    QTest::keyClick(&widget, Qt::Key_Up);
    QCOMPARE(widget.toPlainText(), QStringLiteral("2+2;"));
}

void InputWidgetTest::upDoesNotUseHistoryWhenEditorHasContent()
{
    CommandHistory history;
    history.add(QStringLiteral("1+1;"));
    history.add(QStringLiteral("2+2;"));

    InputWidget widget(&history);
    widget.show();
    widget.setPlainText(QStringLiteral("A;\nB;"));

    QTextCursor cursor = widget.textCursor();
    cursor.movePosition(QTextCursor::End);
    widget.setTextCursor(cursor);

    QTest::keyClick(&widget, Qt::Key_Up);
    QCOMPARE(widget.toPlainText(), QStringLiteral("A;\nB;"));
    QCOMPARE(widget.textCursor().blockNumber(), 0);
}

void InputWidgetTest::upDownUsesHistoryOnlyAfterCtrlP()
{
    CommandHistory history;
    history.add(QStringLiteral("1+1;"));
    history.add(QStringLiteral("2+2;"));

    InputWidget widget(&history);
    widget.show();
    widget.setPlainText(QStringLiteral("A;\nB;"));

    QTest::keyClick(&widget, Qt::Key_P, Qt::ControlModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("2+2;"));

    QTest::keyClick(&widget, Qt::Key_Up);
    QCOMPARE(widget.toPlainText(), QStringLiteral("1+1;"));

    QTest::keyClick(&widget, Qt::Key_Down);
    QCOMPARE(widget.toPlainText(), QStringLiteral("2+2;"));
}

void InputWidgetTest::ctrlPCtrlNContinueHistoryNavigation()
{
    CommandHistory history;
    history.add(QStringLiteral("1+1;"));
    history.add(QStringLiteral("2+2;"));
    history.add(QStringLiteral("3+3;"));

    InputWidget widget(&history);
    widget.show();

    QTest::keyClick(&widget, Qt::Key_P, Qt::ControlModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("3+3;"));

    QTest::keyClick(&widget, Qt::Key_P, Qt::ControlModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("2+2;"));

    QTest::keyClick(&widget, Qt::Key_N, Qt::ControlModifier);
    QCOMPARE(widget.toPlainText(), QStringLiteral("3+3;"));
}

QTEST_MAIN(InputWidgetTest)

#include "test_input_widget.moc"
