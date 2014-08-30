#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../common/Tags.h"
#include "maplemanager.h"


#include <QCloseEvent>
#include <QMessageBox>

#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_server(Packets::server_name_tag,this)
{
    ui->setupUi(this);

    //Init the local server
    if(m_server.init())
    {
        connect(&m_server,SIGNAL(ReadReady(int,QJsonObject)),this,SLOT(dispatchPackets(int,QJsonObject)));
        connect(&m_server,SIGNAL(SocketDeleted(int)),this,SLOT(mapleCrashed(int)));
        connect(&m_server,SIGNAL(SocketConnected(int)),this,SLOT(mapleLaunched(int)));
    }
    else
    {
        QMessageBox::critical(this,tr("Dam it!"),tr("Something's fucked on the socket server"));
    }

    connect(ui->actionAuto,SIGNAL(toggled(bool)),this,SLOT(maple_size_mode(bool)));
    connect(ui->action800,SIGNAL(toggled(bool)),this,SLOT(maple_size_mode(bool)));
    connect(ui->action1024,SIGNAL(toggled(bool)),this,SLOT(maple_size_mode(bool)));
    connect(ui->action1366,SIGNAL(toggled(bool)),this,SLOT(maple_size_mode(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * e)
{
    if(QMessageBox::question(this,
                             tr("Closing..."),
                             tr("Do u really want to close FoxFramework and all Maplestory instances?"),
                             tr("No"),
                             tr("Yes")) == QMessageBox::Accepted)
    {
        for(auto it = m_managers.cbegin(); it!= m_managers.cend(); ++it)
        {
            MapleManager * m = *it;
            m->setRelaunch(false);
        }
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void MainWindow::dispatchPackets(int sock, QJsonObject object)
{
    for(auto it = m_managers.cbegin(); it!= m_managers.cend(); ++it)
    {
        MapleManager * m = *it;
        if( m->getSocket() == sock)
        {
            m->dispatchPackets(object);
            return;
        }
    }
}

void MainWindow::mapleLaunched(int sock)
{
    for(auto it = m_managers.cbegin(); it!= m_managers.cend(); ++it)
    {
        MapleManager * m = *it;
        if( m->status() == MapleManager::ConnectionStatus::WAITIGN)
        {
            m->setSocket(sock);
            m->setStatus(MapleManager::ConnectionStatus::CONNECTED);

            //Set Scripts plugins path on dll
            ui->tabs->addTab(m->widget(), "[Launching...]");
            m->init();           

            QJsonDocument doc;
            QJsonObject obj;
            obj[Packets::header_tag] = (int)Packets::Headers::PATH;
            obj["path"] = qApp->applicationDirPath();
            doc.setObject(obj);
            m_server.send(sock,doc);            

            return;
        }
    }
}

void MainWindow::mapleCrashed(int sock)
{
    for(auto it = m_managers.cbegin(); it!= m_managers.cend(); ++it)
    {
        MapleManager * m = *it;
        if( m->getSocket() == sock)
        {
            if(m->getRelaunch())
            {
                m->reLaunchMaple();
            }
            else
            {
                m_managers.removeOne(m);
                m->deleteLater();
            }
            return;
        }
    }
}

void MainWindow::on_actionRun_MapleStory_triggered()
{
    MapleManager * newManager = new MapleManager(&m_server,this);
    connect(newManager,SIGNAL(changeTabText(QString)),this,SLOT(changeTabText(QString)));

    auto mode = MapleManager::RESIZE_MODE::AUTO;
    if(ui->action800->isChecked())
        mode = MapleManager::RESIZE_MODE::W_800;
    else if(ui->action1024->isChecked())
        mode = MapleManager::RESIZE_MODE::W_1024;
    else if(ui->action1366->isChecked())
        mode = MapleManager::RESIZE_MODE::W_1366;
    newManager->setR_mode(mode);

    m_managers.append(newManager);
    newManager->launchMaple();
}

void MainWindow::maple_size_mode(bool state)
{
    if(!state)
        return;

    const QObject * ptr = sender();
    auto mode = MapleManager::RESIZE_MODE::AUTO;

    if(ptr == ui->actionAuto)
    {
        ui->action800->setChecked(false);
        ui->action1024->setChecked(false);
        ui->action1366->setChecked(false);
    }
    else if(ptr == ui->action800)
    {
        ui->actionAuto->setChecked(false);
        ui->action1024->setChecked(false);
        ui->action1366->setChecked(false);
        mode = MapleManager::RESIZE_MODE::W_800;
    }
    else if(ptr == ui->action1024)
    {
        ui->actionAuto->setChecked(false);
        ui->action800->setChecked(false);
        ui->action1366->setChecked(false);
        mode = MapleManager::RESIZE_MODE::W_1024;
    }
    else if(ptr == ui->action1366)
    {
        ui->actionAuto->setChecked(false);
        ui->action800->setChecked(false);
        ui->action1024->setChecked(false);
        mode = MapleManager::RESIZE_MODE::W_1366;
    }

    for(auto it = m_managers.cbegin(); it!= m_managers.cend(); ++it)
    {
        MapleManager * m = *it;
        m->setR_mode(mode);
    }
}

void MainWindow::on_actionQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionIcons_triggered()
{
    QString Caption = "<h3>About Icons</h3>";
    QString Text = "<p>This framework use Fugue icons by Yusuke Kamiyamane.</p>"
                   "<p><a href=\"http://p.yusukekamiyamane.com/\">[http://p.yusukekamiyamane.com/]</a></p>";

    QMessageBox box(this);
    box.setWindowTitle("About icons");
    box.setText(Caption);
    box.setInformativeText(Text);

    QPixmap pm(QLatin1String("://icons/information.png"));
    if (!pm.isNull())
        box.setIconPixmap(pm);

    box.exec();
}

void MainWindow::on_actionFramework_Itself_triggered()
{
    QString Caption = "<h3>About FoxFramework</h3>";
    QString Text = "<p>Visit WeCodez for more info.</p>"
            "<p><a href=\"https://wecodez.com/\">[https://wecodez.com/]</a></p>";

    QMessageBox box(this);
    box.setWindowTitle(tr("About FoxFramework"));
    box.setText(Caption);
    box.setInformativeText(Text);

    QPixmap pm(QLatin1String("://icons/snail.png"));
    if (!pm.isNull())
        box.setIconPixmap(pm);

    box.exec();
}

void MainWindow::changeTabText(QString text)
{
    QObject * ptr = sender();
    if(MapleManager* man = qobject_cast<MapleManager*>(ptr))
    {
        ui->tabs->setTabText(ui->tabs->indexOf(man->widget()),text);
    }
}
