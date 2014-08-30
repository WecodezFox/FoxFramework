print("importing " + __extension__);
__setupPackage__(__extension__);

var file = new QFile(getFolder() + "form.ui");
file.open(new QIODevice.OpenMode(QIODevice.ReadOnly));

var loader = new QUiLoader();

StatsDisplay.server.widget = loader.load(file);
var w = StatsDisplay.server.widget;



var childList = w.children();

for(var i=0; i< childList.length; i++){

    var name = childList[i].accessibleName;
         if(name === "hpBar" ) {StatsDisplay.server.hpBar  = childList[i];}
    else if(name === "mpBar" ) {StatsDisplay.server.mpBar  = childList[i];}
    else if(name === "expBar") {StatsDisplay.server.expBar = childList[i];}
    //others here
}


GUI.registerWidget("Stats", StatsDisplay.server.widget);

StatsDisplay.server.HandlePackets = function(message)
{
    var stats = JSON.parse(message);

    StatsDisplay.server.hpBar.maximum = stats.maxHp;
    StatsDisplay.server.hpBar.value = stats.Hp;

    StatsDisplay.server.mpBar.maximum = stats.maxMp;
    StatsDisplay.server.mpBar.value = stats.Mp;

    StatsDisplay.server.expBar.maximum = stats.maxExp;
    StatsDisplay.server.expBar.value = stats.Exp;
}
