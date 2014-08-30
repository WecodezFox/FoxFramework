#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QMutex>
#include "core_global.h"
#include <Windows.h>

class CORE_SYMBOL KeyBoard : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Class for KeyBoard operations
     */
    explicit KeyBoard(QObject *parent = 0);


    /*!
     * \brief Press and release a key
     * \param uiKey Virtual-key code for the key.
     * \note See http://msdn.microsoft.com/es-es/library/windows/desktop/dd375731%28v=vs.85%29.aspx
     * for key codes
     */
    Q_INVOKABLE void SendKey(unsigned int uiKey);

    /*!
     * \brief Hold key for desired time
     * \param uiKey Virtual-key code for the key.
     * \param iHoldingDelay Holding time in milliseconds
     * \note See http://msdn.microsoft.com/es-es/library/windows/desktop/dd375731%28v=vs.85%29.aspx
     */
    Q_INVOKABLE void HoldKey(unsigned int uiKey, int iHoldingDelay);

    /*!
     * \brief Press and release key on desired window
     * \param hProcesswnd Handle to target window
     * \param uiKey Virtual-key code for the key.
     * \note See http://msdn.microsoft.com/es-es/library/windows/desktop/dd375731%28v=vs.85%29.aspx
     */
    static void SendKey(HWND hProcesswnd, unsigned int uiKey);

    /*!
     * \brief Hold key for desired time on desired window
     * \param hProcesswnd Handle to target window
     * \param uiKey Virtual-key code for the key.
     * \param iHoldingDelay Holding time in milliseconds
     * \note See http://msdn.microsoft.com/es-es/library/windows/desktop/dd375731%28v=vs.85%29.aspx
     */
    static void HoldKey(HWND hProcesswnd, unsigned int uiKey, int iHoldingDelay);

    HWND getHProcesswnd() const;
    void setHProcesswnd(const HWND &value);
signals:

public slots:

private:
    HWND hProcesswnd;
    QMutex mutex;
};

#endif // KEYBOARD_H
