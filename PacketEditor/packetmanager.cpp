#include "packetmanager.h"
#include "../Core/memorymanager.h"
#include <QMessageBox>
#include <QDebug>
#include <tlhelp32.h>

bool PacketManager::initialized = false;
DWORD SendFunction = 0/*x00470610*/;
DWORD CClientSocket__SendPacketRet = 0/*x0047061A*/;
DWORD dwMainThreadID = 0;
DWORD CClientSocket_ptr = 0;

void __declspec(naked) SendPacketHookAsm()
{
    __asm
    {
        cmp [dwMainThreadID],0
        jnz alreadyKnowMainThreadID
        push eax
        mov eax,  fs:[0x18]
        mov eax, [eax+0x006B8]
        mov [dwMainThreadID], eax
        pop eax

        alreadyKnowMainThreadID:
        push eax
        mov eax,  fs:[0x18]
        mov eax, [eax+0x006B8]
        cmp [dwMainThreadID], eax
        jz  Continue
        mov eax,  [dwMainThreadID]
        mov fs:[0x06b8], eax
        Continue:
        pop eax

        mov eax,[esp+04]
        mov ecx,[CClientSocket_ptr]
        mov ecx,[ecx]
        jmp CClientSocket__SendPacketRet
    }
}

PacketManager::PacketManager(QObject *parent) :
   QObject(parent)
{
}

void PacketManager::init(QScriptValue addys, QScriptValue pointers)
{
    this->addys = addys;
    this->pointer = pointers;

    if(!PacketManager::initialized)
    {
        CClientSocket_ptr = pointers.property("CClientSocket_ptr").toUInt32();
        SendFunction = addys.property("SendFunction").toUInt32();
        CClientSocket__SendPacketRet = addys.property("SendPacketRet").toUInt32();

        if(MemoryManager::JumpCall(true,SendFunction,SendPacketHookAsm,5))
        {
            qDebug() << QString("Hooked SendPacket @0x%1").arg(SendFunction,8,16,QChar('0'));
            PacketManager::initialized = true;
        }
        else
        {
            qDebug() << QStringLiteral("Can't hook SendPacket");
        }
    }
}

BOOL SendP(char Packet_STR[]){
    int Packet_Size;
    BYTE Packet[4096];

    Packet_Size = MemoryManager::ChartoByte(Packet_STR, Packet);

    if(Packet_Size == -1 || Packet_Size < 2){
        return FALSE;
    }

    //created by airride^^
    _asm{
        lea eax,[Packet]
        mov ebx,[Packet_Size]
        push 0x00
        push ebx
        push eax
        push 0x00
        push esp
        call SendFunction
    }

    return TRUE;
}


bool PacketManager::SendPacket(QString packet, QString &Error)
{
    if(PacketManager::preparePacket(packet, Error))
        if(PacketManager::InjectAPacket(packet, Error))
            return true;
    return false;
}

bool PacketManager::preparePacket(QString& input, QString& Error)
{
    QString aux = input.remove(" ");
    //timestamp here?
    if ( (aux.length()%2) != 0)
    {
        Error="Check packet length!";
        return false;
    }
    for (int i=0;i<aux.length();i++)
    {
        if (aux.at(i) >= QChar('a') && aux.at(i) <= QChar('f'))
            continue;
        if (aux.at(i) >= QChar('A') && aux.at(i) <= QChar('F'))
            continue;
        if (aux.at(i) >= QChar('0') && aux.at(i) <= QChar('9'))
            continue;
        if (aux.at(i) == '*')
        {
            aux[i] = QString().sprintf("%X",qrand()%16).at(0);
            continue;
        }
        Error="Found invalid character in packet!";
        return false;
    }

    int ilength = (aux.length()-2)/2;
    for(int n=0, i=2 ; n<ilength; i+=3 , n++)
    {
        aux.insert(i," ");
    }
    input = aux;
    return true;
}

bool PacketManager::InjectAPacket(QString& strPacket, QString& strError)
{
    //strError.clear();

    //remove useless spaces
    strPacket.trimmed();

    //sanity check
    if (strPacket.isEmpty())
    {
        strError = "Please enter a packet.";
        return false;
    }

    qDebug() << strPacket;
    strPacket = strPacket.replace(" ","");

    std::string stdString = strPacket.toStdString();
    char * c_str = &stdString[0];

    return SendP(c_str);
}

bool PacketManager::TokenizePacket(QString &input, std::vector<BYTE> &vData, QString &Error)
{
    QStringList tokens = input.split(" ");
    QStringListIterator it(tokens);
    while(it.hasNext())
    {
        QString token = it.next();
        bool ok;
        unsigned int data = token.toUInt(&ok,16);
        if(ok)
            vData.push_back(static_cast<BYTE>(data));
        else
        {
            Error = "Lexical error";
            return false;
        }
    }
    return true;
}

void PacketManager::encodeString(QString toEncode)
{
    QTextStream s(&m_packetString);
    WORD siz = toEncode.size();

    endodeWORD(siz);

    for(int i=0;i< siz ; i++)
        s << QString().sprintf("%02X",static_cast<BYTE>(toEncode.at(i).toLatin1()));
}

void PacketManager::endodeDWORD(DWORD toEncode)
{
    //AABBCCDD ->DDCCBBAA
    QTextStream s(&m_packetString);
    WORD w1 = HIWORD(toEncode);
    WORD w2 = LOWORD(toEncode);

    s << QString().sprintf("%02X",LOBYTE(w2));
    s << QString().sprintf("%02X",HIBYTE(w2));
    s << QString().sprintf("%02X",LOBYTE(w1));
    s << QString().sprintf("%02X",HIBYTE(w1));
}

void PacketManager::endodeWORD(WORD toEncode)
{
    //AABBCCDD ->DDCCBBAA
    QTextStream s(&m_packetString);
    s << QString().sprintf("%02X",LOBYTE(toEncode));
    s << QString().sprintf("%02X",HIBYTE(toEncode));
}

void PacketManager::encodeBYTE(BYTE toEncode)
{
    QTextStream s(&m_packetString);
    s << QString().sprintf("%02X",toEncode);
}

QString PacketManager::lastError()
{
    return m_error;
}

bool PacketManager::Send(QString s)
{
    return PacketManager::SendPacket(s,m_error);
}

bool PacketManager::Send()
{
    bool b = PacketManager::SendPacket(m_packetString,m_error);
    m_packetString = "";
    return b;
}

bool PacketManager::is_send_blocked(unsigned short header)
{
    return _send_blocked.contains(header);
}

bool PacketManager::is_recv_blocked(unsigned short header)
{
    return _recv_blocked.contains(header);
}

void PacketManager::block_send(unsigned short header)
{
    _send_blocked.insert(header,true);
}

void PacketManager::block_recv(unsigned short header)
{
    _recv_blocked.insert(header,true);
}

