#include "simlexerfactory.h"
#include "simlexercpp.h"
#include "simlexerjs.h"
#include "simlexerapdl.h"

SimLexerFactory::SimLexerFactory(QObject *parent) :
    QObject(parent) {
}

/*!
 * \brief SimLexerFactory::createLexer
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
