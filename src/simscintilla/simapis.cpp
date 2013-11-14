#include "simapis.h"
#include "simabstractlexer.h"

#ifdef USELOG
#include "logutil.h"
#endif

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>

/*!
 * \brief The SimAPIsExtractor class
 */
class SimAPIsExtractor : public QThread
{
public:
    explicit SimAPIsExtractor(const QStringList& apis, SimAPIs* apiholder);

public:
    void stop();

protected:
    void run();

private:

    static QStringList getApiWords(const QString& rawStr, const QString& sep, bool stripImage);
    static QString getApiBaseName(const QString& api);


    typedef QMap<QString, SimAPIs::WordCoordinateList> Word2CordMap;
    Word2CordMap mWord2CordMap;

    typedef QMap<QString, QString>  Word2WordMap;
    Word2WordMap mWord2WordMap;

    QStringList mAPIs;
    SimAPIs* mAPIsHolder;
    bool mStop;

    friend class SimAPIs;
};

SimAPIsExtractor::SimAPIsExtractor(const QStringList& apis, SimAPIs *apiholder):
    mAPIs(apis), mAPIsHolder(apiholder), mStop(false) {}

void SimAPIsExtractor::stop()
{
    mStop = true;
    terminate();
}

void SimAPIsExtractor::run()
{
    mAPIs.sort();
    const QStringList wordSeparators = mAPIsHolder->lexer()->autoCompletionWordSeparators();
    bool cs = mAPIsHolder->lexer()->isCaseSensitive();

    QString sep;
    if (wordSeparators.isEmpty()) {
        sep = " ";
    } else {
        sep = wordSeparators.first();
    }

    for (int i = 0; i < mAPIs.size(); ++i) {
        if (mStop)
            break;

        QStringList apiWords = getApiWords(mAPIs.at(i), sep, true);

        for (int w = 0; w < apiWords.count(); ++w) {
            const QString &word = apiWords[w];

            // 拿到Word对应的坐标列表, 如果没有的话就创建一个并添加到Map中
            SimAPIs::WordCoordinateList cordList = mWord2CordMap[word];

            // 如果这个语言是大小写敏感的并且是新进的word那么就把它保存到map中
            if (!cs && cordList.isEmpty())
                mWord2WordMap[word.toUpper()] = word;

            cordList.append(SimAPIs::WordCoordinate(i, w));
            mWord2CordMap[word] = cordList;
        }
    }
}

QStringList SimAPIsExtractor::getApiWords(const QString &rawStr, const QString &sep, bool stripImage)
{
    QString baseName = getApiBaseName(rawStr);

    if (stripImage) {
        int tail = baseName.indexOf('?');

        if (tail >= 0)
            baseName.truncate(tail);
    }

    if (sep.isEmpty())
        return QStringList(baseName);

    return baseName.split(sep);

}

QString SimAPIsExtractor::getApiBaseName(const QString &api)
{
    QString rawApi = api;
    int tail = rawApi.indexOf('(');

    if (tail >= 0)
        rawApi.truncate(tail);

    return rawApi.simplified();
}

SimAPIs::SimAPIs(SimAbstractLexer* parent): SimAbstractAPIs(parent), m_extractor(nullptr)
{
}

SimAPIs::~SimAPIs()
{
    if (m_extractor != nullptr) {
        delete m_extractor;
    }
}

/*!
 * \brief 添加一条API条目.
 * \param api
 * \sa remove clear() load(const QString&)
 */
void SimAPIs::add(const QString &api)
{
    if (!m_apis.contains(api)) {
        m_apis.append(api);
    }
}

/*!
 * \brief 删除一条API条目.
 * \param api
 * \sa add clear() load(const QString&)
 */
void SimAPIs::remove(const QString &api)
{
    if (m_apis.contains(api)) {
        m_apis.removeAll(api);
    }
}

/*!
 * \brief 清除所有的API条目.
 * \sa add(const QString&) remove(const QString&) load(const QString&)
 */
void SimAPIs::clear()
{
    m_apis.clear();
}

/*!
 * \brief 从外部的API文件中加载API条目.
 * \param filename
 * \return 是否成功加载文件中的API
 * \sa add(const QString&) remove(const QString&) clear()
 */
bool SimAPIs::load(const QString &lang)
{
    const QString kDefaultApiDbFile("apis.db");
    const QString kQuery = tr("select api from api_table where lang=\"%1\"").arg(lang);

    // 打开数据库文件
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(kDefaultApiDbFile);
    if (!db.open()) {
        return false;
    }

    // 从指定语言表中读取API条目
    QSqlQuery query;
    query.exec(kQuery);

    // 把所有条目转存到变量中
    while (query.next()) {
        m_apis.append(query.value(0).toString());
    }

    return true;
}

/*!
 * \brief 从原始的API中提取出经过排序和过滤的API.
 */
void SimAPIs::startExtraction()
{
    if (m_extractor != nullptr) {
        return;
    }

    m_extractor = new SimAPIsExtractor(m_apis, this);
    connect(m_extractor, SIGNAL(finished()), this, SIGNAL(apiExtractionFinished()));
    m_extractor->start();

    emit apiExtractionStarted();
}

/*!
 * \brief 取消API提取过程.
 */
void SimAPIs::cancelExtraction()
{
    if (m_extractor != nullptr) {
        if (m_extractor->isRunning()) {
            m_extractor->wait(500);
            m_extractor->stop();
            m_extractor->deleteLater();
        }
        emit apiExtractionCanceled();
    }
}

/*!
 * \reimp
 */
void SimAPIs::updateAutoCompletionList(const QStringList &context, QStringList &list)
{
    QString path;
    QStringList newContext = getPositionOrigin(context, path);

    if (m_originLength > 0) {
        const QString wsep = lexer()->autoCompletionWordSeparators().first();
        QStringList::const_iterator it = m_origin;

        m_unambiguousContext = path;

        while (it != m_apis.end()) {
            QString base = SimAPIsExtractor::getApiBaseName(*it);

            if (!base.startsWith(path))
                break;

            // Make sure we have something after the path.
            if (base != path) {
                // Get the word we are interested in (ie. the one after the
                // current origin in path).
                QString w = base.mid(m_originLength + wsep.length()).split(wsep).first();

                // Append the space, we know the origin is unambiguous.
                w.append(' ');

                if (!list.contains(w))
                    list << w;
            }

            ++it;
        }
    } else {
        // 此时我们把这些单词加入到多个上下文中.
        m_unambiguousContext.truncate(0);

        bool unambig = true;
        QStringList with_context;

        if (newContext.last().isEmpty())
            lastCompleteWord(newContext[newContext.count() - 2], with_context, unambig);
        else
            lastPartialWord(newContext.last(), with_context, unambig);

        for (int i = 0; i < with_context.count(); ++i) {
            // 去除不明确的上下文.
            QString noc = with_context[i];

            if (unambig) {
                int op = noc.indexOf('(');

                if (op >= 0)
                    noc.truncate(op);
            }

            list << noc;
        }
    }
}

/*!
 * \reimp
 */
void SimAPIs::autoCompletionSelected(const QString &selection)
{
    // 如果选中的是一个API, 那么就记录它的在API列表中的位置.
    QStringList lst = selection.split(' ');

    if (lst.count() != 2) {
        m_originLength = 0;
        return;
    }

    const QString &path = lst[1];
    QString owords;

    if (path.isEmpty())
        owords = m_unambiguousContext;
    else {
        // 检查括号.
        if (!path.startsWith("(") || !path.endsWith(")")) {
            m_originLength = 0;
            return;
        }

        // 去除括号.
        owords = path.mid(1, path.length() - 2);
    }

    m_origin = qLowerBound(m_apis, owords);
    m_originLength = owords.length();
}

/*!
 * \reimp
 */
QStringList SimAPIs::callTips(const QStringList &context, int commas, SimSci::CallTipsStyle style, QList<int> &shifts)
{
    QString path;
    QStringList newContext = getPositionOrigin(context, path);
    QStringList wseps = lexer()->autoCompletionWordSeparators();
    QStringList cts;

    if (m_originLength > 0) {
        QStringList::const_iterator it = m_origin;
        QString prev;

        // 计算上下文的长度
        const QString &wsep = wseps.first();
        QStringList strip = path.split(wsep);
        strip.removeLast();
        int ctstart = strip.join(wsep).length();

        if (ctstart)
            ctstart += wsep.length();

        int shift;

        if (style == SimSci::CTS_Context) {
            shift = ctstart;
            ctstart = 0;
        } else {
            shift = 0;
        }

        // 确认只找出我们感兴趣的函数
        path.append('(');

        while (it != m_apis.end() && (*it).startsWith(path)) {
            QString w = (*it).mid(ctstart);

            if (w != prev && hasEnoughCommas(w, commas)) {
                shifts << shift;
                cts << w;
                prev = w;
            }

            ++it;
        }
    } else {
        const QString &fname = newContext[newContext.count() - 2];

        // Find everywhere the function name appears in the APIs.
        const SimAPIs::WordCoordinateList *wil = getWordIndexOf(fname);

        if (wil)
            for (int i = 0; i < wil->count(); ++i) {
                const SimAPIs::WordCoordinate &wi = (*wil)[i];
                QStringList awords = SimAPIsExtractor::getApiWords(m_apis.at(wi.first), wseps.first(), true);

                // Check the word is the function name and not part of any
                // context.
                if (wi.second != awords.count() - 1)
                    continue;

                const QString &api = m_apis[wi.first];

                int tail = api.indexOf('(');

                if (tail < 0)
                    continue;

                if (!hasEnoughCommas(api, commas))
                    continue;

                if (style == SimSci::CTS_NoContext) {
                    shifts << 0;
                    cts << (fname + api.mid(tail));
                } else {
                    shifts << tail - fname.length();
                    cts << api;
                }
            }
    }
    return cts;
}

QStringList SimAPIs::getPositionOrigin(const QStringList &context, QString &path)
{
    // 获取单词列表，检查上下文是否和上次被调用时传入的一样
    QStringList newContext;
    bool isSameContext = (m_oldContext.count() > 0 && m_oldContext.count() < context.count());

    for (int i = 0; i < context.count(); ++i) {
        QString word = context[i];

        if (!lexer()->isCaseSensitive())
            word = word.toUpper();

        if (i < m_oldContext.count() && m_oldContext[i] != word)
            isSameContext = false;

        newContext << word;
    }

    // 如果上下文改变了那么就重置起点位置
    if (!isSameContext)
        m_originLength = 0;

    // If we have a current mOrigin (ie. the user made a specific selection in
    // the current context) then adjust the mOrigin to include the last complete
    // word as the user may have entered more parts of the name without using
    // auto-completion.
    if (m_originLength > 0) {
        const QString wsep = lexer()->autoCompletionWordSeparators().first();

        int startNew = m_oldContext.count();
        int endNew = newContext.count() - 1;

        if (startNew == endNew) {
            path = m_oldContext.join(wsep);
            m_originLength = path.length();
        } else {
            QString fixed = *m_origin;
            fixed.truncate(m_originLength);

            path = fixed;

            while (startNew < endNew) {
                // 把这个单词添加到当前的路径中
                path.append(wsep);
                path.append(newContext[startNew]);
                m_originLength = path.length();

                // 忽略当前位置上不与路径匹配的条目
                while (m_origin != m_apis.end()) {
                    // See if the current mOrigin has come to an end.
                    if (!isOriginStartsWith(fixed, wsep))
                        m_origin = m_apis.end();
                    else if (isOriginStartsWith(path, wsep))
                        break;
                    else
                        ++m_origin;
                }

                if (m_origin == m_apis.end())
                    break;

                ++startNew;
            }
        }

        // Terminate the path.
        // 终止路径
        path.append(wsep);

        // If the new text wasn't recognised then reset the mOrigin.
        if (m_origin == m_apis.end())
            m_originLength = 0;
    }

    if (m_originLength == 0)
        path.truncate(0);

    // Save the "committed" context for next time.
    m_oldContext = newContext;
    m_oldContext.removeLast();

    return newContext;
}

bool SimAPIs::hasEnoughCommas(const QString &s, int commas) {
    int end = s.indexOf(')');

    if (end < 0)
        return false;

    QString w = s.left(end);

    return (w.count(',') >= commas);
}

const SimAPIs::WordCoordinateList *SimAPIs::getWordIndexOf(const QString &word) const
{
    if (m_extractor == nullptr) {
        return nullptr;
    }

    QString csword;

    // Indirect through the case dictionary if the language isn't case
    // sensitive.
    if (lexer()->isCaseSensitive())
        csword = word;
    else {
        csword = m_extractor->mWord2WordMap[word];

        if (csword.isEmpty())
            return 0;
    }

    // Get the possible API entries if any.
    const WordCoordinateList *wl = &m_extractor->mWord2CordMap[csword];

    if (wl->isEmpty())
        return 0;


    return wl;
}

bool SimAPIs::isOriginStartsWith(const QString &path, const QString &wsep)
{
    const QString &orig = *m_origin;

    if (!orig.startsWith(path))
        return false;

    // Check that the path corresponds to the end of a word, ie. that what
    // follows in the mOrigin is either a word separator or a (.
    QString tail = orig.mid(path.length());

    return (!tail.isEmpty() && (tail.startsWith(wsep) || tail.at(0) == '('));
}

// Add auto-completion words based on the last complete word entered.
void SimAPIs::lastCompleteWord(const QString &word, QStringList &with_context, bool &unambig)
{
    // Get the possible API entries if any.
    const WordCoordinateList *wl = getWordIndexOf(word);

    if (wl)
        addAPIEntries(*wl, true, with_context, unambig);
}

void SimAPIs::lastPartialWord(const QString &word, QStringList &with_context, bool &unambig)
{
    if (m_extractor == nullptr) {
        return;
    }

    if (lexer()->isCaseSensitive()) {
        QMap<QString, WordCoordinateList>::const_iterator it = m_extractor->mWord2CordMap.lowerBound(word);

        while (it != m_extractor->mWord2CordMap.end()) {
            if (!it.key().startsWith(word))
                break;

            addAPIEntries(it.value(), false, with_context, unambig);

            ++it;
        }
    } else {
#if 0
        QMap<QString, QString>::const_iterator it = m_Extractor->mWord2WordMap.lowerBound(word);

        while (it != m_Extractor->mWord2WordMap.end()) {
#ifdef USELOG
            LOG_VAR(it.key());
#endif
            if (!it.key().startsWith(word))
                break;

            addAPIEntries(m_Extractor->mWord2CordMap[it.value()], false, with_context, unambig);
            ++it;
        }
#endif
        foreach(const QString& api, m_apis) {
            if (api.startsWith(word, Qt::CaseInsensitive)) {
                with_context.append(api);
            }
        }
    }
}
void SimAPIs::addAPIEntries(const WordCoordinateList &wl, bool complete, QStringList &with_context, bool &unambig)
{
    if (m_extractor == nullptr) {
        return;
    }

    QStringList wseps = lexer()->autoCompletionWordSeparators();

    for (int w = 0; w < wl.count(); ++w) {
        const WordCoordinate &wi = wl[w];

        QStringList api_words = m_extractor->getApiWords(m_apis.at(wi.first), wseps.first(), false);

        int idx = wi.second;

        if (complete) {
            // 如果是最后一个单词就忽略掉.
            if (++idx >= api_words.count())
                continue;
        }

        QString api_word;

        if (idx == 0) {
            api_word = api_words[0] + ' ';
        } else {
            QStringList orgl = api_words.mid(0, idx);

            QString org = orgl.join(wseps.first());

            api_word = QString("%1 (%2)").arg(api_words[idx]).arg(org);

            // 查看是否已经检查过了起始点
            if (unambig)
                if (m_unambiguousContext.isEmpty()) {
                    m_unambiguousContext = org;
                } else if (m_unambiguousContext != org) {
                    m_unambiguousContext.truncate(0);
                    unambig = false;
                }
        }

        if (!with_context.contains(api_word))
            with_context.append(api_word);
    }
}
