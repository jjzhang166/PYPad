#include "logutil.h"

namespace Log
{
    void PrintText(const char* file,const char* funcName,int lineNumber,const char* strMsg)
    {
        qDebug() <<"File: ["<< file << "] => <<" << funcName << ">> => #"<<lineNumber<<"# =>" << strMsg;
    }
}
