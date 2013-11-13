#include "simlexerjs.h"

SimLexerJs::SimLexerJs(SimScintilla *parent): SimLexerCpp(parent) {}

/*!
 * \reimp
 */
QString SimLexerJs::description(int style) const {
    if (style == Regex)
        return tr("Regular expression");

    return SimLexerCpp::description(style);
}

/*!
 * \reimp
 */
QColor SimLexerJs::defaultColor(int style) const {
    if (style == Regex)
        return QColor(0x3f,0x7f,0x3f);

    return SimLexerCpp::defaultColor(style);
}

/*!
 * \reimp
 */
bool SimLexerJs::defaultEolFill(int style) const {
    if (style == Regex)
        return true;

    return SimLexerCpp::defaultEolFill(style);
}

/*!
 * \reimp
 */
QFont SimLexerJs::defaultFont(int style) const {
    if (style == Regex)
#if defined(Q_OS_WIN)
        return QFont("Courier New",10);
#elif defined(Q_OS_MAC)
        return QFont("Courier", 12);
#else
        return QFont("Bitstream Vera Sans Mono",9);
#endif

    return SimLexerCpp::defaultFont(style);
}

/*!
 * \reimp
 */
QColor SimLexerJs::defaultBackgroundColor(int style) const {
    if (style == Regex)
        return QColor(0xe0,0xf0,0xff);

    return SimLexerCpp::defaultBackgroundColor(style);
}

/*!
 * \reimp
 */
QString SimLexerJs::keywords(int set) const {
    if (set != 1)
        return QString();

    return "abstract boolean break byte case catch char class const continue "
            "debugger default delete do double else enum export extends final "
            "finally float for function goto if implements import in instanceof "
            "int interface long native new package private protected public "
            "return short static super switch synchronized this throw throws "
            "transient try typeof var void volatile while with";
}
