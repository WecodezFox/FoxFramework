#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "core_global.h"
#include <QObject>
#include <Windows.h>

class CORE_SYMBOL MemoryManager : public QObject
{
    Q_OBJECT

public:
    explicit MemoryManager(QObject *parent = 0);

    Q_INVOKABLE static uchar    ReadByte(ulong ulBase, ulong iOffset);
    Q_INVOKABLE static ushort  ReadShort(ulong ulBase, ulong iOffset);
    Q_INVOKABLE static ulong    ReadLong(ulong ulBase, ulong iOffset);
    Q_INVOKABLE static double ReadDouble(ulong ulBase, ulong iOffset);

    Q_INVOKABLE static bool   WriteByte(ulong ulBase, ulong iOffset,   char value);
    Q_INVOKABLE static bool  WriteShort(ulong ulBase, ulong iOffset,  short value);
    Q_INVOKABLE static bool   WriteLong(ulong ulBase, ulong iOffset,   long value);
    Q_INVOKABLE static bool WriteDouble(ulong ulBase, ulong iOffset, double value);

    Q_INVOKABLE static bool  WriteCode(ulong Addr, QString Array, int nop_count);


    template <typename T>
    static T Read (unsigned long ulBase, unsigned long iOffset);

    template <typename T>
    static bool Write(unsigned long ulBase, int iOffset, T iValue);


    static DWORD Write_Hook(char code[], DWORD Prev, DWORD Next, int nop_count);
    static BOOL Write_code(DWORD Addr, char Array[], int nop_count);
    static BOOL DetourHook(__in BOOL bState, DWORD addy, __inout PVOID *ppPointer,__in PVOID pDetour);
    static int ChartoByte(char Array[], BYTE b[]);
    static bool JumpCall(bool bJump, unsigned long ulAddress, void *Function, unsigned long ulNops);
};

template <typename T>
T MemoryManager::Read (unsigned long ulBase, unsigned long iOffset)
{
    //FIXME MemoryManager::Read: Better way?
    __try
    {
        return *(T*)(*(unsigned long*)ulBase + iOffset);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }
}

template <typename T>
bool MemoryManager::Write(unsigned long ulBase, int iOffset, T iValue)
{
    //FIXME MemoryManager::Write: Better way?
    __try
    {
        *(T*)(*(unsigned long*)ulBase + iOffset) = iValue;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
#endif // MEMORYMANAGER_H
