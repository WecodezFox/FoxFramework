#ifndef PACKETEDITOR_GLOBAL_H
#define PACKETEDITOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PACKETEDITOR_LIBRARY)
#  define PACKETEDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PACKETEDITORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PACKETEDITOR_GLOBAL_H
