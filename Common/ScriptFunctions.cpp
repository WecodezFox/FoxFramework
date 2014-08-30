#include "ScriptFunctions.h"
#include "Tags.h"

#include <QApplication>
#include <QScriptEngine>
#include <QScriptContext>

#include <QDebug>
#include <QFile>

QScriptValue ScriptFunctions::getFolder(QScriptContext *context, QScriptEngine * engine)
{
    const QString path = engine->globalObject().property("Path").toString();

    QStringList backtrace = context->backtrace();

    for(auto it = backtrace.begin(); it != backtrace.end(); ++it)
    {
        const QString line = *it;
        if(line.contains(path))
        {
            auto s = line.indexOf(path);
            auto e = line.lastIndexOf("/") + 1;
            const QString folder = line.mid(s, e-s);
            return QScriptValue(folder);
        }
    }

    return QScriptValue(path);
}


QScriptValue ScriptFunctions::include(QScriptContext *context, QScriptEngine *engine)
{
    if(context->argumentCount() != 1)
    {
        context->throwError(QStringLiteral("include accept only one argument"));
        return QScriptValue(false);
    }

    const QString folder = ScriptFunctions::getFolder(context,engine).toString();
    const QString file = context->argument(0).toString();

    QFile f(folder + file);
    bool ok = false;
    if(f.open(QIODevice::ReadOnly))
    {
        ok = true;
        QString content = f.readAll();
        engine->evaluate(content);
    }
    f.close();
    return QScriptValue(ok);
}

QScriptValue ScriptFunctions::nativePrint(QScriptContext *context, QScriptEngine * /*engine*/)
{
    if(context->argumentCount() == 1)
    {
        qDebug() << context->argument(0).toString();
    }
    return QScriptValue();
}

void ScriptFunctions::registerFuncions(QScriptEngine *engine)
{
     engine->globalObject().setProperty(API::Folder_func,engine->newFunction(ScriptFunctions::getFolder));
     engine->globalObject().setProperty(API::Include_func,engine->newFunction(ScriptFunctions::include));
     engine->globalObject().setProperty(API::Print_func,engine->newFunction(ScriptFunctions::nativePrint));
}


QMap<QString,QScriptValue> ScriptFunctions::callAllImportsFunction(
        QScriptEngine *engine,const QString function, QMap<QString,QScriptValue> args)
{
    QMap<QString,QScriptValue> ret;
    const QStringList imports = engine->importedExtensions();
    const QScriptValue g_object = engine->globalObject();
    for(auto it = imports.cbegin() ; it != imports.cend(); ++it)
    {
        const QString extension = *it;
        QStringList parts = extension.split(".");
        QScriptValue funct = g_object.property(parts.first());
        for(auto i = 1; i < parts.length(); ++i)
            funct = funct.property(parts.at(i));
        funct = funct.property(function);
        if(funct.isFunction())
            ret.insert(extension,funct.call(QScriptValue(), QScriptValueList() << args.value(extension)));
    }
    return ret;
}
