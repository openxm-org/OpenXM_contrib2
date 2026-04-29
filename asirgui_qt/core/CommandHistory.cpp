#include "core/CommandHistory.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QTextStream>

void CommandHistory::add(const QString &command)
{
    const QString trimmed = command.trimmed();
    if (trimmed.isEmpty()) {
        resetTraversal();
        return;
    }
    if (!m_entries.isEmpty() && m_entries.constLast() == trimmed) {
        resetTraversal();
        return;
    }

    m_entries.append(trimmed);
    while (m_entries.size() > MaxHistory) {
        m_entries.removeFirst();
    }
    resetTraversal();
}

QString CommandHistory::previous()
{
    if (m_entries.isEmpty()) {
        return {};
    }

    if (m_index < 0) {
        m_index = m_entries.size() - 1;
    } else if (m_index > 0) {
        --m_index;
    }
    return m_entries.value(m_index);
}

QString CommandHistory::next()
{
    if (m_entries.isEmpty()) {
        return {};
    }

    if (m_index < 0) {
        return {};
    }
    if (m_index >= m_entries.size() - 1) {
        m_index = -1;
        return {};
    }

    ++m_index;
    return m_entries.value(m_index);
}

void CommandHistory::resetTraversal()
{
    m_index = -1;
}

void CommandHistory::save(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }

    QJsonArray array;
    for (const QString &entry : m_entries) {
        array.append(entry);
    }
    file.write(QJsonDocument(array).toJson(QJsonDocument::Compact));
}

void CommandHistory::load(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QByteArray data = file.readAll();
    QStringList entries;

    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        const QJsonArray array = doc.array();
        entries.reserve(array.size());
        for (const QJsonValue &value : array) {
            const QString entry = value.toString().trimmed();
            if (!entry.isEmpty()) {
                entries.append(entry);
            }
        }
    } else {
        QTextStream in(data);
        while (!in.atEnd()) {
            const QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                entries.append(line);
            }
        }
    }

    while (entries.size() > MaxHistory) {
        entries.removeFirst();
    }
    m_entries = entries;
    resetTraversal();
}

const QStringList &CommandHistory::entries() const
{
    return m_entries;
}
