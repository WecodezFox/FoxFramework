#include "mainwindow.h"
#include <Windows.h>
#include <QApplication>
#include <thread>
//Detours requirement...
extern "C" __declspec(dllexport) void dummy(void){
    return;
}

//#include <QtPlugin>
//Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin)
std::thread::id localGet_id;
int main(int argc, char *argv[])
{
    Q_UNUSED(argv);
    argc =0;
    QApplication a(argc, NULL);

    MainWindow w;
    w.setThreadId(localGet_id);
    //w.show();

    return a.exec();
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        localGet_id = std::this_thread::get_id();

        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, (LPVOID)hInstance, 0, NULL);
        if (hThread != NULL)
            break;
        else
            return FALSE;
    }
    }
    return TRUE;
}
