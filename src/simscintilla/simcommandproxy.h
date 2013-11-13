#ifndef SIMCOMMANDPROXY_H
#define SIMCOMMANDPROXY_H

#include "simscintilladefs.h"
#include "simscintilla_global.h"

#include <QObject>
#include <QSettings>

class SimCommand;
class SimScintilla;
class SIMSCINTILLA_EXPORT SimCommandProxy : public QObject
{
    Q_OBJECT
public:
    static bool readSettings(QSettings &qs, const QString& prefix = "/SimScintilla");
    static bool writeSettings(QSettings &qs, const QString& prefix = "/SimScintilla");

    QList<SimCommand*>& getCommands() { return m_cmds; }

    void clearKeys();
    void clearAlternateKeys();

    SimCommand *boundTo(int key) const;
    SimCommand *find(SimSci::Command command) const;

private:
    friend class SimScintillaPrivate;
    explicit SimCommandProxy(SimScintilla* parent);
    ~SimCommandProxy();
    void init();
    void clear();


    static SimScintilla* m_scintilla;
    static QList<SimCommand* > m_cmds;
};

#endif // SIMCOMMANDPROXY_H
