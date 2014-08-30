#include "player.h"
#include <Windows.h>
#include <detours.h>
#include <QScriptEngine>
#include <QtWidgets/qapplication.h>


Player::Player(QObject *parent) :
    QObject(parent),
    m_maxHp(0),
    m_Hp(0),
    m_maxMP(0),
    m_Mp(0),
    m_Lvl(0),
    m_map(0)
{  
}

int Player::maxHp() const
{
    return m_maxHp;
}

int Player::Hp() const
{
    return m_Hp;
}

int Player::maxMp() const
{
    return m_maxMP;
}

int Player::Mp() const
{
    return m_Mp;
}

int Player::Lvl() const
{
    return m_Lvl;
}

void Player::setMaxHp(int arg)
{
    if (m_maxHp != arg) {
        m_maxHp = arg;
        emit maxHpChanged(arg);
    }
}

void Player::setHp(int arg)
{
    if (m_Hp != arg) {
        m_Hp = arg;
        emit HpChanged(arg);
    }
}

void Player::setMaxMp(int arg)
{
    if (m_maxMP != arg) {
        m_maxMP = arg;
        emit maxMpChanged(arg);
    }
}

void Player::setMp(int arg)
{
    if (m_Mp != arg) {
        m_Mp = arg;
        emit MpChanged(arg);
    }
}

void Player::setLvl(int arg)
{
    if (m_Lvl != arg) {
        m_Lvl = arg;
        emit LvlChanged(arg);
    }
}

void Player::setEnteringMap(int arg)
{
    m_enteringMap = arg;
    emit enteringMap(arg);
    QApplication::processEvents();
}

void Player::setMap(int arg)
{
    if (m_map != arg) {
        m_map = arg;
        m_enteringMap = 0;
        emit mapChanged(arg);
    }
}

void Player::setSpawnX(int arg)
{
    if (m_spawnX != arg) {
        m_spawnX = arg;
        emit spawnXChanged(arg);
    }
}

void Player::setSpawnY(int arg)
{
    if (m_spawnY != arg) {
        m_spawnY = arg;
        emit spawnYChanged(arg);
    }
}

void Player::setName(QString arg)
{
    if (m_Name != arg) {
        m_Name = arg;
        emit NameChanged(arg);
    }
}

void Player::setJobCode(int arg)
{
    if (m_JobCode != arg) {
        m_JobCode = arg;
        emit JobCodeChanged(arg);
    }
}

void Player::setMaxExp(int arg)
{
    if (m_maxExp != arg) {
        m_maxExp = arg;
        emit maxExpChanged(arg);
    }
}

void Player::setExp(int arg)
{
    if (m_Exp != arg) {
        m_Exp = arg;
        emit ExpChanged(arg);
    }
}

int Player::getEnteringMap() const
{
    return m_enteringMap;
}

int Player::map() const
{
    return m_map;
}

int Player::spawnX() const
{
    return m_spawnX;
}

int Player::spawnY() const
{
    return m_spawnY;
}

QString Player::Name() const
{
    return m_Name;
}

int Player::JobCode() const
{
    return m_JobCode;
}

int Player::maxExp() const
{
    return m_maxExp;
}

int Player::Exp() const
{
    return m_Exp;
}

