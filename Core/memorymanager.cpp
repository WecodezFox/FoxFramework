#include "memorymanager.h"
#include <QDebug>
#include <detours.h>

#define jmp(frm, to) (int)(((int)to - (int)frm) - 5)

MemoryManager::MemoryManager(QObject *parent) :
    QObject(parent)
{
}

uchar MemoryManager::ReadByte(ulong ulBase, ulong iOffset)
{
    return MemoryManager::Read<uchar>(ulBase,iOffset);
}

ushort MemoryManager::ReadShort(ulong ulBase, ulong iOffset)
{
    return MemoryManager::Read<ushort>(ulBase,iOffset);
}

ulong MemoryManager::ReadLong(ulong ulBase, ulong iOffset)
{
    return MemoryManager::Read<ulong>(ulBase,iOffset);
}

double MemoryManager::ReadDouble(ulong ulBase, ulong iOffset)
{
    return MemoryManager::Read<double>(ulBase,iOffset);
}

bool MemoryManager::WriteByte(ulong ulBase, ulong iOffset, char value)
{
    return MemoryManager::Write<char>(ulBase,iOffset,value);
}

bool MemoryManager::WriteShort(ulong ulBase, ulong iOffset, short value)
{
    return MemoryManager::Write<short>(ulBase,iOffset,value);
}

bool MemoryManager::WriteLong(ulong ulBase, ulong iOffset, long value)
{
    return MemoryManager::Write<long>(ulBase,iOffset,value);
}

bool MemoryManager::WriteDouble(ulong ulBase, ulong iOffset, double value)
{
    return MemoryManager::Write<double>(ulBase,iOffset,value);
}


bool MemoryManager::WriteCode(ulong Addr, QString Array, int nop_count)
{
    std::string stdString = Array.toStdString();
    char * Arr = &stdString[0];
    bool b = Write_code(Addr,Arr,nop_count);
    return b;
}

#define Asm_Byte_Code_Max 0x100

char StrArray[128] = "0123456789ABCDEF";

int MemoryManager::ChartoByte(char Array[], BYTE b[])
{
    int i, Hit = 0;

    for (i = 0; Array[i] != 0x00; i++)
    {
        if (Array[i] == 0x2A)
        {
            Array[i] = StrArray[rand() % 0x10];
        }

        if (Array[i] >= 0x61 && Array[i] <= 0x66)
        {
            b[Hit] = Array[i] - 0x57; Hit++;
        }

        else if (Array[i] >= 0x41 && Array[i] <= 0x46)
        {
            b[Hit] = Array[i] - 0x37; Hit++;

        }
        else if (Array[i] >= 0x30 && Array[i] <= 0x39)

        {
            b[Hit] = Array[i] - 0x30; Hit++;
        }
    }

    if (Hit % 2 != 0)
    {
        qDebug("Bad Lenght");
        return -1;
    }

    Hit = Hit / 2;

    for (i = 0; i<Hit; i++)
    {
        b[i] = b[i * 2] * 0x10 + b[i * 2 + 1];
    }

    return Hit;
}

DWORD MemoryManager::Write_Hook(char code[], DWORD Prev, DWORD Next, int nop_count)
{
    int i;
    BOOL Flag = FALSE;

    DWORD old;
    if (VirtualProtect((DWORD *)Prev, 7 + nop_count, PAGE_EXECUTE_READWRITE, &old) == FALSE)
    {
        qDebug() << QString("Can't VirtualProtect 0x%1").arg(Prev,0,16);
        return FALSE;
    }

    switch (*(DWORD *)code)
    {
    case 0x00706D6A: // "jmp"
        *(BYTE *)Prev = 0xE9;
        break;

    case 0x6C6C6163: // "call"
        *(BYTE *)Prev = 0xE8;
        break;

    case 0x0000656A: // "je"
        *(WORD *)Prev = 0x840F;
        Flag = TRUE;
        break;

    case 0x00656E6A: // "jne"
        *(WORD *)Prev = 0x850F;
        Flag = TRUE;
        break;

    case 0x0000626A: // "jb"
        *(WORD *)Prev = 0x820F;
        Flag = TRUE;
        break;

    case 0x0000616A: // "ja"
        *(WORD *)Prev = 0x870F;
        Flag = TRUE;
        break;

    default:
        qDebug("@Write_Hook");
        return FALSE;
        break;
    }

    *(DWORD *)(Prev + 1 + Flag) = Next - Prev - 5 - Flag;

    if (nop_count == 0)
    {
        return Prev + 5 + Flag;
    }

    for (i = 0; i < nop_count; i++)
    {
        *(BYTE *)(Prev + 5 + Flag + i) = 0x90; // "nop"
    }

    if (VirtualProtect((DWORD *)Prev, 7 + nop_count, old, &old) == FALSE)
    {
        qDebug() << QString("Can't VirtualProtect back 0x%1").arg(Prev,0,16);
        return FALSE;
    }
    return Prev + 5 + nop_count + Flag;
}

bool MemoryManager::JumpCall(bool bJump, unsigned long ulAddress, void* Function, unsigned long ulNops)
{
    __try
    {
        DWORD dwOldProtect;
        VirtualProtect((void*)ulAddress, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

        *(unsigned char*)ulAddress = (bJump ? 0xE9 : 0xE8);
        *(unsigned long*)(ulAddress + 1) = jmp(ulAddress, Function);
        memset((void*)(ulAddress + 5), 0x90, ulNops);

        VirtualProtect((void*)ulAddress, 10, dwOldProtect, &dwOldProtect);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

BOOL MemoryManager::Write_code(DWORD Addr, char Array[], int nop_count)
{
    int i, Hit = 0;
    BYTE ByteCode[Asm_Byte_Code_Max];
    DWORD old;

    if (Array != NULL && Array[0] != 0x00)
    {
        Hit = ChartoByte(Array, ByteCode);
    }

    if (VirtualProtect((DWORD *)Addr, Hit + nop_count, PAGE_EXECUTE_READWRITE, &old) == FALSE)
    {
        qDebug() << QString("Can't VirtualProtect 0x%1").arg(Addr,0,16);
        return FALSE;
    }

    if (Array != NULL && Array[0] != 0x00)
    {
        for (i = 0; i<Hit; i++)
        {
            *(BYTE *)(Addr + i) = ByteCode[i];
        }
    }

    for (i = 0; i<nop_count; i++)
    {
        *(BYTE *)(Addr + Hit + i) = 0x90; // "nop"
    }

    return VirtualProtect((DWORD *)Addr, Hit + nop_count, old, &old);
}

BOOL MemoryManager::DetourHook(BOOL bState, DWORD addy, PVOID *ppPointer, PVOID pDetour)
{
    if(bState)
        *(PVOID*)ppPointer = (PVOID)addy;

    if (DetourTransactionBegin() == NO_ERROR)
        if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR)
            if ((bState ? DetourAttach : DetourDetach)(ppPointer, pDetour) == NO_ERROR)
                if (DetourTransactionCommit() == NO_ERROR)
                    return TRUE;
    return FALSE;
}
