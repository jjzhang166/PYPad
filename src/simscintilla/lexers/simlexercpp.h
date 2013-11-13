#ifndef SIMLEXERCPP_H
#define SIMLEXERCPP_H

#include "../simabstractlexer.h"

#include <QColor>
#include <QFont>

class SimScintilla;
class SimLexerFactory;
class SimLexerCppPrivate;

class SimLexerCpp : public SimAbstractLexer
{
    Q_OBJECT

public:

    //! 如下枚举类型定义了为C++语法分析器所用的不同风格
    enum Tag {
        //! 默认
        Default = 0,
        InactiveDefault = Default + 64,

        //! C 注释
        Comment = 1,
        InactiveComment = Comment + 64,

        //! C++ 注释.
        CommentLine = 2,
        InactiveCommentLine = CommentLine + 64,

        //! JavaDoc/Doxygen 风格的C注释.
        CommentDoc = 3,
        InactiveCommentDoc = CommentDoc + 64,

        //! 数字.
        Number = 4,
        InactiveNumber = Number + 64,

        //! 关键字.
        Keyword = 5,
        InactiveKeyword = Keyword + 64,

        //! 双引号引用的字符串.
        DoubleQuotedString = 6,
        InactiveDoubleQuotedString = DoubleQuotedString + 64,

        //! 单引号引用的字符串.
        SingleQuotedString = 7,
        InactiveSingleQuotedString = SingleQuotedString + 64,

        //! IDL UUID.
        UUID = 8,
        InactiveUUID = UUID + 64,

        //! 预处理块.
        PreProcessor = 9,
        InactivePreProcessor = PreProcessor + 64,

        //! 操作符.
        Operator = 10,
        InactiveOperator = Operator + 64,

        //! 识别符.
        Identifier = 11,
        InactiveIdentifier = Identifier + 64,

        //! 未结束字符串的行尾.
        UnclosedString = 12,
        InactiveUnclosedString = UnclosedString + 64,

        //! C# 逐字字符串.
        VerbatimString = 13,
        InactiveVerbatimString = VerbatimString + 64,

        //! JavaScript 正则表达式.
        Regex = 14,
        InactiveRegex = Regex + 64,

        //! JavaDoc/Doxygen 风格的C++注释.
        CommentLineDoc = 15,
        InactiveCommentLineDoc = CommentLineDoc + 64,

        //! 集合2中定义的关键字.这个类必须继承并重新实现keywords函数来使用这个风格
        KeywordSet2 = 16,
        InactiveKeywordSet2 = KeywordSet2 + 64,

        //! JavaDoc/Doxygen 关键字.
        CommentDocKeyword = 17,
        InactiveCommentDocKeyword = CommentDocKeyword + 64,

        //! JavaDoc/Doxygen 关键字错误.
        CommentDocKeywordError = 18,
        InactiveCommentDocKeywordError = CommentDocKeywordError + 64,

        //! 在集合5中定义的全局类或者定义别名.这个类必须继承并实现keywords来使用这个风格.
        GlobalClass = 19,
        InactiveGlobalClass = GlobalClass + 64,

        //! C++原始字符串.
        RawString = 20,
        InactiveRawString = RawString + 20,

        //! Vala 三重引用的逐字字符串.
        TripleQuotedVerbatimString = 21,
        InactiveTripleQuotedVerbatimString = TripleQuotedVerbatimString + 21,

        //! Pike #字引用的字符串.
        HashQuotedString = 22,
        InactiveHashQuotedString = HashQuotedString + 22,

        //! 预处理流注释.
        PreProcessorComment = 23,
        InactivePreProcessorComment = PreProcessorComment + 23
    };


    int lang() const { return SCLEX_CPP; }
    QString lexerStr() const;
    QString description(int style) const;

    QStringList autoCompletionWordSeparators() const;

    QString blockEnd(int *style = 0) const;
    QString blockStart(int *style = 0) const;
    QString blockStartKeyword(int *style = 0) const;

    int braceStyle() const;
    QString wordCharacters() const;

    QColor defaultColor(int style) const;
    bool defaultEolFill(int style) const;
    QFont defaultFont(int style) const;
    QColor defaultBackgroundColor(int style) const;
    QString keywords(int set) const;

    void refreshProperties();

    bool isFoldAtElse() const;
    bool isFoldComments() const;
    bool isFoldCompact() const;
    bool isFoldPreprocessor() const;
    bool isStylePreprocessor() const;

    void setDollarsAllowed(bool allowed);
    bool isDollarsAllowed() const;

    void setHighlightTripleQuotedStrings(bool enabled);
    bool isHighlightTripleQuotedStrings() const;

    void setHighlightHashQuotedStrings(bool enabled);
    bool isHighlightHashQuotedStrings() const;

public slots:

    void setKeywordsCaseSensitive(bool cs);
    bool isKeywordsCaseSensitive() const;

    virtual void setFoldAtElse(bool fold);

    virtual void setFoldComments(bool fold);

    virtual void setFoldCompact(bool fold);

    virtual void setFoldPreprocessor(bool fold);

    virtual void setStylePreprocessor(bool style);

protected:
    void readProperties(QSettings &qs, const QString &prefix);
    void writeProperties(QSettings &qs, const QString &prefix);

protected:
    friend class SimLexerFactory;
    explicit SimLexerCpp(SimScintilla* parent = 0);
    ~SimLexerCpp();

    SimLexerCppPrivate* d;
};

#endif // SIMLEXERCPP_H
