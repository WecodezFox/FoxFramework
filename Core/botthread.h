#ifndef BOTTHREAD_H
#define BOTTHREAD_H

#include <QThread>
#include <QScriptValue>
#include <QWaitCondition>
#include <QMutex>
#include <functional>

class BotThread : public QThread
{
    Q_OBJECT
    friend class BotManager;
public:    
    void run();
    void pause();
    void resume();
signals:

public slots:

private:
    explicit BotThread(QObject *parent = 0);
    ~BotThread();    

    QMutex sync;
    QWaitCondition pauseCond;
    bool m_pause;

    void registerStep(std::function<void()> function, int frec);
    void registerStep(QScriptValue function, int frec);

    class BotStep
    {
    public:
        BotStep(std::function<void()> fun, int frec);
        BotStep(QScriptValue function, int frec);
        void call();
        int frecuency;
        int untilNextCall;
    private:
        enum TYPE { SCRIPT = 0, NATIVE};
        TYPE Type;
        std::function<void()> nativeFunction;
        QScriptValue scriptFunction;
    };

    QList<BotStep*> m_steps;
    int m_delay;
    void calcDelay();
};

#endif // BOTTHREAD_H
