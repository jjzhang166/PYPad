#ifndef SIMAPIS_H
#define SIMAPIS_H

#include "simabstractapis.h"
#include "simscintilla_global.h"

#include <QStringList>

class SimAbstractLexer;
class SimAPIsExtractor;

class SIMSCINTILLA_EXPORT SimAPIs : public SimAbstractAPIs
{
    Q_OBJECT

public:

    typedef QPair<quint32, quint32> WordCoordinate;
    typedef QList<WordCoordinate> WordCoordinateList;

    explicit SimAPIs(SimAbstractLexer* parent = 0);
    virtual ~SimAPIs();

    void add(const QString& api);
    void remove(const QString& api);
    void clear();

    bool load(const QString& lang);

    void startExtraction();
    void cancelExtraction();

    void updateAutoCompletionList(const QStringList &context, QStringList &list);
    void autoCompletionSelected(const QString &selection);
    QStringList callTips(const QStringList &context, int commas, SimSci::CallTipsStyle style,QList<int> &shifts);

signals:

    /*!
     * \brief 开始提取API时发射此信号
     */
    void apiExtractionStarted();

    /*!
     * \brief 取消API提取时发射此信号
     */
    void apiExtractionCanceled();

    /*!
     * \brief 结束API提取时发射此信号
     */
    void apiExtractionFinished();

private:

    QStringList getPositionOrigin(const QStringList &context, QString &path);
    bool hasEnoughCommas(const QString &s, int commas);
    const WordCoordinateList *getWordIndexOf(const QString &word) const;
    bool isOriginStartsWith(const QString &path, const QString &wsep);
    void lastCompleteWord(const QString &word, QStringList &with_context, bool &unambig);
    void lastPartialWord(const QString &word, QStringList &with_context, bool &unambig);
    void addAPIEntries(const WordCoordinateList &wl, bool complete, QStringList &with_context, bool &unambig);

    SimAPIsExtractor* m_extractor;
    QStringList m_apis;
    QStringList m_oldContext;
    QStringList::const_iterator m_origin;
    int m_originLength;
    QString m_unambiguousContext;
    Q_DISABLE_COPY(SimAPIs)
};

#endif // SIMAPIS_H
