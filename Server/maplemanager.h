#ifndef MAPLEMANAGER_H
#define MAPLEMANAGER_H
#include <Windows.h>
#include <QObject>
#include <QJsonObject>
#include <QScriptable>
#include <QFuture>
#include <QFutureWatcher>

class QScriptEngine;

class Server;
class ManagerWidget;

class MapleManager : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    /*!
     * \brief Class to manage each Maplestory instance
     * \param server Server instance to comunicate with Maplestory
     * \param widget A widget to parent ManagerWidget
     */
    MapleManager(Server* server, QWidget* widget);

    ~MapleManager();
    /*!
     * \brief Status of the instance socket
     */
    enum class ConnectionStatus : char { WAITIGN = 0, //!< Waiting for Server.dll to connect with us.
                                         CONNECTED //!< Server.dll connected with us.
                                       };
    /*!
     * \brief The Status of the Maplestory launch process
     */
    enum class LaunchStatus : char { LAUNCHIGN = 0, //!< First Launch of Maplestory for this instance.
                                     RELAUNCHIGN    //!< Second and go on launch of Maplestory for this instance.
                                   };

    /*!
     * \brief Get Relaunch flag for this instance.
     * \return Relaunch Flag.
     */
    bool getRelaunch() const;    

    /*!
     * \brief Get socket status for this instance.
     * \return Socket status.
     */
    ConnectionStatus status() const;

    /*!
     * \brief Set socket status for this instance.
     * \param status Socket new status.
     */
    void setStatus(const ConnectionStatus &status);

    /*!
     * \brief Socket descriptor used to send througth Server.
     * \return Socket descriptor.
     */
    int getSocket() const;

    /*!
     * \brief Socket descriptor used to send througth Server.
     * \param value Socket descriptor.
     */
    void setSocket(int value);

    /*!
     * \brief Launch First MapleStory
     */
    void launchMaple();

    /*!
     * \brief Relaunch MapleStory
     * \note On this method, all saved status (Hacks, Bots...) will be
     * sent to the new MapleStory instance again;
     */
    void reLaunchMaple();

    /*!
     * \brief Process Packets sent from the dll.
     * \param object data sent drom dll.
     */
    void dispatchPackets(QJsonObject object);

    QWidget *widget() const;

    void init();

    enum class RESIZE_MODE : char { AUTO = 0, W_800, W_1024, W_1366 };

    RESIZE_MODE getR_mode() const;
    void setR_mode(const RESIZE_MODE &value);


signals:
    void changeTabText(QString);

public slots:
    void send(QString message, QString extension = QStringLiteral("global"));
    /*!
     * \brief Set Relaunch flag for this instance.
     * \param relaunch Relaunch flag new state.
     */
    void setRelaunch(bool relaunch);


private slots:
    void launchFailed();
    void launchSuccess();
    void embedMaple();
    void saveSettings();
    void loadSettings();
    void showDBG(bool state);
    void changeTabName(QString s);
private:
    Server *m_server;
    ManagerWidget *m_widget;
    QScriptEngine *m_engine;

    ConnectionStatus m_status;
    LaunchStatus m_launch_status;
    int m_socket;
    bool m_relaunch;
    DWORD	m_dwProcessId;
    HWND	m_hProcesswnd;    
    RESIZE_MODE r_mode;
    void launch();
    void resizeMaple(RESIZE_MODE r_mode);

    void handleExtensionPacket(QJsonObject object);
    void syncNewMaple();    

    bool eventFilter(QObject *target, QEvent *event);

    QFuture<void> m_Future;
    QFutureWatcher<void> m_FutWatcher;
};

#endif // MAPLEMANAGER_H
