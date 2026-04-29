#pragma once

#include <QString>
#include <QStringList>

class CommandHistory {
public:
    void add(const QString &command);
    QString previous();
    QString next();
    void resetTraversal();
    void save(const QString &path) const;
    void load(const QString &path);
    const QStringList &entries() const;

private:
    static constexpr int MaxHistory = 256;

    QStringList m_entries;
    int m_index = -1;
};
