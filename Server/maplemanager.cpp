#include <QtConcurrent/QtConcurrent>

#include "maplemanager.h"
#include "maplelauncher.h"
#include "server.h"
#include "managerwidget.h"

#include "../common/ScriptFunctions.h"
#include "../common/Tags.h"

#include <QApplication>
#include <QFileDialog>

#include <QScriptEngine>
#include <QtScriptTools/QScriptEngineDebugger>
#include <QDebug>


MapleManager::MapleManager(Server* server, QWidget *widget) :
    QObject(server),
    m_status(ConnectionStatus::WAITIGN),
    m_launch_status(LaunchStatus::LAUNCHIGN),
    m_socket(0),
    m_relaunch(true),
    r_mode(MapleManager::RESIZE_MODE::AUTO)
{
    m_server = server;
    m_widget = new ManagerWidget(widget);
    m_widget->installEventFilter(this);
    connect(m_widget,SIGNAL(save()),this,SLOT(saveSettings()));
    connect(m_widget,SIGNAL(load()),this,SLOT(loadSettings()));
    connect(m_widget,SIGNAL(showDBG(bool)),this,SLOT(showDBG(bool)));
    connect(m_widget,SIGNAL(relaunch(bool)),this,SLOT(setRelaunch(bool)));
    connect(m_widget,SIGNAL(s_changeTabName(QString)),this,SLOT(changeTabName(QString)));

    m_engine = new QScriptEngine(this);

    QScriptEngineDebugger *dbg = new QScriptEngineDebugger(this);
    dbg->attachTo(m_engine);
    m_widget->regiterDebugger((QWidget*)dbg->standardWindow());

    ScriptFunctions::registerFuncions(m_engine);
    m_engine->globalObject().setProperty(API::Path,qApp->applicationDirPath());
    m_engine->globalObject().setProperty(API::GUI,m_engine->newQObject(m_widget));
    m_engine->globalObject().setProperty(API::Socket,m_engine->newQObject(this));
}

MapleManager::~MapleManager()
{
    m_widget->deleteLater();
}

void MapleManager::init()
{    
    //Load script extensions

    const QStringList availableExtensions = m_engine->availableExtensions();

    //Load "Qt" extensions first
    for(auto it = availableExtensions.cbegin(); it!= availableExtensions.cend(); ++it)
    {
        if((*it).startsWith("qt",Qt::CaseInsensitive))
        {
            QScriptValue imported = m_engine->importExtension(*it);
            if(imported.isUndefined())
            {
                if(m_engine->hasUncaughtException())
                    qDebug() << m_engine->uncaughtException().toString();
            }
        }
    }

    //Load custom "server" extensions
    for(auto it = availableExtensions.cbegin(); it!= availableExtensions.cend(); ++it)
    {
        if(!(*it).contains(Packets::client_tag))
        {
            QScriptValue imported = m_engine->importExtension(*it);
            if(imported.isUndefined())
            {
                if(m_engine->hasUncaughtException())
                    qDebug() << m_engine->uncaughtException().toString();
            }
        }
    }
}

void MapleManager::send(QString message, QString extension)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj[Packets::header_tag] = (int)Packets::Headers::EXTENSION;
    obj[Packets::extension_tag] = extension;
    obj[Packets::content_tag] = message;
    doc.setObject(obj);
    m_server->send(m_socket,doc);
}

void MapleManager::launchMaple()
{
    static char tries = 0;
    m_launch_status = LaunchStatus::LAUNCHIGN;
    //Avoid infinite loop of failed launchs
    if(++tries < 5)
        launch();
}

void MapleManager::reLaunchMaple()
{
    m_launch_status = LaunchStatus::RELAUNCHIGN;
    launch();
}

void MapleManager::embedMaple()
{
    if(this->m_dwProcessId != NULL)
    {
        m_Future = QtConcurrent::run(&MapleLauncher::SetParentWindow,m_hProcesswnd,m_widget->getMapleContainer(),0,0);
        m_FutWatcher.setFuture(m_Future);
        connect(&m_FutWatcher,&QFutureWatcher<void>::resultReadyAt,[=](int) {
            resizeMaple(r_mode);
        });
    }
    else
    {
        QTimer::singleShot(800,this,SLOT(embedMaple()));
    }    
}

void MapleManager::dispatchPackets(QJsonObject obj)
{    
    Packets::Headers header = static_cast<Packets::Headers>(obj.value(Packets::header_tag).toInt());
    switch (header) {
    case Packets::Headers::EMBED:
        this->m_hProcesswnd = (HWND)obj.value("handle").toInt();
        embedMaple();
        syncNewMaple();
        break;
    case Packets::Headers::EXTENSION:
        handleExtensionPacket(obj);
        break;
    case Packets::Headers::DBG:
        m_widget->toogleReload(obj.value("state").toBool());
        break;
    default:
        qWarning() << QStringLiteral("Header not implemented... yet?");
        break;
    }
}

void MapleManager::launchFailed()
{
    qWarning() << QStringLiteral("launch Failed!");
    sender()->deleteLater();
    if(this->m_relaunch)
        launchMaple();
}

void MapleManager::launchSuccess()
{
    qDebug() << QStringLiteral("launch Success!");
    MapleLauncher *launcher = qobject_cast<MapleLauncher*>(sender());
    if(launcher)
    {
        this->m_dwProcessId = launcher->dwProcessId;

        m_widget->setDwProcessId(this->m_dwProcessId);

        emit changeTabText(QString("[%1]").arg(m_dwProcessId,4,10,QChar('0')).toUpper());
        launcher->deleteLater();
    }
}

MapleManager::RESIZE_MODE MapleManager::getR_mode() const
{
    return r_mode;
}

void MapleManager::setR_mode(const RESIZE_MODE &value)
{
    if(r_mode != value)
    {
        r_mode = value;
        resizeMaple(r_mode);
    }
}


QWidget *MapleManager::widget() const
{
    return (QWidget*)m_widget;
}

void MapleManager::launch()
{
    this->m_status = ConnectionStatus::WAITIGN;
    this->m_dwProcessId = NULL;
    this->m_hProcesswnd = NULL;

    MapleLauncher * launcher = new MapleLauncher(0,L"C:/Program Files/NEXON/Europe MapleStory/MapleStory.exe");//FIXME maple path

    /* Fail handle */
    connect(launcher, &MapleLauncher::LaunchFail, this, &MapleManager::launchFailed , Qt::QueuedConnection);

    /* Success handle*/
    connect(launcher, &MapleLauncher::LaunchSuccess, this, &MapleManager::launchSuccess, Qt::QueuedConnection);

    launcher->start();
}

void MapleManager::resizeMaple(MapleManager::RESIZE_MODE r_mode)
{

    auto size = m_widget->getMapleWidget()->size(); //AUTO mode

    switch (r_mode) {
    case MapleManager::RESIZE_MODE::W_800:
        size = QSize(800,600);
        break;
    case MapleManager::RESIZE_MODE::W_1024:
        size = QSize(1024,768);
        break;
    case MapleManager::RESIZE_MODE::W_1366:
        size = QSize(1366,768);
        break;
    }
    ::MoveWindow( m_hProcesswnd, 0, 0, size.width(), size.height(), TRUE);
}

void MapleManager::handleExtensionPacket(QJsonObject object)
{
    QString content = object.value(Packets::content_tag).toString();
    QString extension = object.value(Packets::extension_tag).toString();
    extension.replace(Packets::client_tag, Packets::server_tag);
    QStringList ext = extension.split(".");
    if(!ext.isEmpty())
    {       
        QScriptValue function = m_engine->globalObject().property(ext.first());
        for(auto i = 1; i < ext.length(); ++i)
            function = function.property(ext.at(i));
        function = function.property(Packets::handler_tag);
        function.call(QScriptValue(),QScriptValueList() << content );
    }
}

void MapleManager::syncNewMaple()
{
    ScriptFunctions::callAllImportsFunction(m_engine,API::Sync_func);
}

void MapleManager::saveSettings()
{
    QString target = QFileDialog::getSaveFileName(this->m_widget,
                                                  tr("Save"),
                                                  qApp->applicationDirPath(),
                                                  tr("Ini files (*.ini)"));
    if(!target.isEmpty())
    {
        QSettings setting(target,QSettings::IniFormat);
        QMap<QString, QScriptValue> ext_string = ScriptFunctions::callAllImportsFunction(m_engine,API::Save_func);
        for(auto it = ext_string.cbegin(); it != ext_string.cend(); ++it)
            setting.setValue(it.key(),it.value().toString());
    }
}

void MapleManager::loadSettings()
{
    QString target = QFileDialog::getOpenFileName(this->m_widget,
                                                  tr("Save"),
                                                  qApp->applicationDirPath(),
                                                  tr("Ini files (*.ini)"));
    if(!target.isEmpty())
    {
        QSettings setting(target,QSettings::IniFormat);
        QStringList AllKeys = setting.allKeys();
        QMap<QString,QScriptValue> args;
        for(auto it = AllKeys.cbegin(); it != AllKeys.cend(); ++it)
        {
            const QString s = *it;
            args.insert(s,QScriptValue(setting.value(s).toString()));
        }
        ScriptFunctions::callAllImportsFunction(m_engine,API::Load_func,args);
    }

}

void MapleManager::showDBG(bool state)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj[Packets::header_tag] = (int)Packets::Headers::DBG;
    obj["state"] = state;
    doc.setObject(obj);
    m_server->send(m_socket,doc);
}

bool MapleManager::eventFilter(QObject * target, QEvent *event)
{
    if (target == m_widget)
    {
        if (event->type() == QEvent::Resize && r_mode == MapleManager::RESIZE_MODE::AUTO)
        {
            auto size = m_widget->getMapleWidget()->size();
            ::MoveWindow( m_hProcesswnd, 0, 0, size.width(), size.height(), TRUE);
        }
    }
    return QObject::eventFilter(target,event);
}

int MapleManager::getSocket() const
{
    return m_socket;
}

void MapleManager::setSocket(int value)
{
    m_socket = value;
}

MapleManager::ConnectionStatus MapleManager::status() const
{
    return m_status;
}

void MapleManager::setStatus(const MapleManager::ConnectionStatus &status)
{
    m_status = status;
}

bool MapleManager::getRelaunch() const
{
    return m_relaunch;
}

void MapleManager::setRelaunch(bool relaunch)
{
    m_relaunch = relaunch;
}

void MapleManager::changeTabName(QString s)
{
    emit changeTabText(s);
}
