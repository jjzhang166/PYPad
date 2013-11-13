#ifndef SIMLEXERJS_H
#define SIMLEXERJS_H

#include "simlexercpp.h"

class SimScintilla;
class SimLexerFactory;

class SimLexerJs : public SimLexerCpp
{
    Q_OBJECT
public:
    int lang() const { return SCLEX_CPP; }
    QString lexerStr() const { return "Javascript"; }
    QString description(int style) const;

    QColor defaultColor(int style) const;
    bool defaultEolFill(int style) const;
    QFont defaultFont(int style) const;
    QColor defaultBackgroundColor(int style) const;
    QString keywords(int set) const;

protected:
    friend class SimLexerFactory;
    explicit SimLexerJs(SimScintilla* parent = 0);
};

#endif // SIMLEXERJS_H
