#include "desktop/common/MathView.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QWebEnginePage>
#include <QWebEngineSettings>

namespace {

class MathPage : public QWebEnginePage {
public:
    explicit MathPage(MathView *owner)
        : QWebEnginePage(owner)
        , m_owner(owner)
    {
    }

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override
    {
        Q_UNUSED(type);
        Q_UNUSED(isMainFrame);
        if (url.scheme() == QStringLiteral("asirgui")
            && url.host() == QStringLiteral("force-render")) {
            QUrlQuery query(url);
            const QString token = query.queryItemValue(QStringLiteral("token"));
            if (!token.isEmpty() && m_owner) {
                QTimer::singleShot(0, m_owner, [owner = m_owner, token]() {
                    owner->notifyForceRenderRequested(token);
                });
            }
            return false;
        }
        return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    }

private:
    MathView *m_owner = nullptr;
};

}

MathView::MathView(QWidget *parent)
    : QWebEngineView(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setPage(new MathPage(this));
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    setHtml(QStringLiteral(), QUrl(QStringLiteral("qrc:/")));
    page()->load(QUrl(QStringLiteral("qrc:/math_render.html")));
}

void MathView::appendLatex(const QString &title, const QString &latex)
{
    QJsonArray args;
    args.append(title);
    args.append(latex);
    const QString json = QString::fromUtf8(QJsonDocument(args).toJson(QJsonDocument::Compact));
    page()->runJavaScript(QStringLiteral("appendLatexFromJson(%1);").arg(json));
}

void MathView::appendPlainText(const QString &title, const QString &text)
{
    QJsonArray args;
    args.append(title);
    args.append(text);
    const QString json = QString::fromUtf8(QJsonDocument(args).toJson(QJsonDocument::Compact));
    page()->runJavaScript(QStringLiteral("appendPlainTextFromJson(%1);").arg(json));
}

void MathView::appendRenderSkipped(const QString &title, const QString &message,
    const QString &details, const QString &token, bool allowForce)
{
    QJsonArray args;
    args.append(title);
    args.append(message);
    args.append(details);
    args.append(token);
    args.append(allowForce);
    const QString json = QString::fromUtf8(QJsonDocument(args).toJson(QJsonDocument::Compact));
    page()->runJavaScript(QStringLiteral("appendRenderSkippedFromJson(%1);").arg(json));
}

void MathView::clearOutput()
{
    page()->runJavaScript(QStringLiteral("clearOutput();"));
}

void MathView::scrollToBottom()
{
    page()->runJavaScript(QStringLiteral("window.scrollTo(0, document.body.scrollHeight);"));
}

void MathView::notifyForceRenderRequested(const QString &token)
{
    emit forceRenderRequested(token);
}
