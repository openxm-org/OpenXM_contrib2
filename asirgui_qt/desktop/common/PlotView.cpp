#include "desktop/common/PlotView.h"

#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QWidget>

#include <cmath>

namespace {

double adjustScale(double e, double w)
{
    const int bucket = static_cast<int>(std::floor(w / e));
    switch (bucket) {
    case 1:
        return e / 4.0;
    case 2:
    case 3:
        return e / 2.0;
    case 4:
    case 5:
    case 6:
    case 7:
        return e;
    default:
        return 2.0 * e;
    }
}

double computeTickStep(double minValue, double maxValue, int pixelWidth)
{
    const double width = maxValue - minValue;
    if (!(width > 0.0) || pixelWidth <= 0) {
        return 1.0;
    }
    const double scaledWidth = width * 400.0 / static_cast<double>(pixelWidth);
    const double base = std::pow(10.0, std::floor(std::log10(scaledWidth)));
    return adjustScale(base, scaledWidth);
}

QString formatTickLabel(double value)
{
    if (qFuzzyIsNull(value)) {
        value = 0.0;
    }
    return QString::number(value, 'g', 6);
}

} // namespace

PlotView::PlotView(QWidget *parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    m_container = new QWidget(this);
    m_layout = new QVBoxLayout(m_container);
    m_layout->setContentsMargins(12, 12, 12, 12);
    m_layout->setSpacing(12);
    m_layout->addStretch();

    setWidget(m_container);
    setBackgroundRole(QPalette::Base);
}

void PlotView::appendPlot(const QString &title, const QImage &image, bool axesValid, double xMin, double xMax, double yMin, double yMax)
{
    if (image.isNull()) {
        return;
    }

    auto *block = new QFrame(m_container);
    block->setFrameShape(QFrame::StyledPanel);

    auto *layout = new QVBoxLayout(block);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, block);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    auto *imageLabel = new QLabel(block);
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);

    m_layout->insertWidget(m_layout->count() - 1, block);
    m_entries.append({block, titleLabel, imageLabel, image, axesValid, xMin, xMax, yMin, yMax});
    refreshPlots();
    QTimer::singleShot(0, this, &PlotView::scrollToLatest);
}

void PlotView::clearOutput()
{
    while (m_layout->count() > 1) {
        QLayoutItem *item = m_layout->takeAt(0);
        if (!item) {
            continue;
        }
        delete item->widget();
        delete item;
    }
    m_entries.clear();
}

void PlotView::scrollToLatest()
{
    if (m_entries.isEmpty()) {
        return;
    }

    if (m_layout) {
        m_layout->activate();
    }
    if (m_container) {
        m_container->adjustSize();
    }

    if (QFrame *frame = m_entries.constLast().frame) {
        const int top = qMax(0, frame->y() - 12);
        verticalScrollBar()->setValue(top);
    }
}

QColor PlotView::foregroundColor() const
{
    return m_foregroundColor;
}

QColor PlotView::backgroundColor() const
{
    return m_backgroundColor;
}

int PlotView::lineThickness() const
{
    return m_lineThickness;
}

double PlotView::zoomFactor() const
{
    return m_zoomFactor;
}

bool PlotView::fitToWidthEnabled() const
{
    return m_fitToWidth;
}

bool PlotView::showAxes() const
{
    return m_showAxes;
}

bool PlotView::showAxisLabels() const
{
    return m_showAxisLabels;
}

void PlotView::setForegroundColor(const QColor &color)
{
    if (!color.isValid() || color == m_foregroundColor) {
        return;
    }
    m_foregroundColor = color;
    refreshPlots();
}

void PlotView::setBackgroundColor(const QColor &color)
{
    if (!color.isValid() || color == m_backgroundColor) {
        return;
    }
    m_backgroundColor = color;
    refreshPlots();
}

void PlotView::setLineThickness(int thickness)
{
    const int normalized = qMax(1, thickness);
    if (normalized == m_lineThickness) {
        return;
    }
    m_lineThickness = normalized;
    refreshPlots();
}

void PlotView::setZoomFactor(double factor)
{
    const double normalized = qBound(0.25, factor, 8.0);
    if (qFuzzyCompare(m_zoomFactor, normalized)) {
        return;
    }
    m_fitToWidth = false;
    m_zoomFactor = normalized;
    refreshPlots();
}

void PlotView::zoomIn()
{
    m_fitToWidth = false;
    m_zoomFactor = qMin(8.0, m_zoomFactor * 1.25);
    refreshPlots();
}

void PlotView::zoomOut()
{
    m_fitToWidth = false;
    m_zoomFactor = qMax(0.25, m_zoomFactor / 1.25);
    refreshPlots();
}

void PlotView::resetZoom()
{
    m_fitToWidth = false;
    m_zoomFactor = 1.0;
    refreshPlots();
}

void PlotView::setFitToWidth(bool enabled)
{
    if (m_fitToWidth == enabled) {
        return;
    }
    m_fitToWidth = enabled;
    refreshPlots();
}

void PlotView::setShowAxes(bool enabled)
{
    if (m_showAxes == enabled) {
        return;
    }
    m_showAxes = enabled;
    refreshPlots();
}

void PlotView::setShowAxisLabels(bool enabled)
{
    if (m_showAxisLabels == enabled) {
        return;
    }
    m_showAxisLabels = enabled;
    refreshPlots();
}

void PlotView::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    if (m_fitToWidth) {
        refreshPlots();
    }
}

QImage PlotView::renderStyledImage(const PlotEntry &entry) const
{
    if (entry.sourceImage.isNull()) {
        return {};
    }

    const bool drawAxisLabels = m_showAxes && m_showAxisLabels && entry.axesValid;
    const int leftMargin = drawAxisLabels ? 56 : 0;
    const int rightMargin = drawAxisLabels ? 12 : 0;
    const int topMargin = drawAxisLabels ? 10 : 0;
    const int bottomMargin = drawAxisLabels ? 30 : 0;
    const QRect plotRect(leftMargin, topMargin, entry.sourceImage.width(), entry.sourceImage.height());

    QImage styled(entry.sourceImage.size() + QSize(leftMargin + rightMargin, topMargin + bottomMargin),
        QImage::Format_ARGB32_Premultiplied);
    styled.fill(m_backgroundColor);

    QPainter painter(&styled);
    const auto mapX = [&](double value) {
        if (qFuzzyCompare(entry.xMin, entry.xMax)) {
            return plotRect.left();
        }
        return plotRect.left() + qRound((static_cast<double>(plotRect.width() - 1))
            * ((value - entry.xMin) / (entry.xMax - entry.xMin)));
    };
    const auto mapY = [&](double value) {
        if (qFuzzyCompare(entry.yMin, entry.yMax)) {
            return plotRect.bottom();
        }
        return plotRect.top() + qRound((static_cast<double>(plotRect.height() - 1))
            * ((entry.yMax - value) / (entry.yMax - entry.yMin)));
    };

    if (m_showAxes && entry.axesValid) {
        painter.setPen(QPen(QColor(148, 163, 184), 1, Qt::DashLine));

        int x0 = plotRect.left();
        if (entry.xMin < 0.0 && entry.xMax > 0.0) {
            x0 = mapX(0.0);
        } else if (entry.xMin < 0.0) {
            x0 = plotRect.right();
        }

        int y0 = plotRect.bottom();
        if (entry.yMin < 0.0 && entry.yMax > 0.0) {
            y0 = mapY(0.0);
        } else if (entry.yMax < 0.0) {
            y0 = plotRect.top();
        }

        painter.drawLine(x0, plotRect.top(), x0, plotRect.bottom());
        painter.drawLine(plotRect.left(), y0, plotRect.right(), y0);

        if (drawAxisLabels) {
            painter.setPen(QPen(QColor(71, 85, 105), 1));
            const QFontMetrics metrics(painter.font());
            const double xStep = computeTickStep(entry.xMin, entry.xMax, plotRect.width());
            const double yStep = computeTickStep(entry.yMin, entry.yMax, plotRect.height());
            const int tickLength = 5;
            const int labelGap = 4;

            for (double n = std::ceil(entry.xMin / xStep); n * xStep <= entry.xMax + xStep * 0.5; n += 1.0) {
                const double value = n * xStep;
                const int x = mapX(value);
                const QString label = formatTickLabel(value);
                const int labelWidth = metrics.horizontalAdvance(label);
                const int tickBottom = qMin(styled.height() - 1, y0 + tickLength);
                const int textTop = qBound(0, tickBottom + labelGap, styled.height() - metrics.height());
                const int textLeft = qBound(0, x - labelWidth / 2, styled.width() - labelWidth);

                painter.drawLine(x, y0, x, tickBottom);
                painter.drawText(QRect(textLeft, textTop, labelWidth, metrics.height()),
                    Qt::AlignHCenter | Qt::AlignTop, label);
            }

            for (double n = std::ceil(entry.yMin / yStep); n * yStep <= entry.yMax + yStep * 0.5; n += 1.0) {
                const double value = n * yStep;
                const int y = mapY(value);
                const QString label = formatTickLabel(value);
                const int labelWidth = metrics.horizontalAdvance(label);
                const int tickLeft = qMax(0, x0 - tickLength);
                const int textLeft = qBound(0, tickLeft - labelGap - labelWidth, styled.width() - labelWidth);
                const int textTop = qBound(0, y - metrics.height() / 2, styled.height() - metrics.height());

                painter.drawLine(tickLeft, y, x0, y);
                painter.drawText(QRect(textLeft, textTop, labelWidth, metrics.height()),
                    Qt::AlignRight | Qt::AlignVCenter, label);
            }
        }
    }

    const QRgb fgRgb = m_foregroundColor.rgba();

    if (m_lineThickness <= 1) {
        for (int y = 0; y < entry.sourceImage.height(); ++y) {
            const QRgb *srcLine = reinterpret_cast<const QRgb *>(entry.sourceImage.constScanLine(y));
            QRgb *dstLine = reinterpret_cast<QRgb *>(styled.scanLine(plotRect.top() + y));
            for (int x = 0; x < entry.sourceImage.width(); ++x) {
                if (qAlpha(srcLine[x]) > 0) {
                    dstLine[plotRect.left() + x] = fgRgb;
                }
            }
        }
    } else {
        const int radius = m_lineThickness / 2;
        painter.setPen(Qt::NoPen);
        for (int y = 0; y < entry.sourceImage.height(); ++y) {
            const QRgb *srcLine = reinterpret_cast<const QRgb *>(entry.sourceImage.constScanLine(y));
            for (int x = 0; x < entry.sourceImage.width(); ++x) {
                if (qAlpha(srcLine[x]) > 0) {
                    painter.fillRect(plotRect.left() + x - radius, plotRect.top() + y - radius,
                        m_lineThickness, m_lineThickness, m_foregroundColor);
                }
            }
        }
    }

    return styled;
}

QPixmap PlotView::buildPixmap(const PlotEntry &entry) const
{
    const QImage styled = renderStyledImage(entry);
    if (styled.isNull()) {
        return {};
    }

    QSize targetSize = styled.size();
    if (m_fitToWidth) {
        const int availableWidth = qMax(64, viewport()->width() - 64);
        if (targetSize.width() > availableWidth) {
            targetSize = styled.size().scaled(availableWidth, 100000, Qt::KeepAspectRatio);
        }
    } else if (!qFuzzyCompare(m_zoomFactor, 1.0)) {
        targetSize = QSize(qMax(1, qRound(styled.width() * m_zoomFactor)),
                           qMax(1, qRound(styled.height() * m_zoomFactor)));
    }

    if (targetSize == styled.size()) {
        return QPixmap::fromImage(styled);
    }
    return QPixmap::fromImage(styled.scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation));
}

void PlotView::refreshPlots()
{
    for (PlotEntry &entry : m_entries) {
        if (!entry.imageLabel) {
            continue;
        }
        const QPixmap pixmap = buildPixmap(entry);
        entry.imageLabel->setPixmap(pixmap);
        entry.imageLabel->setMinimumSize(pixmap.size());
    }
}
