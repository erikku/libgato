#ifndef LIBGATO_GLOBAL_H
#define LIBGATO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBGATO_LIBRARY)
#  define LIBGATO_EXPORT Q_DECL_EXPORT
#else
#  define LIBGATO_EXPORT Q_DECL_IMPORT
#endif

typedef quint16 GatoHandle;

#endif // LIBGATO_GLOBAL_H
