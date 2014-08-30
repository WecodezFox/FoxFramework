#ifndef STATHOOK_H
#define STATHOOK_H


#include <QtScript/QScriptExtensionPlugin>
class QScriptEngine;
class QScriptContext;

class StatHook : public QScriptExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fox.statshook")
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
    static QScriptValue postBypass(QScriptContext *context, QScriptEngine *engine);
private:
    static void setHooks(QScriptEngine *engine);
};

#endif // STATHOOK_H
