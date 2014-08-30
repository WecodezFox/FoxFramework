#include "injector.h"

#include <QScriptContext>
#include <QScriptEngine>

#include <QDir>
#include <QDirIterator>

#include <Windows.h>

Injector::Injector(QObject *parent) :
    QObject(parent)
{
}

bool Injector::injectFolder(QString folder)
{
    bool ret = true;

    QStringList filter;
    filter << "*.dll";

    QDirIterator it(folder,filter);
    while(it.hasNext())
        ret &= _inject(it.next());

    return ret;
}

bool Injector::inject()
{
    auto count = context()->argumentCount();
    if(count == 1)
    {        
        const QString path = engine()->globalObject().property("path").toString();
        QString folder = path;

        QStringList backtrace = context()->backtrace();

        for(auto it = backtrace.begin(); it != backtrace.end(); ++it)
        {
            const QString line = *it;
            if(line.contains(path))
            {
                auto s = line.indexOf(path);
                auto e = line.lastIndexOf("/") + 1;
                folder = line.mid(s, e-s);
                break;
            }
        }

        if(!folder.endsWith("/"))
            folder.append("/");

        const QString dll = context()->argument(0).toString();
        const QString target  = folder + dll;

        return _inject(target);
    }
    else
    {
        context()->throwError("Inject takes one argument");
        return false;
    }
}

bool Injector::_inject(QString file)
{
    std::wstring stdWString = file.toStdWString();
    LPCWSTR lib = stdWString.c_str();
    return LoadLibraryW(lib) != NULL;
}
