#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "core_global.h"
#include "singleton.h"

class QScriptEngine;

class CORE_SYMBOL Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp NOTIFY maxHpChanged)
    Q_PROPERTY(int Hp READ Hp WRITE setHp NOTIFY HpChanged)

    Q_PROPERTY(int maxMp READ maxMp WRITE setMaxMp NOTIFY maxMpChanged)
    Q_PROPERTY(int Mp READ Mp WRITE setMp NOTIFY MpChanged)

    Q_PROPERTY(int maxExp READ maxExp WRITE setMaxExp NOTIFY maxExpChanged)
    Q_PROPERTY(int Exp READ Exp WRITE setExp NOTIFY ExpChanged)

    Q_PROPERTY(int Lvl READ Lvl WRITE setLvl NOTIFY LvlChanged)

    Q_PROPERTY(int enteringMap READ getEnteringMap WRITE setEnteringMap NOTIFY enteringMap)
    Q_PROPERTY(int map READ map WRITE setMap NOTIFY mapChanged)

    Q_PROPERTY(int spawnX READ spawnX WRITE setSpawnX NOTIFY spawnXChanged)
    Q_PROPERTY(int spawnY READ spawnY WRITE setSpawnY NOTIFY spawnYChanged)

    Q_PROPERTY(QString Name READ Name WRITE setName NOTIFY NameChanged)
    Q_PROPERTY(int JobCode READ JobCode WRITE setJobCode NOTIFY JobCodeChanged)
public:
    Player(QObject *parent = 0);

    int maxHp() const;
    int Hp() const;

    int maxMp() const;
    int Mp() const;

    int Lvl() const;

    int getEnteringMap() const;
    int map() const;

    int spawnX() const;
    int spawnY() const;

    QString Name() const;

    int JobCode() const;

    int maxExp() const;
    int Exp() const;

signals:

    void maxHpChanged(int arg);
    void HpChanged(int arg);

    void maxMpChanged(int arg);
    void MpChanged(int arg);

    void LvlChanged(int arg);

    void enteringMap(int arg);
    void mapChanged(int arg);

    void spawnXChanged(int arg);
    void spawnYChanged(int arg);

    void NameChanged(QString arg);

    void JobCodeChanged(int arg);

    void maxExpChanged(int arg);

    void ExpChanged(int arg);

public slots:

    void setMaxHp(int arg);
    void setHp(int arg);

    void setMaxMp(int arg);
    void setMp(int arg);

    void setLvl(int arg);

    void setEnteringMap(int arg);
    void setMap(int arg);

    void setSpawnX(int arg);
    void setSpawnY(int arg);

    void setName(QString arg);

    void setJobCode(int arg);

    void setMaxExp(int arg);

    void setExp(int arg);

protected:
    Player(Player const&);
    Player& operator=(Player const&);

private:

    int m_maxHp;
    int m_Hp;
    int m_maxMP;
    int m_Mp;
    int m_Lvl;

    QScriptEngine* engine;

    ulong sp_ret;
    int m_enteringMap;
    int m_map;
    int m_spawnX;
    int m_spawnY;
    QString m_Name;
    int m_JobCode;
    int m_maxExp;
    int m_Exp;
};

#endif // PLAYER_H
