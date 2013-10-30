#ifndef DEBUGUTIL_H
#define DEBUGUTIL_H
#include <QDebug>

#ifdef DEBUG_VAR
#undef DEBUG_VAR
#endif
#define DEBUG_VAR(VAR) qDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << #VAR << ":" <<  VAR;

#ifdef DEBUG_TEXT
#undef DEBUG_TEXT
#endif
#define DEBUG_TEXT(Text)  qDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ <<":" << Text;


#ifdef DEBUG_FUNC
#undef DEBUG_FUNC
#endif
#define DEBUG_FUNC qDebug() << __FILE__ << ":" << __LINE__ << ":" <<  __FUNCTION__;

#ifdef DEBUG_FUNC_ENTER
#define DEBUG_FUNC_ENTER
#endif
#define DEBUG_FUNC_ENTER qDebug() << __FILE__ << ":" << __LINE__ << "Enter" << __FUNCTION__ ;

#ifdef DEBUG_FUNC_LEAVE
#undef DEBUG_FUNC_LEAVE
#endif
#define DEBUG_FUNC_LEAVE qDebug() << __FILE__ << ":" << __LINE__ << "Leave" << __FUNCTION__

#ifdef DEBUG_VAR_COND
#undef DEBUG_VAR_COND
#endif
#define DEBUG_VAR_COND(D,VAR)  if(D){DEBUG_VAR(VAR);}

#endif // DEBUGUTIL_H
