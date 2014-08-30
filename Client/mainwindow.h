#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFuture>
#include <QFutureWatcher>

#include "connection.h"

#include <thread>

class QScriptEngine;

class BotManager;
class KeyBoard;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Log(QString s);
    void setThreadId(std::thread::id localGet_id);
signals:
    void LogMe(QString s);

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void init();
    void readSocket(QJsonObject obj);
    void displayLog(QString s);
private:
    Ui::MainWindow *ui;
    Connection connection;
    QScriptEngine *engine;
    KeyBoard *keyBoard;

    BotManager *m_bot;
    QFuture<bool> m_FutureBot;
    QFutureWatcher<bool> m_BotWatcher;

    void setPath(const QString path);
    void callPostBypassFunctions();
    void handleExtensionPacket(QJsonObject object);
    int id;
};

#endif // MAINWINDOW_H
