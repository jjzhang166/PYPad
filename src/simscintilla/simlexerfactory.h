#ifndef SIMLEXERFACTORY_H
#define SIMLEXERFACTORY_H

#include "simscintilla_global.h"
#include "simscintilladefs.h"
#include "simabstractlexer.h"

#include <QObject>

class SimScintilla;

class SIMSCINTILLA_EXPORT SimLexerFactory : public QObject
{
    Q_OBJECT
public:
    static SimAbstractLexer* createLexer(SimSci::Language lang, SimScintilla* scintilla);

private:
    explicit SimLexerFactory(QObject *parent = 0);

};

#endif // SIMLEXERFACTORY_H
