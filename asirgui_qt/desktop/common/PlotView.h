#pragma once

#include <QColor>
#include <QImage>
#include <QScrollArea>
#include <QVector>

class QFrame;
class QLabel;
class QVBoxLayout;
class QWidget;
class QResizeEvent;

class PlotView : public QScrollArea {
    Q_OBJECT

public:
    explicit PlotView(QWidget *parent = nullptr);

    void appendPlot(const QString &title, const QImage &image, bool axesValid, double xMin, double xMax, double yMin, double yMax);
    void clearOutput();
    void scrollToLatest();

    QColor foregroundColor() const;
    QColor backgroundColor() const;
    int lineThickness() const;
    double zoomFactor() const;
    bool fitToWidthEnabled() const;
    bool showAxes() const;
    bool showAxisLabels() const;

public slots:
    void setForegroundColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setLineThickness(int thickness);
    void setZoomFactor(double factor);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void setFitToWidth(bool enabled);
    void setShowAxes(bool enabled);
    void setShowAxisLabels(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    struct PlotEntry {
        QFrame *frame = nullptr;
        QLabel *titleLabel = nullptr;
        QLabel *imageLabel = nullptr;
        QImage sourceImage;
        bool axesValid = false;
        double xMin = 0.0;
        double xMax = 0.0;
        double yMin = 0.0;
        double yMax = 0.0;
    };

    QImage renderStyledImage(const PlotEntry &entry) const;
    QPixmap buildPixmap(const PlotEntry &entry) const;
    void refreshPlots();

    QWidget *m_container = nullptr;
    QVBoxLayout *m_layout = nullptr;
    QVector<PlotEntry> m_entries;
    QColor m_foregroundColor = QColor(17, 24, 39);
    QColor m_backgroundColor = QColor(248, 250, 252);
    int m_lineThickness = 1;
    double m_zoomFactor = 1.0;
    bool m_fitToWidth = true;
    bool m_showAxes = true;
    bool m_showAxisLabels = true;
};
