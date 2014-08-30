#include "managerwidget.h"
#include "ui_managerwidget.h"
#include "maplelauncher.h"

ManagerWidget::ManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManagerWidget)
{
    ui->setupUi(this);
}

ManagerWidget::~ManagerWidget()
{
    delete ui;
}

void ManagerWidget::registerWidget(QString name, QWidget *widget)
{
    ui->tabWidget->addTab(widget,name);
}

HWND ManagerWidget::getMapleContainer() const
{
    return (HWND)ui->mapleTab->winId();
}

QWidget *ManagerWidget::getMapleWidget() const
{
    return ui->mapleTab;
}

void ManagerWidget::regiterDebugger(QWidget *dbg)
{
    ui->debugTab->layout()->addWidget(dbg);
}

bool ManagerWidget::tryEmbed(QString windowName, QString tabName)
{
    HWND handle = MapleLauncher::findWindow(m_dwProcessId, windowName);
    if(handle)
    {
        QWidget * wid = new QWidget(this);
        MapleLauncher::SetParentWindow(handle,(HWND)wid->winId());
        this->registerWidget(tabName, wid);
        return true;
    }
    return false;
}

DWORD ManagerWidget::dwProcessId() const
{
    return m_dwProcessId;
}

void ManagerWidget::setDwProcessId(const DWORD &dwProcessId)
{
    m_dwProcessId = dwProcessId;
}

void ManagerWidget::changeTabName(QString s)
{
    emit s_changeTabName(s);
}

void ManagerWidget::toogleReload(bool b)
{
    ui->remoteDbg->setChecked(b);
}

void ManagerWidget::on_remoteDbg_toggled(bool checked)
{
    emit showDBG(checked);
}


void ManagerWidget::on_btnSave_clicked()
{
    emit save();
}

void ManagerWidget::on_btnLoad_clicked()
{
    emit load();
}

void ManagerWidget::on_rebootBtn_toggled(bool checked)
{
    emit relaunch(checked);
}
