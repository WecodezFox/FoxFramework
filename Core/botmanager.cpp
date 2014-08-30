#include "botmanager.h"
#include "injector.h"
#include "keyboard.h"
#include "botthread.h"
#include "../Common/Tags.h"
#include <QScriptEngine>
#include <QDebug>

BotManager::BotManager(QScriptEngine* eng,QObject *parent) :
    QObject(parent),
    _init(false)
{
    engine = eng;
    thread = new BotThread(this);
}

bool BotManager::FindMapleWindow()
{
    TCHAR szBuffer[200];
    DWORD dwTemp;
    DWORD pid = GetCurrentProcessId();
    for (HWND hWnd = GetTopWindow(NULL); hWnd != NULL; hWnd = GetNextWindow(hWnd, GW_HWNDNEXT))
    {
        GetWindowThreadProcessId(hWnd, &dwTemp);
        if (dwTemp != pid){	continue; }
        if (!GetClassName(hWnd, szBuffer, sizeof(szBuffer) / sizeof(TCHAR))){	continue;	}

        if (!wcscmp(szBuffer,L"StartUpDlgClass"))
        {            
            QObject* ptrInjector = engine->globalObject().property(API::Injector_Obj).toQObject();
            if(Injector *injector = qobject_cast<Injector*>(ptrInjector) )
            {
                const QString path = engine->globalObject().property(API::Path).toString();
                injector->injectFolder( path + "/dlls/");
            }

            KeyBoard::SendKey(hWnd,VK_ESCAPE);
            Sleep(15000);
            return FindMapleWindow();
        }

        if (!wcscmp(szBuffer, L"MapleStoryClass"))
        {
            this->hProcesswnd = hWnd;
            break;
        }
    }
    if(this->hProcesswnd == NULL)
    {
        return false;
    }


    while(!ShowWindow(this->hProcesswnd,SW_HIDE)){Sleep(100);}

    while(!ShowWindow(this->hProcesswnd,SW_NORMAL)){Sleep(100);}

    Sleep(300);
    return true;
}

bool BotManager::init()
{
    if(_init)
        return true;

    bool b = FindMapleWindow();    

    qDebug() << "Starting Bot Thread...";
    thread->start();

    _init = true;
    return b;
}

void BotManager::pause()
{
    thread->pause();
}

void BotManager::resume()
{
    thread->resume();
}

HWND BotManager::getHProcesswnd() const
{
    return hProcesswnd;
}

bool BotManager::registerStep()
{
    auto count = context()->argumentCount();
    if(count == 2)
    {
        const QScriptValue function = context()->argument(0);
        const QScriptValue frecuency = context()->argument(1);
        if(function.isFunction() && frecuency.isNumber())
        {
            thread->registerStep(function, frecuency.toInt32());
            return true;
        }
    }
    context()->throwError("RegisterStep takes (function, int) as arguments");
    return false;
}
