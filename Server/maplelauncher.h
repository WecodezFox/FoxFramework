#ifndef MAPLELAUNCHER_H
#define MAPLELAUNCHER_H
#include <windows.h>
#include <QThread>
#include <QMutex>

class MapleLauncher : public QThread
{
    Q_OBJECT
public:
    /*!
     * \brief Aux class to launch Maplestory.
     * \param parent QObject parent.
     * \param strProcess Path of Maplestory.exe.
     */
    explicit MapleLauncher(QObject *parent = 0 , std::wstring strProcess = L"");
    ~MapleLauncher();

    DWORD	dwProcessId;//!< Launched PID.
    HWND	hProcesswnd;//!< Launched window handle.
    DWORD   oldPid;     //!< Only used on relaunched maple.

    /*!
     * \brief Embed child window into parent window.
     * \param child Child window handle.
     * \param hParentwnd Parent window handle.
     * \param width Child window final width. If 0 then child width will equal parent width.
     * \param heigth Child window final heigth. If 0 then child heigth will equal parent heigth.
     */
    static void SetParentWindow(HWND child,  HWND hParentwnd, int width = 0, int heigth = 0);

    /*!
     * \brief findWindow
     * \param PID
     * \param windowName
     * \return
     */
    static HWND findWindow(unsigned long PID, QString windowName);

    /*!
     * \brief Change Maplestory.exe path.
     * \param newPath Absolute new path.
     */
    static void changePath(QString newPath);

signals:
        /*!
         * \brief Signal emmited when launch success.
         */
        void LaunchSuccess();

        /*!
         * \brief Signal emmited when launch fails.
         */
        void LaunchFail();

protected:
        /*!
         * \brief Start Maplestory with Server.dll injected.
         * \return True on success
         */
        bool StartProcess();       

        /*!
         * \brief Override from QThread. Run StartProcess() & FindProcess()
         * on background. Emit LaunchFail or LaunchSuccess.
         */
        void run();
private:

    static std::wstring	strProcess;
    HANDLE	hProcess;
    QMutex mutex;
    bool _b;
};

#endif // MAPLELAUNCHER_H
