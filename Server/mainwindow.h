#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MapleManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void dispatchPackets(int sock, QJsonObject object);

    void mapleLaunched(int sock);
    void mapleCrashed(int sock);

    void on_actionRun_MapleStory_triggered();
    void maple_size_mode(bool);
    void on_actionQt_triggered();

    void on_actionIcons_triggered();

    void on_actionFramework_Itself_triggered();

    void changeTabText(QString text);
private:
    Ui::MainWindow *ui;
    Server m_server;

    QList<MapleManager *> m_managers;
};

#endif // MAINWINDOW_H
