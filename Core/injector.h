#ifndef INJECTOR_H
#define INJECTOR_H

#include <QObject>
#include <QScriptable>
#include "core_global.h"

class CORE_SYMBOL Injector : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    /*!
     * \brief Injector load dynamic-link libraries (.dll) on runtime
     */
    explicit Injector(QObject *parent = 0);

    /*!
     * \brief Inject all dynamic-link libraries (.dll) on a folder
     * \param folder Target folder to search dlls on.
     * \return success
     */
    bool injectFolder(QString folder);
signals:

public slots:
    /*!
     * \brief Script call to inject a single dll
     * \return success
     * \note Call this only from a script
     * \note Script param::string: dll name found on a relative folder from the
     * extension caller.
     */
    bool inject();

private:
    bool _inject(QString file);
};

#endif // INJECTOR_H
