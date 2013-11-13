#ifndef SIMDBMANAGER_H
#define SIMDBMANAGER_H

#include "simscintilla_global.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class SIMSCINTILLA_EXPORT SimDbManager : public QObject
{
    Q_OBJECT
public:
    static SimDbManager* instance();

    void setDbFile(const QString& file);

    QStringList allFunctions(const QString& ver);
    QStringList allCommands(const QString& ver);

    QString functionDescription(const QString& func);
    QString commandDescription(const QString& cmd);

    int functionArgumentsCount(const QString& func);
    int commandArgumentsCount(const QString& cmd);

    QStringList functionArguments(const QString& func);
    QStringList commandArguments(const QString& cmd);

    QString functionHelpPath(const QString& func);
    QString commandHelpPath(const QString& cmd);

    bool goThroughAllTables(const QString& ver = QString());

private:
    explicit SimDbManager(QObject *parent = 0);

    typedef bool (* HasThisItemPredicate)(const QString& item);
    typedef bool (* ItemInTablePredicate) (const QString& , const QString& );

    bool hasThisItem(const QString& item, const QStringList& tables, ItemInTablePredicate predicate);

    bool hasThisFunction(const QString& func);
    bool hasThisCommand(const QString& cmd);
    bool isDataInThisTable(const QString& table, const QString& funcOrCmd, const QString& name);
    bool isFunctionInThisTable(const QString& table, const QString& func);
    bool isCommandInThisTable(const QString& table, const QString& cmd);
    bool hasSpecifiedVersion();
    QString helpPath(const QString& table, const QString& funcOrCmd, const QString& item);
    QStringList allItems(const QString& ver, const QStringList& tables, const QString& funcOrCmd);
    QString description(const QString& item, const QString& table, const QString& funcOrCmd, HasThisItemPredicate predicate);

    QString m_DbFile;
    QSqlDatabase m_Db;

    typedef QList<QString> TableList;
    TableList m_FunctionTables;
    TableList m_CommandTables;

    struct FunctionCommandData {
        FunctionCommandData(): isValid(false){}
        QString functionTable;
        QString commandTable;
        bool isValid;
    };

    FunctionCommandData mFuncCmdData;
};

#endif // SIMDBMANAGER_H
