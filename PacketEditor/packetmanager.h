#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <QObject>
#include <Windows.h>
#include <QStringList>
#include <QMap>
#include <vector>
#include <QScriptValue>

#pragma pack(push, 1)
struct COutPacket
{
    BOOL fLoopback;
    union
    {
        LPBYTE lpbData;
        LPVOID lpvData;
        LPWORD lpwHeader;
    };
    DWORD dwcbData;
    UINT uOffset;
    BOOL fEncryptedByShanda;
};

struct CInPacket
{
    BOOL fLoopback; //0
    INT iState; //2
    union
    {
        LPVOID lpvData;
        struct
        {
            DWORD dw;
            WORD wHeader;
        } *pHeader;
        struct
        {
            DWORD dw;
            BYTE bData[0];
        } *pData;
    };
    ULONG usLength; //size of preceding struct
    ULONG usRawSeq; //pData->dw & 0xffff
    //should be DWORD, then SIZE_T, according to jony
    ULONG usDataLen; //usLength - 4
    ULONG usUnknown; //0xcc
    UINT uOffset; //sizeof(DWORD) == 4
    LPVOID lpv; //idk; 1238e0?
};


#pragma pack(pop)
class PacketManager : public QObject
{
    Q_OBJECT
public:
    explicit PacketManager(QObject *parent = 0);
    void init(QScriptValue addys, QScriptValue pointers);
    bool is_send_blocked(unsigned short header);
    bool is_recv_blocked(unsigned short header);
    void block_send(unsigned short header);
    void block_recv(unsigned short header);

    void initPacket(WORD header);
    void encodeString(QString toEncode);
    void endodeDWORD(DWORD toEncode);
    void endodeWORD(WORD toEncode);
    void encodeBYTE(BYTE toEncode);

    QString lastError();

    static bool SendPacket(QString packet , QString &Error);
signals:
    
public slots:
    bool Send(QString s);
    bool Send();

private:
    QMap<unsigned short,bool> _send_blocked;
    QMap<unsigned short,bool> _recv_blocked;
    QString m_packetString;
    QString m_error;
    QScriptValue addys;
    QScriptValue pointer;

    static bool initialized;
    static bool preparePacket(QString &input, QString &Error);
    static bool InjectAPacket(QString &input, QString &strError);
    static bool TokenizePacket(QString &input, std::vector<BYTE>& vData, QString &Error);
};
extern PacketManager* _packetManager;
#endif // PACKETMANAGER_H
