#include "simlexerfactory.h"
#include "./lexers/simlexercpp.h"
#include "./lexers/simlexerjs.h"
#include "./lexers/simlexerapdl.h"

SimLexerFactory::SimLexerFactory(QObject *parent) :
    QObject(parent) {
}

/*!
 * \brief 根据传入的语言创建Lexer对象.
 * \param lang 语言的枚举值
 * \param scintilla 对象指针
 * \return Lexer 对象指针
 */
SimAbstractLexer* SimLexerFactory::createLexer(SimSci::Language lang, SimScintilla* scintilla) {
    SimAbstractLexer* lexer = nullptr;
    switch (lang) {
    case SimSci::Lang_Cpp:
        lexer = new SimLexerCpp(scintilla);
        break;

    case SimSci::Lang_Js:
        lexer = new SimLexerJs(scintilla);
        break;

    case SimSci::Lang_Apdl:
        lexer = new SimLexerApdl(scintilla);
        break;

    default:
        break;
    }

    return lexer;
}
