#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include <QObject>
#include <QScriptable>
#include <Windows.h>
#include "core_global.h"

class QScriptEngine;
class BotThread;


class CORE_SYMBOL BotManager : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    BotManager(QScriptEngine* eng, QObject *parent = 0);
    HWND getHProcesswnd() const;

signals:

public slots:
    bool registerStep();

    bool init();
    void pause();
    void resume();
private:
    bool FindMapleWindow();
    HWND hProcesswnd;

    QScriptEngine *engine; 
    BotThread* thread;
    bool _init;
};

#endif // BOTMANAGER_H
