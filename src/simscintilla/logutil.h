#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <QDebug>

#define USE_DEBUG
#ifdef USE_DEBUG
#ifndef LOG_TEXT
#define LOG_TEXT(TEXT) Log::PrintText(__FILE__,__FUNCTION__,__LINE__,#TEXT)
#endif

#ifndef LOG_VAR
#define LOG_VAR(VAR) Log::PrintValue(__FILE__,__FUNCTION__,__LINE__,#VAR,VAR)
#endif

#ifndef LOG_TARGET_VAR
#define LOG_TARGET_VAR(VAR) LOG_TEXT(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"); \
                            LOG_VAR(VAR); \
                            LOG_TEXT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
#endif

#ifndef LOG_SECTION_START
#define LOG_SECTION_START LOG_TEXT(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
#endif

#ifndef LOG_SECTION_END
#define LOG_SECTION_END LOG_TEXT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
#endif

#else // USE_DEBUG

#ifndef LOG_TEXT
#define LOG_TEXT(TEXT)
#endif

#ifndef LOG_VAR
#define LOG_VAR(VAR)
#endif

#ifndef LOG_TARGET_VAR
#define LOG_TARGET_VAR(VAR)
#endif

#ifndef LOG_SECTION_START
#define LOG_SECTION_START
#endif

#ifndef LOG_SECTION_END
#define LOG_SECTION_END
#endif
#endif // USE_DEBUG

namespace Log
{
    void PrintText(const char* file,const char* funcName,int lineNumber,const char* strMsg);

    template <typename T>
    void PrintValue(const char* file,const char* funcName,int lineNumber,const char* valueName,const T& value)
    {
        qDebug() <<"File: ["<< file <<":" << lineNumber<<  "] : <<" << funcName << ">> : {" << valueName <<"=" << value <<"}" ;
    }
}
#endif // LOGUTIL_H
