#ifndef PATTERNFINDER_H
#define PATTERNFINDER_H

#include <QObject>
#include <QScriptable>
#include <QScriptValue>
#include "core_global.h"

class CORE_SYMBOL PatternFinder : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    /*!
     * \brief Wrapper for Franc[e]sco's pattern find to use it on scripting.
     */
    explicit PatternFinder(QObject *parent = 0);

    static QList<unsigned long> Find(QString pattern);
signals:

public slots:
    QScriptValue find(QString pattern);
};

#endif // PATTERNFINDER_H
