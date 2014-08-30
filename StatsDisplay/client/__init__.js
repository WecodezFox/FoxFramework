print("importing " + __extension__);
__setupPackage__(__extension__);

StatsDisplay.client.sendToServer = function()
{
    var stats = {};
    stats.Hp = Player.Hp;
    stats.maxHp = Player.maxHp;
    stats.Mp = Player.Mp;
    stats.maxMp = Player.maxMp;
    stats.Exp = Player.Exp;
    stats.maxExp = Player.maxExp;

    var JSONText = JSON.stringify(stats);
    Socket.send(JSONText,__extension__);
}

Player.HpChanged.connect(StatsDisplay.client.sendToServer);
Player.MpChanged.connect(StatsDisplay.client.sendToServer);
Player.ExpChanged.connect(StatsDisplay.client.sendToServer);
