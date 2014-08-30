#ifndef MANAGERWIDGET_H
#define MANAGERWIDGET_H

#include <QWidget>
#include <Windows.h>

namespace Ui {
class ManagerWidget;
}

class ManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManagerWidget(QWidget *parent = 0);
    ~ManagerWidget();

    /*!
     * \brief Add a new tab with the widget inside.
     * \param name Tab name.
     * \param widget Widget to register
     */
    Q_INVOKABLE void registerWidget(QString name, QWidget* widget);

    /*!
     * \return HWND of the widget to embed Maplestory
     */
    HWND getMapleContainer() const;

    QWidget* getMapleWidget() const;

    void regiterDebugger(QWidget* dbg);

    /*!
     * \brief tryEmbed
     * \param windowName
     * \param tabName
     */
    Q_INVOKABLE bool tryEmbed(QString windowName, QString tabName);

    DWORD dwProcessId() const;
    void setDwProcessId(const DWORD &dwProcessId);

    Q_INVOKABLE void changeTabName(QString s);

    void toogleReload(bool b);
signals:
    void showDBG(bool);
    void save();
    void load();
    void relaunch(bool);
    void s_changeTabName(QString s);
private slots:
    void on_remoteDbg_toggled(bool checked);

    void on_btnSave_clicked();

    void on_btnLoad_clicked();

    void on_rebootBtn_toggled(bool checked);

private:
    Ui::ManagerWidget *ui;
    DWORD	m_dwProcessId;        
};

#endif // MANAGERWIDGET_H
