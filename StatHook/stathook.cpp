#include "stathook.h"

#include <Windows.h>

#include <QScriptEngine>
#include <QDebug>

#include "../Core/player.h"
#include "../Core/memorymanager.h"
#include "patternfinder.h"

Player * player = 0;
QScriptValue pointers;
QScriptValue addys;

typedef void (__fastcall* CUIStatusBar__SetNumberValue_t)(void* lpvEcx, void* lpvEdx, int iCurrentHp, int iMaximumHp, int iCurrentMp, int iMaximumMp, __int64 iCurrentExp, __int64 iMaximumExp, int iTempExp, int unk1, int unk2);
CUIStatusBar__SetNumberValue_t CUIStatusBar__SetNumberValue = NULL;
void __fastcall CUIStatusBar__SetNumberValue__Hook(void* lpvEcx, void* lpvEdx, int iCurrentHp, int iMaximumHp, int iCurrentMp, int iMaximumMp, __int64 iCurrentExp, __int64 iMaximumExp, int iTempExp, int unk1, int unk2)
{
    player->setMaxHp(iMaximumHp);
    player->setHp   (iCurrentHp);
    player->setMaxMp(iMaximumMp);
    player->setMp   (iCurrentMp);
    player->setMaxExp(iMaximumExp);
    player->setExp(iCurrentExp);
    return CUIStatusBar__SetNumberValue(lpvEcx, lpvEdx, iCurrentHp, iMaximumHp, iCurrentMp, iMaximumMp, iCurrentExp, iMaximumExp, iTempExp, unk1, unk2);
}

typedef void (__fastcall* CUIStatusBar__SetStatusValue)(void*,void*, int lv, int nJob, LPCSTR sCharName, int unk1, int unk2);
CUIStatusBar__SetStatusValue Maple__SetStatusValue = NULL;
void __fastcall Hook__SetStatusValue(void* lpecx,void* lpEdx,  int lv, int nJob, LPCSTR sCharName, int unk1, int unk2)
{
    player->setLvl(lv);
    player->setJobCode(nJob);
    return Maple__SetStatusValue(lpecx,lpEdx,lv,nJob, sCharName,  unk1,  unk2);
}

typedef void (__fastcall* CVecCtrl__SetActive_t)(void* lpvEcx, void* lpvEdx, int bActive, int x, int y, int vx, int vy, int nMoveAction, void* lpvFoothold);
static CVecCtrl__SetActive_t CVecCtrl__SetActive = NULL;
void __fastcall _SetActive_hook(void* lpvEcx, void* lpvEdx, int bActive, int x, int y, int vx, int vy, int nMoveAction, void* lpvFoothold)
{
    static const ulong Sp_ret = addys.property("CVecCtrl_SetActive_ret").toUInt32();
    if (_ReturnAddress() == reinterpret_cast<void*>(Sp_ret))
    {
        static const ulong CUIMiniMap_Ptr = pointers.property("CUIMiniMap_Ptr").toUInt32();
        static const ulong CUIMiniMap_ID_Offset = pointers.property("CUIMiniMap_ID_Offset").toUInt32();
        ulong mapID = MemoryManager::Read<ulong>(CUIMiniMap_Ptr, CUIMiniMap_ID_Offset);

        player->setSpawnX(x);
        player->setSpawnY(y);
        player->setEnteringMap(mapID);
        Sleep(100);
        player->setMap(mapID);
        return CVecCtrl__SetActive(lpvEcx, lpvEdx, bActive, player->spawnX(), player->spawnY(), vx, vy, nMoveAction, lpvFoothold);
    }
    return CVecCtrl__SetActive(lpvEcx, lpvEdx, bActive, x, y, vx, vy, nMoveAction, lpvFoothold);
}

void StatHook::setHooks(QScriptEngine *engine)
{    
    QObject* ptr = engine->globalObject().property("Player").toQObject();
    if(player = qobject_cast<Player*>(ptr))
    {
        addys = engine->globalObject().property("Addys");
        const ulong CUIStatusBar__SetNumberValue_addy = addys.property("CUIStatusBar_SetNumberValue").toUInt32();
        const ulong CUIStatusBar_SetStatusValue_addy = addys.property("CUIStatusBar_SetStatusValue").toUInt32();
        const ulong CVecCtrl_SetActive_addy = addys.property("CVecCtrl_SetActive").toUInt32();

        if(MemoryManager::DetourHook(TRUE,CUIStatusBar__SetNumberValue_addy,(PVOID *)&CUIStatusBar__SetNumberValue,CUIStatusBar__SetNumberValue__Hook))
            qDebug() << QString("Hooked CUIStatusBar_SetNumberValue @0x%1").arg(CUIStatusBar__SetNumberValue_addy,8,16,QChar('0'));
        else
            qWarning() << QStringLiteral("Can't hook CUIStatusBar_SetNumberValue");

        if(MemoryManager::DetourHook(TRUE,CUIStatusBar_SetStatusValue_addy,(PVOID *)&Maple__SetStatusValue,Hook__SetStatusValue))
            qDebug() << QString("Hooked CUIStatusBar_SetStatusValue @0x%1").arg(CUIStatusBar_SetStatusValue_addy,8,16,QChar('0'));
        else
            qWarning() << QStringLiteral("Can't hook CUIStatusBar_SetStatusValue");

        if(MemoryManager::DetourHook(TRUE,CVecCtrl_SetActive_addy,(PVOID*)&CVecCtrl__SetActive,_SetActive_hook))
            qDebug() << QString("Hooked CVecCtrl_SetActive @0x%1").arg(CVecCtrl_SetActive_addy,8,16,QChar('0'));
        else
            qWarning() << QStringLiteral("Can't hook CVecCtrl_SetActive");
    }
    pointers = engine->globalObject().property("Pointers");
}

//############################# PLUGIN CODE ###########################################

QStringList StatHook::keys() const
{
    QStringList list;
    list << QStringLiteral("statsHook");
    list << QStringLiteral("statsHook.client");
    return list;
}

void StatHook::initialize(const QString &key, QScriptEngine * engine)
{
    if (key == QStringLiteral("statsHook"))
    {
        setupPackage(key,engine);
    }
    else if (key == QStringLiteral("statsHook.client"))
    {       
        setupPackage(key,engine);
        engine->globalObject()
                .property("statsHook")
                .property("client")
                .setProperty("postBypass", engine->newFunction(&StatHook::postBypass));
    }
    else
    {
        Q_ASSERT_X(false, "PlugTest::initialize", qPrintable(key));
    }
}

QScriptValue StatHook::postBypass(QScriptContext * /*context*/, QScriptEngine *engine)
{
    setHooks(engine);
    return QScriptValue();
}

