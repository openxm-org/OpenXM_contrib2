#pragma once

#include <QWebEngineView>

class MathView : public QWebEngineView {
    Q_OBJECT

public:
    explicit MathView(QWidget *parent = nullptr);

    void appendLatex(const QString &title, const QString &latex);
    void appendPlainText(const QString &title, const QString &text);
    void appendRenderSkipped(const QString &title, const QString &message,
        const QString &details, const QString &token, bool allowForce);
    void clearOutput();
    void scrollToBottom();
    void notifyForceRenderRequested(const QString &token);

signals:
    void forceRenderRequested(const QString &token);
};
