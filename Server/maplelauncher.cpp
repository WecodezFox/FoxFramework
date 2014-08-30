#include "maplelauncher.h"
#include "detours.h"
#include <vector>
#include <QApplication>
#include <QMutexLocker>
#include <QDebug>


std::wstring MapleLauncher::strProcess;

MapleLauncher::MapleLauncher(QObject *parent, std::wstring strProcess):
    QThread(parent),
    dwProcessId(NULL),
    hProcess(NULL),
    oldPid(NULL)
{
    MapleLauncher::strProcess = strProcess;    
}

MapleLauncher::~MapleLauncher()
{
    while (_b) { sleep(100);}
}

bool MapleLauncher::StartProcess()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    QString dllPath;
    dllPath = qApp->applicationDirPath() + "/Client.dll";

    std::string dll = dllPath.toStdString();
    std::vector<CHAR> V( dll.length() + 1);
    for (int i=0;i< (int)dll.length();i++)
        V[i] = dll[i];

    if(!DetourCreateProcessWithDllEx(this->strProcess.c_str(),
                                     NULL, NULL, NULL, TRUE,
                                     CREATE_DEFAULT_ERROR_MODE,
                                     NULL, NULL, &si, &pi,
                                     &V[0], NULL)
                                     )
    {        
        return false;
    }

    WaitForInputIdle(pi.hProcess, INFINITE);

    this->hProcess = pi.hProcess;
    this->dwProcessId = pi.dwProcessId;
    return true;
}

void MapleLauncher::SetParentWindow(HWND child, HWND  hParentwnd ,int width , int heigth)
{
    LONG lStyle = NULL;
    do
    {
        lStyle = GetWindowLong(child, GWL_STYLE );
    }
    while(lStyle == NULL);

    LONG SetLong = NULL;
    do
    {
        SetLong = SetWindowLong( child, GWL_STYLE, ( lStyle & ~WS_CAPTION));
    }
    while(SetLong == NULL);

    int w;
    int h;

    if(width == 0 || heigth == 0)
    {
        RECT r;
        GetWindowRect(hParentwnd , &r);
        w = r.right - r.left;
        h = r.bottom - r.top;
    }
    else
    {
        w = width;
        h = heigth;
    }

    ::MoveWindow( child, 0, 0,w, h, TRUE);

    SetParent(child,hParentwnd);
}

HWND MapleLauncher::findWindow(unsigned long PID, QString windowName)
{
    //FIXME MapleLauncher::findWindow: Do this work? =.=
    std::wstring s_name = windowName.toStdWString();
    LPCWSTR c_name = s_name.c_str();

    HWND hWnd = FindWindow(L"",c_name);

    DWORD dwTemp;
    GetWindowThreadProcessId(hWnd, &dwTemp);
    if (dwTemp != PID) { return NULL; }
    return hWnd;
}

void MapleLauncher::changePath(QString newPath)
{
    MapleLauncher::strProcess = newPath.toStdWString();
}


void MapleLauncher::run()
{
    _b = true;
    if(!StartProcess())
    {
        emit LaunchFail();
        qDebug() << QStringLiteral("LaunchFail()");
        _b = false;
        return;
    }

    qDebug() << QStringLiteral("LaunchSuccess()");
    emit LaunchSuccess();
    _b = false;
}
