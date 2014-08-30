#ifndef PACKETEDITOR_H
#define PACKETEDITOR_H

#include "packeteditor_global.h"
#include <QtScript/QScriptExtensionPlugin>
class QScriptEngine;
class QScriptContext;
class PacketManager;

class PACKETEDITORSHARED_EXPORT PacketEditor:  public QScriptExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fox.packetEditor")
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
    static QScriptValue postBypass(QScriptContext *context, QScriptEngine *engine);

private:

};

#endif // PACKETEDITOR_H
