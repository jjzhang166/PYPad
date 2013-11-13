#ifndef SIMSCINTILLA_GLOBAL_H
#define SIMSCINTILLA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SIMSCINTILLA_LIBRARY)
#  define SIMSCINTILLA_EXPORT /*Q_DECL_EXPORT*/
#else
#  define SIMSCINTILLA_EXPORT Q_DECL_IMPORT
#endif

/*!
 * \defgroup SimScintilla 封装Scintilla的功能模块
 *
 * 封装开源Scintilla的各种功能.
 */

#endif // SIMSCINTILLA_GLOBAL_H
