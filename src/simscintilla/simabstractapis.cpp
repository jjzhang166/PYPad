#include "simabstractapis.h"
#include "simabstractlexer.h"

#include <QSqlDatabase>
#include <QSqlQuery>

SimAbstractAPIs::SimAbstractAPIs(SimAbstractLexer *parent) :
    QObject(parent), m_Lexer(parent)
{    
}

SimAbstractAPIs::~SimAbstractAPIs()
{
}

/*!
 * \brief 返回Lexer对象.
 * \return lexer对象
 */
SimAbstractLexer* SimAbstractAPIs::lexer() const
{
    return m_Lexer;
}

/*!
 * \brief 从数据库中读表获取系统中安装了哪几种API.
 */
QStringList SimAbstractAPIs::installedAPIs() {
    const QString kDefaultApiDbFile("apis.db");
    const QString kQuery("select * from installed_api_table");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(kDefaultApiDbFile);
    if (!db.open()) {
        return QStringList();
    }

    QStringList installedApis;
    QSqlQuery query;
    query.exec(kQuery);
    while(query.next()) {
        installedApis.append(query.value(0).toString());
    }
    return installedApis;
}

void SimAbstractAPIs::autoCompletionSelected(const QString &selection)
{
    Q_UNUSED(selection);
}
