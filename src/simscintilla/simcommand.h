#ifndef SIMCOMMAND_H
#define SIMCOMMAND_H

#include "simscintilladefs.h"
#include "simscintilla_global.h"

#include <QObject>

class SimScintilla;
class SimCommandPrivate;
class SIMSCINTILLA_EXPORT SimCommand
{
public:
    void execute();
    SimSci::Command command();

    void setKey(int key);
    int key() const;

    void setAlternateKey(int akey);
    int alternateKey() const;

    static bool isValidKey(int key);

    QString description() const;

    ~SimCommand();
private:
    friend class SimCommandProxy;
    explicit SimCommand(SimSci::Command cmd, int key, int altKey, const QString& desc, SimScintilla* scintilla);

    SimCommandPrivate* d;
    Q_DISABLE_COPY(SimCommand)
};

#endif // SIMCOMMAND_H
