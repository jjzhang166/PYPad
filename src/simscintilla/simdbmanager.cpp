#include "simdbmanager.h"

#include <QDebug>
#include <QStringList>
#include <QSqlError>
#include <QSqlRecord>

static const QString CommandTable("command");
static const QString FunctionTable("function");

SimDbManager::SimDbManager(QObject *parent) :
    QObject(parent)
{
    m_Db = QSqlDatabase::addDatabase("QSQLITE");
}

SimDbManager* SimDbManager::instance()
{
    static SimDbManager instance;
    return &instance;
}

/*!
 * \brief 设置Sqlite的数据文件路径.
 * \param file 文件路径
 */
void SimDbManager::setDbFile(const QString &file)
{
    m_DbFile = file;
    m_Db.setDatabaseName(m_DbFile);

    goThroughAllTables();
}

/*!
 * \brief 返回所有的条目.
 * \param ver ANSYS 版本号
 * \param tables 数据库表
 * \param funcOrCmd 函数还是命令
 * \return 所有函数或者命令
 */
QStringList SimDbManager::allItems(const QString& ver, const QStringList& tables, const QString& funcOrCmd)
{
    QString tableName;
    for (const QString& table: tables) {
        if (table.contains(ver)) {
            tableName = table;
            break;
        }
    }

    if(tableName.isEmpty()) {
        return QStringList();
    }

    QStringList functions;
    QSqlQuery query(m_Db);
    QString strQuery = QString("SELECT %1 FROM %2").arg(funcOrCmd).arg(tableName);
    query.prepare(strQuery);
    query.exec();
    while (query.next()) {
        functions.append(query.value(0).toString());
    }
    return functions;
}

/*!
 * \brief 获取指定版本ANSYS所有的函数.
 * \param ver 版本号
 * \return 所有函数
 */
QStringList SimDbManager::allFunctions(const QString& ver)
{
    return allItems(ver, m_FunctionTables, "FUNCTION");
}

/*!
 * \brief 获取指定版本ANSYS所有的命令.
 * \param ver 版本号
 * \return 所有命令
 */
QStringList SimDbManager::allCommands(const QString& ver)
{
    return allItems(ver, m_CommandTables, "COMMAND");
}

/*!
 * \brief 获取描述信息.
 * \param item 条目名称
 * \param table 表明
 * \param funcOrCmd 函数还是命令
 * \param predicate 用于判断的回调函数
 * \return
 */
QString SimDbManager::description(const QString& item, const QString& table, const QString& funcOrCmd, HasThisItemPredicate predicate)
{
    if (!predicate(item) || !hasSpecifiedVersion()) {
        return QString();
    }

    QSqlQuery query(m_Db);
    QString strQuery;
    strQuery = QString("SELECT DESCRIPTION FROM %1 WHERE %2=\"%3\"").arg(table).arg(funcOrCmd).arg(item);
    query.prepare(strQuery);
    query.exec();

    QString desc;
    while (query.next()) {
        desc = query.value(0).toString();
    }
    return desc;
}

/*!
 * \brief 获取指定函数的描述信息.
 * \param func 函数名
 * \return 描述信息
 */
QString SimDbManager::functionDescription(const QString& func)
{
    if (!hasThisFunction(func) || !hasSpecifiedVersion()) {
        return QString();
    }

    QSqlQuery query(m_Db);
    QString strQuery;
    strQuery = QString("SELECT DESCRIPTION FROM %1 WHERE FUNCTION=\"%2\"").arg(mFuncCmdData.functionTable).arg(func);
    query.prepare(strQuery);
    query.exec();

    QString desc;
    while (query.next()) {
        desc = query.value(0).toString();
    }
    return desc;
}

/*!
 * \brief 获取指定命令的描述信息.
 * \param cmd 命令
 * \return 描述信息
 */
QString SimDbManager::commandDescription(const QString& cmd)
{
    if (!hasThisCommand(cmd) || !hasSpecifiedVersion()) {
        return QString();
    }

    QSqlQuery query(m_Db);
    QString strQuery;
    strQuery = QString("SELECT DESCRIPTION FROM %1 WHERE COMMAND=\"%2\"").arg(mFuncCmdData.commandTable).arg(cmd);
    query.prepare(strQuery);
    query.exec();

    QString desc;
    while (query.next()) {
        desc = query.value(0).toString();
    }
    return desc;

}

/*!
 * \brief 获取指定函数的参数个数
 * \param func 函数名
 * \return 参数个数
 */
int SimDbManager::functionArgumentsCount(const QString& func)
{
    if (!hasThisFunction(func) || !hasSpecifiedVersion()) {
        return -1;
    }

    QSqlQuery query(m_Db);
    QString strQuery;
    strQuery = QString("SELECT NUMARGS FROM %1 WHERE FUNCTION=\"%2\"").arg(mFuncCmdData.functionTable).arg(func);
    query.prepare(strQuery);
    query.exec();

    int argCnt = -1;
    while(query.next()) {
        argCnt = query.value(0).toInt();
    }
    return argCnt;
}

/*!
 * \brief 获取指定命令的参数个数
 * \param cmd 命令
 * \return 参数个数
 */
int SimDbManager::commandArgumentsCount(const QString& cmd)
{
    if (!hasThisCommand(cmd) || !hasSpecifiedVersion()) {
        return -1;
    }

    QSqlQuery query(m_Db);
    QString strQuery;
    strQuery = QString("SELECT NUMARGS FROM %1 WHERE COMMAND=\"%2\"").arg(mFuncCmdData.commandTable).arg(cmd);
    query.prepare(strQuery);
    query.exec();

    int argCnt = -1;
    while(query.next()) {
        argCnt = query.value(0).toInt();
    }
    return argCnt;
}

/*!
 * \brief 获取指定函数的参数列表
 * \param func 函数名
 * \return 参数列表
 */
QStringList SimDbManager::functionArguments(const QString& func)
{
    if (!hasThisFunction(func) || !hasSpecifiedVersion()) {
        return QStringList();
    }

    int argCount = functionArgumentsCount(func);
    QString argQuery;
    for (int i = 0; i < argCount; ++i) {
        argQuery += QString("ARG%1,").arg(i + 1);
    }
    const QString trimmedArgQuery = argQuery.left(argQuery.length() - 1);

    QSqlQuery query(m_Db);
    QString strQuery = QString("SELECT %1 FROM %2 WHERE FUNCTION=\"%3\"")
            .arg(trimmedArgQuery).arg(mFuncCmdData.functionTable).arg(func);
    query.prepare(strQuery);
    query.exec();
    QStringList argList;
    while (query.next()) {
        int colCount = query.record().count();
        for (int colIndex = 0; colIndex < colCount; ++colIndex) {
            argList.append(query.value(colIndex).toString());
        }
    }
    return argList;
}

/*!
 * \brief 获取指定命令的参数列表
 * \param cmd 命令
 * \return 参数列表
 */
QStringList SimDbManager::commandArguments(const QString& cmd)
{
    if (!hasThisCommand(cmd) || !hasSpecifiedVersion()) {
        return QStringList();
    }

    int argCount = commandArgumentsCount(cmd);
    QString argQuery;
    for (int i = 0; i < argCount; ++i) {
        argQuery += QString("ARG%1,").arg(i + 1);
    }
    const QString trimmedArgQuery = argQuery.left(argQuery.length() - 1);

    QSqlQuery query(m_Db);
    QString strQuery = QString("SELECT %1 FROM %2 WHERE COMMAND=\"%3\"")
            .arg(trimmedArgQuery).arg(mFuncCmdData.commandTable).arg(cmd);
    query.prepare(strQuery);
    query.exec();
    QStringList argList;
    while (query.next()) {
        int colCount = query.record().count();
        for (int colIndex = 0; colIndex < colCount; ++colIndex) {
            argList.append(query.value(colIndex).toString());
        }
    }
    return argList;
}

/*!
 * \brief 获取指定函数的帮助路径
 * \param func 函数名
 * \return 帮助路径
 */
QString SimDbManager::functionHelpPath(const QString& func)
{
    return helpPath(mFuncCmdData.functionTable, "FUNCTION", func);
}

/*!
 * \brief 获取指定命令的帮助路径
 * \param cmd 命令
 * \return 帮助路径
 */
QString SimDbManager::commandHelpPath(const QString& cmd)
{
    return helpPath(mFuncCmdData.commandTable, "COMMAND", cmd);
}

/*!
 * \brief 打开数据库, 并遍历所有数据表, 初始化内部变量
 * \param ver 版本号
 */
bool SimDbManager::goThroughAllTables(const QString& ver)
{
    if (!m_Db.isOpen()) {
        if (!m_Db.open()) {
            qWarning() << m_Db.lastError().text();
            return false;
        }
    }

    if (!m_Db.isValid()) {
        return false;
    }

    QStringList allTables = m_Db.tables();
    if (!allTables.isEmpty())
        allTables.pop_front();

    m_CommandTables.clear();
    m_FunctionTables.clear();

    for(const QString& table : allTables) {
        if (table.contains(CommandTable, Qt::CaseInsensitive)) {
            m_CommandTables.append(table);
            if (!ver.isEmpty() && table.contains(ver, Qt::CaseInsensitive)) {
                mFuncCmdData.commandTable = table;
            }
        } else if (table.contains(FunctionTable, Qt::CaseInsensitive)) {
            m_FunctionTables.append(table);
            if (!ver.isEmpty() && table.contains(ver, Qt::CaseInsensitive)) {
                mFuncCmdData.functionTable = table;
            }
        }
    }

    if (!mFuncCmdData.commandTable.isEmpty()) {
        mFuncCmdData.isValid = true;
    }
    return true;
}

bool SimDbManager::hasThisItem(const QString& item, const QStringList& tables, ItemInTablePredicate predicate)
{
    bool has = false;
    for (const QString& table: tables) {
        has = predicate(table, item);
        if (has) {
            break;
        }
    }
    return has;
}

/*!
 * \brief 遍历所有函数表中查询是否有指定的函数
 * \param func 函数名
 * \return 是否存在
 */
bool SimDbManager::hasThisFunction(const QString& func)
{
    bool has = false;
    for (const QString& table: m_FunctionTables) {
        has = isFunctionInThisTable(table, func);
        if (has) {
            break;
        }
    }
    return has;
}

/*!
 * \brief 遍历所有函数表中查询是否有指定的命令
 * \param func 命令名
 * \return 是否存在
 */
bool SimDbManager::hasThisCommand(const QString& cmd)
{
    bool has = false;
    for (const QString& table: m_CommandTables) {
        has = isCommandInThisTable(table, cmd);
        if (has) {
            break;
        }
    }
    return has;
}

/*!
 * \brief 是否指定了ANSYS版本号
 * \return
 */
bool SimDbManager::hasSpecifiedVersion()
{
    if (!mFuncCmdData.isValid) {
        qDebug() << "Please specify version number first!";
        return false;
    }
    return true;
}

QString SimDbManager::helpPath(const QString &table, const QString &funcOrCmd, const QString &item)
{
    if (!hasSpecifiedVersion()) {
        return QString();
    }

    QSqlQuery query(m_Db);
    QString strQuery = QString("SELECT HELPDIR FROM %1 WHERE %2=\"%3\"")
            .arg(table).arg(funcOrCmd).arg(item);
    query.prepare(strQuery);
    query.exec();

    QString path;
    while (query.next()) {
        path = query.value(0).toString();
    }
    return path;
}

bool SimDbManager::isDataInThisTable(const QString& table, const QString& funcOrCmd, const QString& name) {
    QSqlQuery query(m_Db);
    QString strQuery = QString("SELECT %2 FROM %1 WHERE %2=\"%3\"").arg(table).arg(funcOrCmd).arg(name);
    query.prepare(strQuery);
    query.exec();
    return !query.record().isEmpty();
}

bool SimDbManager::isFunctionInThisTable(const QString& table, const QString& func)
{
    return isDataInThisTable(table, "FUNCTION", func);
}

bool SimDbManager::isCommandInThisTable(const QString& table, const QString& cmd)
{
    return isDataInThisTable(table, "COMMAND", cmd);
}
