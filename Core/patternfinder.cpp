#include "patternfinder.h"
#include "PatternFind.h"

#include <QScriptEngine>

PatternFinder::PatternFinder(QObject *parent) :
    QObject(parent)
{
}

QList<unsigned long> PatternFinder::Find(QString pattern)
{
    QList<unsigned long> ret;

    PFSEARCH pf;
    unsigned long start = 0;
    //QByteArray ba = pattern.toLocal8Bit();
    //char *c_str = ba.data();
    std::string stdString = pattern.toStdString();
    char * c_str = &stdString[0];

    while( (FindPattern(c_str, &pf, (LPVOID)start) == PF_NONE) && (ret.length() < 10))
    {
        ret.append((unsigned long)pf.lpvResult);
        start = (unsigned long)pf.lpvResult + 8;
    }

    return ret;
}

QScriptValue PatternFinder::find(QString pattern)
{
    QList<unsigned long> ret;

    PFSEARCH pf;
    unsigned long start = 0;
    QByteArray ba = pattern.toLocal8Bit();
    char *c_str = ba.data();

    while( (FindPattern(c_str, &pf, (LPVOID)start) == PF_NONE) && (ret.length() < 10))
    {
        ret.append((unsigned long)pf.lpvResult);
        start = (unsigned long)pf.lpvResult + 8;
    }

    QScriptValue arr = engine()->newArray(ret.length());
    for(auto i=0; i< ret.length(); ++i)
        arr.setProperty(i,QScriptValue((uint)ret.at(i)));
    return arr;
}
