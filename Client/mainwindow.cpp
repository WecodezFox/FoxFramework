#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include <QCloseEvent>
#include <Windows.h>

#include "botmanager.h"
#include "injector.h"
#include "patternfinder.h"
#include "player.h"
#include "keyboard.h"
#include "memorymanager.h"


#include "../Common/Tags.h"
#include "../Common/ScriptFunctions.h"

#include <sstream>
MainWindow * win;
QtMessageHandler defaultHandler;

void LogHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QByteArray lMessage = message.toLocal8Bit();

    QString text;

    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug (%1): %2").arg(context.function).arg(lMessage.constData());
        break;

    case QtWarningMsg:
        text = QString("Warning (%1): %2").arg(context.function).arg(lMessage.constData());
        break;

    case QtCriticalMsg:
        text = QString("Critical (%1): %2").arg(context.function).arg(lMessage.constData());
        break;
    case QtFatalMsg:
        text = QString("Fatal (%1): %2").arg(context.function).arg(lMessage.constData());
        win->Log(text);
        abort();
    }

    win->Log(text);

    defaultHandler(type, context, message);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    connection(this)
{
    //qDebug() hack around
    win = this;
    defaultHandler = qInstallMessageHandler(LogHandler);
    //end hack

    ui->setupUi(this);

    connect(&connection,SIGNAL(ReadReady(QJsonObject)),this,SLOT(readSocket(QJsonObject)));

    connect(&connection,&Connection::ServerGoAway, []{::TerminateProcess(::GetCurrentProcess(), 0);});    

    //safe logging from other threads
    connect(this,SIGNAL(LogMe(QString)),this,SLOT(displayLog(QString)),Qt::QueuedConnection);

   // setPath("C:/FoxFrame/build-FoxFramework_2_0/Desktop_Qt_5_3_0_MSVC2013_32bit/output_d");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Log(QString s)
{
    emit LogMe(s);
}

void MainWindow::setThreadId(std::thread::id localGet_id)
{
    std::stringstream ss;
    ss << localGet_id;
    id = std::stoull(ss.str());
    qDebug() << "Main Thread Id" << id;
}

void MainWindow::init()
{    
    this->setWindowTitle(QString("[%1] Client Debugger").arg(GetCurrentProcessId(),4,10,QChar('0')).toUpper());
    m_FutureBot = QtConcurrent::run(m_bot,&BotManager::init);
    m_BotWatcher.setFuture(m_FutureBot);

    //Get window handle before embed
    connect(&m_BotWatcher,&QFutureWatcher<bool>::resultReadyAt,[=](int) {
        bool b = m_FutureBot.result();

        keyBoard->setHProcesswnd(m_bot->getHProcesswnd());
        callPostBypassFunctions();

        QJsonDocument doc;
        QJsonObject obj;
        obj[Packets::header_tag] = (int)Packets::Headers::EMBED;
        obj["success"] = b;
        obj["handle"] = (int)m_bot->getHProcesswnd();
        doc.setObject(obj);
        connection.send(doc);
    } );
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj[Packets::header_tag] = (int)Packets::Headers::DBG;
    obj["state"] = false;
    doc.setObject(obj);
    connection.send(doc);
    e->ignore();
    this->hide();
}

void MainWindow::readSocket(QJsonObject obj)
{
    Packets::Headers header = static_cast<Packets::Headers>(obj.value(Packets::header_tag).toInt());
    switch (header) {
    case Packets::Headers::PATH:
        setPath(obj.value("path").toString());
        break;
    case Packets::Headers::EXTENSION:
        handleExtensionPacket(obj);
        break;
    case Packets::Headers::DBG:
        this->setVisible(obj.value("state").toBool());
        break;
    default:
        qWarning() << QStringLiteral("Header not implemented... yet?");
        break;
    }
}

void MainWindow::displayLog(QString s)
{
    ui->textBrowser->append(s);
}

void MainWindow::setPath(const QString path)
{
    QStringList paths = qApp->libraryPaths();
    paths << path;
    qApp->setLibraryPaths(paths);

    engine = new QScriptEngine(this);

    QScriptEngineDebugger *dbg = new QScriptEngineDebugger(this);
    dbg->attachTo(engine);
    ui->splitter->addWidget(dbg->standardWindow());

    keyBoard = new KeyBoard(this);
    m_bot = new BotManager(engine,this);

    engine->globalObject().setProperty("TID",id);
    engine->globalObject().setProperty(API::Path,path);
    engine->globalObject().setProperty(API::Socket,engine->newQObject(&connection));
    engine->globalObject().setProperty(API::KeyBoard_Obj,engine->newQObject(keyBoard));
    engine->globalObject().setProperty(API::Bot_Obj,engine->newQObject(m_bot));
    engine->globalObject().setProperty(API::Injector_Obj,engine->newQObject(new Injector(this)));
    engine->globalObject().setProperty(API::AoBScanner_Obj,engine->newQObject(new PatternFinder(this)));
    engine->globalObject().setProperty(API::Player_Obj,engine->newQObject(new Player(this)));
    engine->globalObject().setProperty(API::Addys_Obj,engine->newQObject(new QObject(this)));
    engine->globalObject().setProperty(API::Pointers_Obj,engine->newQObject(new QObject(this)));
    engine->globalObject().setProperty(API::Memory_Obj,engine->newQObject(new MemoryManager(this)));

    ScriptFunctions::registerFuncions(engine);

    //Load script extensions
    const QStringList availableExtensions = engine->availableExtensions();

    //Load "Qt" extensions first
    for(auto it = availableExtensions.cbegin(); it!= availableExtensions.cend(); ++it)
    {
        if((*it).startsWith("qt",Qt::CaseInsensitive))
        {
            QScriptValue imported = engine->importExtension(*it);
            if(imported.isUndefined())
            {
                if(engine->hasUncaughtException())
                    qDebug() << engine->uncaughtException().toString();
            }
        }
    }

    //Load custom "client" extensions
    for(auto it = availableExtensions.cbegin(); it!= availableExtensions.cend(); ++it)
    {
        if(!(*it).contains(Packets::server_tag))
        {
            QScriptValue imported = engine->importExtension(*it);
            if(imported.isUndefined())
            {
                if(engine->hasUncaughtException())
                    qDebug() << engine->uncaughtException().toString();
            }
        }
    }

    QTimer::singleShot(5000,this,SLOT(init()));
}

void MainWindow::callPostBypassFunctions()
{    
    ScriptFunctions::callAllImportsFunction(engine,API::PostBypass_handler);
}

void MainWindow::handleExtensionPacket(QJsonObject object)
{
    QString content = object.value(Packets::content_tag).toString();
    QString extension = object.value(Packets::extension_tag).toString();
    extension.replace(Packets::server_tag,Packets::client_tag);
    QStringList ext = extension.split(".");
    if(!ext.isEmpty())
    {
        QScriptValue function = engine->globalObject().property(ext.first());
        for(auto i = 1; i < ext.length(); ++i)
            function = function.property(ext.at(i));
        function = function.property(Packets::handler_tag);
        function.call(QScriptValue(),QScriptValueList() << content );
    }
}
