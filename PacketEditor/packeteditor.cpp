#include "packeteditor.h"
#include "packetmanager.h"

#include <QScriptEngine>
#include "../Common/Tags.h"

PacketManager * pe;

QStringList PacketEditor::keys() const
{
    QStringList list;
    list << QStringLiteral("packetEditor");
    list << QStringLiteral("packetEditor.client");
    return list;
}

void PacketEditor::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QStringLiteral("packetEditor"))
    {
        setupPackage(key,engine);
    }
    else if (key == QStringLiteral("packetEditor.client"))
    {
        setupPackage(key,engine);
        engine->globalObject()
                .property("packetEditor")
                .property("client")
                .setProperty("postBypass", engine->newFunction(&PacketEditor::postBypass));
        pe = new PacketManager(this);
    }
    else
    {
        Q_ASSERT_X(false, "PlugTest::initialize", qPrintable(key));
    }
}

QScriptValue PacketEditor::postBypass(QScriptContext * /*context*/, QScriptEngine *engine)
{
    const QScriptValue addys = engine->globalObject().property(API::Addys_Obj);
    const QScriptValue pointers = engine->globalObject().property(API::Pointers_Obj);
    pe->init(addys,pointers);
    engine->globalObject().setProperty("Packets",engine->newQObject(pe));
    return QScriptValue();
}
