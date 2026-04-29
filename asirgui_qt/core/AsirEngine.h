#pragma once

#include <QObject>
#include <QImage>
#include <QString>

struct AsirResult {
    QString command;
    QString text;
    QString latex;
    QString mathPlainText;
    QImage plotImage;
    QString plotSummary;
    QString timingText;
    bool isError = false;
    struct AxisInfo {
        bool valid = false;
        double xMin = 0.0;
        double xMax = 0.0;
        double yMin = 0.0;
        double yMax = 0.0;
    };
    AxisInfo axes;
};
Q_DECLARE_METATYPE(AsirResult)

class AsirEngine : public QObject {
    Q_OBJECT

public:
    explicit AsirEngine(QObject *parent = nullptr)
        : QObject(parent) {}
    ~AsirEngine() override = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void executeString(const QString &command, bool producePresentation = true) = 0;
    virtual void interruptCurrent() = 0;
    virtual bool isReady() const = 0;

signals:
    void resultReady(const AsirResult &result);
    void errorOccurred(const QString &message);
    void engineReady();
};
