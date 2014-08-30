#include "keyboard.h"
#include <QMutexLocker>

DWORD dwDLLFunc = (DWORD)GetProcAddress(LoadLibraryW(L"User32.dll" ), "PostMessageA") + 5;

__declspec(naked) BOOL WINAPI PostMsg(HWND hWnd, unsigned int Msg, WPARAM wParam, LPARAM lParam)
{
    __asm
    {
        mov edi, edi
        push ebp
        mov ebp, esp
        jmp dword ptr ds:[dwDLLFunc]
    }
}

KeyBoard::KeyBoard(QObject *parent) :
    QObject(parent),
    hProcesswnd(NULL)
{
}

void KeyBoard::SendKey(unsigned int uiKey)
{
    QMutexLocker lock(&mutex);
    LPARAM lParam = (MapVirtualKey(uiKey, 0) << 16) + 1;
    PostMsg(hProcesswnd, WM_KEYDOWN, uiKey, lParam);
    PostMsg(hProcesswnd, WM_KEYUP, uiKey, NULL);
}

void KeyBoard::HoldKey(unsigned int uiKey, int iHoldingDelay)
{
    QMutexLocker lock(&mutex);
    LPARAM lParam = (MapVirtualKey(uiKey, 0) << 16) + 1;
    PostMsg(hProcesswnd, WM_KEYDOWN, uiKey, lParam);
    Sleep(iHoldingDelay);
    PostMsg(hProcesswnd, WM_KEYUP, uiKey, NULL);
}

void KeyBoard::SendKey(HWND hProcesswnd, unsigned int uiKey)
{
    LPARAM lParam = (MapVirtualKey(uiKey, 0) << 16) + 1;
    PostMsg(hProcesswnd, WM_KEYDOWN, uiKey, lParam);
    PostMsg(hProcesswnd, WM_KEYUP, uiKey, NULL);
}

void KeyBoard::HoldKey(HWND hProcesswnd, unsigned int uiKey, int iHoldingDelay)
{
    LPARAM lParam = (MapVirtualKey(uiKey, 0) << 16) + 1;
    PostMsg(hProcesswnd, WM_KEYDOWN, uiKey, lParam);
    Sleep(iHoldingDelay);
    PostMsg(hProcesswnd, WM_KEYUP, uiKey, NULL);
}
HWND KeyBoard::getHProcesswnd() const
{
    return hProcesswnd;
}

void KeyBoard::setHProcesswnd(const HWND &value)
{
    hProcesswnd = value;
}

