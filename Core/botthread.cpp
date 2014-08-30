#include "botthread.h"
#include <QDebug>

int gcd(int u, int v) {
    return (v != 0) ? gcd(v, u%v) : u;
}

int gcd(std::vector<int> v)
{
    if(v.size() == 1)
        return v.at(0);

    std::vector<int> aux;
    for(int i=0; i < v.size()-1; ++i)
        aux.push_back(gcd(v.at(i),v.at(i+1)));

    return gcd(aux);
}

BotThread::BotThread(QObject *parent) :
    QThread(parent),
    m_pause(false),
    m_delay(100)
{
}

BotThread::~BotThread()
{
    for(auto i = m_steps.begin(); i != m_steps.end(); ++i)
        delete *i;
    m_steps.clear();
}

void BotThread::run()
{
    while(true)
    {
        sync.lock();
        if(m_pause)
            pauseCond.wait(&sync);
        sync.unlock();

        for(auto i = m_steps.begin(); i != m_steps.end(); ++i)
        {
            BotStep *step = *i;
            step->untilNextCall -= m_delay;
            if(step->untilNextCall < 0)
                step->call();
        }
        msleep(m_delay);
    }
}

void BotThread::pause()
{
    sync.lock();
    qDebug() << "Pausing Bot Thread...";
    m_pause = true;
    sync.unlock();
}

void BotThread::resume()
{
    sync.lock();
    qDebug() << "Resuming Bot Thread...";
    m_pause = false;
    sync.unlock();
    pauseCond.wakeAll();
}

void BotThread::registerStep(std::function<void()> function, int frec)
{
    m_steps.append(new BotStep(function,frec));
    calcDelay();
}

void BotThread::registerStep(QScriptValue function, int frec)
{
    if(function.isFunction())
    {
        m_steps.append(new BotStep(function,frec));
        calcDelay();
    }
}

void BotThread::calcDelay()
{
    std::vector<int> aux;
    for(auto i = m_steps.begin(); i != m_steps.end(); ++i)
    {
        BotStep *step = *i;
        aux.push_back(step->frecuency);
    }
    m_delay = gcd(aux);
    qDebug() << "new delay: " << m_delay;
}


BotThread::BotStep::BotStep(std::function<void()> fun, int frec) {
    this->Type = BotThread::BotStep::NATIVE;
    this->nativeFunction = fun;
    this->frecuency = frec;
    this->untilNextCall = frec;
}

BotThread::BotStep::BotStep(QScriptValue function, int frec)
{
    this->Type = BotThread::BotStep::SCRIPT;
    this->scriptFunction = function;
    this->frecuency = frec;
    this->untilNextCall = frec;
}

void BotThread::BotStep::call()
{
    if(this->Type == BotThread::BotStep::NATIVE)
        nativeFunction();
    else
        scriptFunction.call();

    untilNextCall = frecuency;
}
