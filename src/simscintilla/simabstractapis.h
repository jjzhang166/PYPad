#ifndef SIMABSTRACTAPIS_H
#define SIMABSTRACTAPIS_H

#include "simscintilladefs.h"

#include <QObject>

class SimAbstractLexer;
class SimAbstractAPIsPrivate;
class SimAbstractAPIs : public QObject
{
    Q_OBJECT

public:
    explicit SimAbstractAPIs(SimAbstractLexer *parent = 0);
    virtual ~SimAbstractAPIs();
    SimAbstractLexer *lexer() const;
    QStringList installedAPIs();

    virtual void updateAutoCompletionList(const QStringList &context, QStringList &list) = 0;
    virtual void autoCompletionSelected(const QString &selection);
    virtual QStringList callTips(const QStringList &context, int commas, SimSci::CallTipsStyle style,QList<int> &shifts) = 0;

protected:
    SimAbstractLexer* m_Lexer;
    Q_DISABLE_COPY(SimAbstractAPIs)
};

#endif // SIMABSTRACTAPIS_H
