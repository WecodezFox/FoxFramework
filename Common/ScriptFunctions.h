#ifndef SCRIPTFUNCTIONS_H
#define SCRIPTFUNCTIONS_H

#include <QScriptValue>
#include <QMap>
class QScriptEngine;
class QScriptContext;

namespace ScriptFunctions {

static QScriptValue getFolder(QScriptContext *context, QScriptEngine *engine);
static QScriptValue include(QScriptContext *context, QScriptEngine *engine);
static QScriptValue nativePrint(QScriptContext *context, QScriptEngine *engine);

void registerFuncions(QScriptEngine *engine);
QMap<QString,QScriptValue> callAllImportsFunction(
        QScriptEngine *engine,
        const QString function,
        QMap<QString,QScriptValue> args = QMap<QString,QScriptValue>());
}


#endif // SCRIPTFUNCTIONS_H
