#ifndef SORO_GLOBAL_H
#define SORO_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(CORE_LIBRARY)
#  define SORO_CORE_EXPORT Q_DECL_EXPORT
#else
#  define SORO_CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // SORO_GLOBAL_H
