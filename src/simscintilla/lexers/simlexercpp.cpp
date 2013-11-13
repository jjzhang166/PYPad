#include "simlexercpp.h"

#ifdef USELOG
#include "../logutil.h"
#endif
struct SimLexerCppPrivate {
    SimLexerCppPrivate(SimLexerCpp* qptr): q(qptr) {}
    bool isFoldAtElse : 1;
    bool isFoldComments : 1;
    bool isFoldCompact : 1;
    bool isFoldPreprocessor : 1;
    bool isStylePreprocessor : 1;
    bool isDollarsAllowed : 1;
    bool isHighlightTriple : 1;
    bool isHighlightHash : 1;

    bool isKeywordsCaseSensitive;

    void notifyAtElsePropChanged();
    void notifyCommentPropChanged();
    void notifyCompactPropChanged();
    void notifyPreprocPropChanged();
    void notifyStylePreprocPropChanged();
    void notifyDollarsPropChanged();
    void notifyHighlightTriplePropChanged();
    void notifyHighlightHashPropChanged();

    SimLexerCpp* q;
};

void SimLexerCppPrivate::notifyAtElsePropChanged() {
    emit q->propertyChanged("fold.at.else",(isFoldAtElse ? "1" : "0"));
}

void SimLexerCppPrivate::notifyCommentPropChanged() {
    emit q->propertyChanged("fold.comment",(isFoldComments ? "1" : "0"));
}

void SimLexerCppPrivate::notifyCompactPropChanged() {
    emit q->propertyChanged("fold.compact",(isFoldCompact ? "1" : "0"));
}

void SimLexerCppPrivate::notifyPreprocPropChanged() {
    emit q->propertyChanged("fold.preprocessor",(isFoldPreprocessor ? "1" : "0"));
}

void SimLexerCppPrivate::notifyStylePreprocPropChanged() {
    emit q->propertyChanged("styling.within.preprocessor",(isStylePreprocessor ? "1" : "0"));
}

void SimLexerCppPrivate::notifyDollarsPropChanged() {
    emit q->propertyChanged("lexer.cpp.allow.dollars",(isDollarsAllowed ? "1" : "0"));
}

void SimLexerCppPrivate::notifyHighlightTriplePropChanged() {
    emit q->propertyChanged("lexer.cpp.triplequoted.strings", (isHighlightTriple ? "1" : "0"));
}

void SimLexerCppPrivate::notifyHighlightHashPropChanged() {
    emit q->propertyChanged("lexer.cpp.hashquoted.strings", (isHighlightHash ? "1" : "0"));
}

SimLexerCpp::SimLexerCpp(SimScintilla *parent): SimAbstractLexer(parent) {
    d = new SimLexerCppPrivate(this);
}

SimLexerCpp::~SimLexerCpp() {
    SAFE_RELEASE(d);
}

/*!
 * \reimp
 */
QString SimLexerCpp::lexerStr() const {
    if (d->isKeywordsCaseSensitive) {
        return "cpp";
    } else {
        return "cppnocase";
    }
}

/*!
 * \reimp
 */
QString SimLexerCpp::description(int style) const {
    switch (style)
    {
    case Default:
        return tr("Default");

    case InactiveDefault:
        return tr("Inactive default");

    case Comment:
        return tr("C comment");

    case InactiveComment:
        return tr("Inactive C comment");

    case CommentLine:
        return tr("C++ comment");

    case InactiveCommentLine:
        return tr("Inactive C++ comment");

    case CommentDoc:
        return tr("JavaDoc style C comment");

    case InactiveCommentDoc:
        return tr("Inactive JavaDoc style C comment");

    case Number:
        return tr("Number");

    case InactiveNumber:
        return tr("Inactive number");

    case Keyword:
        return tr("Keyword");

    case InactiveKeyword:
        return tr("Inactive keyword");

    case DoubleQuotedString:
        return tr("Double-quoted string");

    case InactiveDoubleQuotedString:
        return tr("Inactive double-quoted string");

    case SingleQuotedString:
        return tr("Single-quoted string");

    case InactiveSingleQuotedString:
        return tr("Inactive single-quoted string");

    case UUID:
        return tr("IDL UUID");

    case InactiveUUID:
        return tr("Inactive IDL UUID");

    case PreProcessor:
        return tr("Pre-processor block");

    case InactivePreProcessor:
        return tr("Inactive pre-processor block");

    case Operator:
        return tr("Operator");

    case InactiveOperator:
        return tr("Inactive operator");

    case Identifier:
        return tr("Identifier");

    case InactiveIdentifier:
        return tr("Inactive identifier");

    case UnclosedString:
        return tr("Unclosed string");

    case InactiveUnclosedString:
        return tr("Inactive unclosed string");

    case VerbatimString:
        return tr("C# verbatim string");

    case InactiveVerbatimString:
        return tr("Inactive C# verbatim string");

    case Regex:
        return tr("JavaScript regular expression");

    case InactiveRegex:
        return tr("Inactive JavaScript regular expression");

    case CommentLineDoc:
        return tr("JavaDoc style C++ comment");

    case InactiveCommentLineDoc:
        return tr("Inactive JavaDoc style C++ comment");

    case KeywordSet2:
        return tr("Secondary keywords and identifiers");

    case InactiveKeywordSet2:
        return tr("Inactive secondary keywords and identifiers");

    case CommentDocKeyword:
        return tr("JavaDoc keyword");

    case InactiveCommentDocKeyword:
        return tr("Inactive JavaDoc keyword");

    case CommentDocKeywordError:
        return tr("JavaDoc keyword error");

    case InactiveCommentDocKeywordError:
        return tr("Inactive JavaDoc keyword error");

    case GlobalClass:
        return tr("Global classes and typedefs");

    case InactiveGlobalClass:
        return tr("Inactive global classes and typedefs");

    case RawString:
        return tr("C++ raw string");

    case InactiveRawString:
        return tr("Inactive C++ raw string");
    }

    return QString();
}

/*!
 * \reimp
 */
QStringList SimLexerCpp::autoCompletionWordSeparators() const {
    QStringList wl;

    wl << "::" << "->" << ".";

    return wl;
}

/*!
 * \reimp
 */
QString SimLexerCpp::blockEnd(int *style) const {
    if (style)
        *style = Operator;

    return "}";
}

/*!
 * \reimp
 */
QString SimLexerCpp::blockStart(int *style) const {
    if (style)
        *style = Operator;

    return "{";
}

/*!
 * \reimp
 */
QString SimLexerCpp::blockStartKeyword(int *style) const {
    if (style)
        *style = Keyword;

    return "case catch class default do else finally for if private "
           "protected public struct try union while";
}

/*!
 * \reimp
 */
int SimLexerCpp::braceStyle() const {
    return Operator;
}

/*!
 * \reimp
 */
QString SimLexerCpp::wordCharacters() const {
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#";
}

/*!
 * \reimp
 */
QColor SimLexerCpp::defaultColor(int style) const {
#ifdef USELOG
    LOG_VAR(style);
#endif
    switch (style)
    {
    case Default:
        return QColor(0x80, 0x80, 0x80);

    case Comment:
    case CommentLine:
        return QColor(0x00, 0x7f, 0x00);

    case CommentDoc:
    case CommentLineDoc:
        return QColor(0x3f, 0x70, 0x3f);

    case Number:
        return QColor(0x00, 0x7f, 0x7f);

    case Keyword:
    case KeywordSet2:
        return QColor(0x00, 0x00, 0x9f);

    case DoubleQuotedString:
    case SingleQuotedString:
    case RawString:
        return QColor(0x7f, 0x00, 0x7f);

    case PreProcessor:
        return QColor(0x7f, 0x7f, 0x00);

    case Operator:
    case UnclosedString:
        return QColor(0x00, 0x00, 0x00);

    case VerbatimString:
    case TripleQuotedVerbatimString:
    case HashQuotedString:
        return QColor(0x00, 0x7f, 0x00);

    case Regex:
        return QColor(0x3f, 0x7f, 0x3f);

    case CommentDocKeyword:
        return QColor(0x30, 0x60, 0xa0);

    case CommentDocKeywordError:
        return QColor(0x80, 0x40, 0x20);

    case PreProcessorComment:
        return QColor(0x65, 0x99, 0x00);

    case InactiveDefault:
    case InactiveUUID:
    case InactiveCommentLineDoc:
    case InactiveKeywordSet2:
    case InactiveCommentDocKeyword:
    case InactiveCommentDocKeywordError:
        return QColor(0xc0, 0xc0, 0xc0);

    case InactiveComment:
    case InactiveCommentLine:
    case InactiveNumber:
    case InactiveVerbatimString:
    case InactiveTripleQuotedVerbatimString:
    case InactiveHashQuotedString:
        return QColor(0x90, 0xb0, 0x90);

    case InactiveCommentDoc:
        return QColor(0xd0, 0xd0, 0xd0);

    case InactiveKeyword:
        return QColor(0x90, 0x90, 0xb0);

    case InactiveDoubleQuotedString:
    case InactiveSingleQuotedString:
    case InactiveRawString:
        return QColor(0xb0, 0x90, 0xb0);

    case InactivePreProcessor:
        return QColor(0xb0, 0xb0, 0x90);

    case InactiveOperator:
    case InactiveIdentifier:
    case InactiveGlobalClass:
        return QColor(0xb0, 0xb0, 0xb0);

    case InactiveUnclosedString:
        return QColor(0x00, 0x00, 0x00);

    case InactiveRegex:
        return QColor(0x7f, 0xaf, 0x7f);

    case InactivePreProcessorComment:
        return QColor(0xa0, 0xc0, 0x90);
    }

    return SimAbstractLexer::defaultColor(style);
}

/*!
 * \reimp
 */
bool SimLexerCpp::defaultEolFill(int style) const {
    switch (style)
    {
    case UnclosedString:
    case InactiveUnclosedString:
    case VerbatimString:
    case InactiveVerbatimString:
    case Regex:
    case InactiveRegex:
    case TripleQuotedVerbatimString:
    case InactiveTripleQuotedVerbatimString:
    case HashQuotedString:
    case InactiveHashQuotedString:
        return true;
    }

    return SimAbstractLexer::defaultEolFill(style);
}

/*!
 * \reimp
 */
QFont SimLexerCpp::defaultFont(int style) const {
    QFont f;

    switch (style)
    {
    case Comment:
    case InactiveComment:
    case CommentLine:
    case InactiveCommentLine:
    case CommentDoc:
    case InactiveCommentDoc:
    case CommentLineDoc:
    case InactiveCommentLineDoc:
    case CommentDocKeyword:
    case InactiveCommentDocKeyword:
    case CommentDocKeywordError:
    case InactiveCommentDocKeywordError:
#if defined(Q_OS_WIN)
        f = QFont("Comic Sans MS",9);
#elif defined(Q_OS_MAC)
        f = QFont("Comic Sans MS", 12);
#else
        f = QFont("Bitstream Vera Serif",9);
#endif
        break;

    case Keyword:
    case KeywordSet2:
    case InactiveKeyword:
    case Operator:
    case InactiveOperator:
        f = SimAbstractLexer::defaultFont(style);
        f.setBold(true);
        break;

    case DoubleQuotedString:
    case InactiveDoubleQuotedString:
    case SingleQuotedString:
    case InactiveSingleQuotedString:
    case UnclosedString:
    case InactiveUnclosedString:
    case VerbatimString:
    case InactiveVerbatimString:
    case Regex:
    case InactiveRegex:
    case TripleQuotedVerbatimString:
    case InactiveTripleQuotedVerbatimString:
    case HashQuotedString:
    case InactiveHashQuotedString:
#if defined(Q_OS_WIN)
        f = QFont("Courier New",10);
#elif defined(Q_OS_MAC)
        f = QFont("Courier", 12);
#else
        f = QFont("Bitstream Vera Sans Mono",9);
#endif
        break;

    default:
        f = SimAbstractLexer::defaultFont(style);
    }

    return f;
}

/*!
 * \reimp
 */
QColor SimLexerCpp::defaultBackgroundColor(int style) const {
    switch (style)
    {
    case UnclosedString:
    case InactiveUnclosedString:
        return QColor(0xe0,0xc0,0xe0);

    case VerbatimString:
    case InactiveVerbatimString:
    case TripleQuotedVerbatimString:
    case InactiveTripleQuotedVerbatimString:
        return QColor(0xe0,0xff,0xe0);

    case Regex:
    case InactiveRegex:
        return QColor(0xe0,0xf0,0xe0);

    case RawString:
    case InactiveRawString:
        return QColor(0xff,0xf3,0xff);

    case HashQuotedString:
    case InactiveHashQuotedString:
        return QColor(0xe7,0xff,0xd7);
    }

    return SimAbstractLexer::defaultBackgroundColor(style);
}

/*!
 * \reimp
 */
QString SimLexerCpp::keywords(int set) const {
    if (set == 1)
        return
            "and and_eq asm auto bitand bitor bool break case "
            "catch char class compl const const_cast continue "
            "default delete do double dynamic_cast else enum "
            "explicit export extern false float for friend goto if "
            "inline int long mutable namespace new not not_eq "
            "operator or or_eq private protected public register "
            "reinterpret_cast return short signed sizeof static "
            "static_cast struct switch template this throw true "
            "try typedef typeid typename union unsigned using "
            "virtual void volatile wchar_t while xor xor_eq"
            "const vo";

    if (set == 3)
        return
            "a addindex addtogroup anchor arg attention author b "
            "brief bug c class code date def defgroup deprecated "
            "dontinclude e em endcode endhtmlonly endif "
            "endlatexonly endlink endverbatim enum example "
            "exception f$ f[ f] file fn hideinitializer "
            "htmlinclude htmlonly if image include ingroup "
            "internal invariant interface latexonly li line link "
            "mainpage name namespace nosubgrouping note overload "
            "p page par param post pre ref relates remarks return "
            "retval sa section see showinitializer since skip "
            "skipline struct subsection test throw todo typedef "
            "union until var verbatim verbinclude version warning "
            "weakgroup $ @ \\ & < > # { }";

    return QString();
}

/*!
 * \brief 更新属性信息.
 */
void SimLexerCpp::refreshProperties() {
    d->notifyAtElsePropChanged();
    d->notifyCommentPropChanged();
    d->notifyCompactPropChanged();
    d->notifyDollarsPropChanged();
    d->notifyHighlightHashPropChanged();
    d->notifyHighlightTriplePropChanged();
    d->notifyPreprocPropChanged();
    d->notifyStylePreprocPropChanged();
}

/*!
 * \brief 返回是否在else处折叠.
 */
bool SimLexerCpp::isFoldAtElse() const {
    return d->isFoldAtElse;
}

/*!
 * \brief 获取是否折叠注释.
 */
bool SimLexerCpp::isFoldComments() const {
    return d->isFoldComments;
}

/*!
 * \brief 获取是否紧凑折叠.
 */
bool SimLexerCpp::isFoldCompact() const {
    return d->isFoldCompact;
}

/*!
 * \brief 获取是否预处理折叠.
 */
bool SimLexerCpp::isFoldPreprocessor() const {
    return d->isFoldPreprocessor;
}

/*!
 * \brief 获取是否预处理高亮显示.
 */
bool SimLexerCpp::isStylePreprocessor() const {
    return d->isStylePreprocessor;
}

/*!
 * \brief 设置是否允许$号.
 * \param allowed 是否允许
 */
void SimLexerCpp::setDollarsAllowed(bool allowed) {
    d->isDollarsAllowed = allowed;
}

/*!
 * \brief 获取是否允许$号.
 */
bool SimLexerCpp::isDollarsAllowed() const {
    return d->isDollarsAllowed;
}

/*!
 * \brief 设置是否对三重引用的字符串采用高亮显示.
 * \param enabled 是否应用
 */
void SimLexerCpp::setHighlightTripleQuotedStrings(bool enabled) {
    d->isHighlightTriple = enabled;
}

/*!
 * \brief 返回是否对三重引用的字符串采用高亮显示.
 */
bool SimLexerCpp::isHighlightTripleQuotedStrings() const {
    return d->isHighlightTriple;
}

/*!
 * \brief 设置使用对#号引用的字符串采用高亮.
 * \param enabled 是否应用
 */
void SimLexerCpp::setHighlightHashQuotedStrings(bool enabled) {
    d->isHighlightHash = enabled;
}

/*!
 * \brief 返回是否对#号引用的字符串应用高亮显示.
 */
bool SimLexerCpp::isHighlightHashQuotedStrings() const {
    return d->isHighlightHash;
}

/*!
 * \brief 设置关键字是否大小写敏感.
 * \param cs 大小写敏感
 */
void SimLexerCpp::setKeywordsCaseSensitive(bool cs) {
    d->isKeywordsCaseSensitive = cs;
}

/*!
 * \brief 获取关键字是否大小写敏感.
 * \return
 */
bool SimLexerCpp::isKeywordsCaseSensitive() const {
    return d->isKeywordsCaseSensitive;
}

/*!
 * \brief 设置是否在Else出折叠.
 * \param fold 是否折叠
 * \sa isFoldAtElse()
 */
void SimLexerCpp::setFoldAtElse(bool fold) {
    d->isFoldAtElse = fold;
    d->notifyAtElsePropChanged();
}

/*!
 * \brief 设置是否在注释处折叠.
 * \param fold 折叠
 * \sa isFoldComments()
 */
void SimLexerCpp::setFoldComments(bool fold) {
    d->isFoldComments = fold;
    d->notifyCommentPropChanged();
}

/*!
 * \brief 设置是否紧凑折叠.
 * \param fold 折叠
 * \sa isFoldCompact()
 */
void SimLexerCpp::setFoldCompact(bool fold) {
    d->isFoldCompact = fold;
    d->notifyCompactPropChanged();
}

/*!
 * \brief 设置是否在预处理处折叠.
 * \param fold 折叠
 * \sa isFoldPreprocessor()
 */
void SimLexerCpp::setFoldPreprocessor(bool fold) {
    d->isFoldPreprocessor = fold;
    d->notifyPreprocPropChanged();
}


/*!
 * \brief 设置是否在预处理后的语句应用自定义样式.
 * \param style 是否采用样式
 * /sa isStylePreprocessor()
 */
void SimLexerCpp::setStylePreprocessor(bool style) {
    d->isStylePreprocessor = style;
    d->notifyStylePreprocPropChanged();
}

/*!
 * \brief 从QSettings中读取相应的字段数值并赋予到变量中.
 * \param qs
 * \param prefix
 */
void SimLexerCpp::readProperties(QSettings &qs, const QString &prefix) {

    d->isFoldAtElse = qs.value(prefix + "foldatelse", false).toBool();
    d->isFoldComments = qs.value(prefix + "foldcomments", false).toBool();
    d->isFoldCompact = qs.value(prefix + "foldcompact", true).toBool();
    d->isFoldPreprocessor = qs.value(prefix + "foldpreprocessor", true).toBool();
    d->isStylePreprocessor = qs.value(prefix + "stylepreprocessor", false).toBool();
    d->isDollarsAllowed = qs.value(prefix + "dollars", true).toBool();
    d->isHighlightTriple = qs.value(prefix + "highlighttriple", false).toBool();
    d->isHighlightHash = qs.value(prefix + "highlighthash", false).toBool();
}

/*!
 * \brief 将变量值写入到QSettings中
 * \param qs
 * \param prefix
 */
void SimLexerCpp::writeProperties(QSettings &qs, const QString &prefix) {
    qs.setValue(prefix + "foldatelse", d->isFoldAtElse);
    qs.setValue(prefix + "foldcomments", d->isFoldComments);
    qs.setValue(prefix + "foldcompact", d->isFoldCompact);
    qs.setValue(prefix + "foldpreprocessor", d->isFoldPreprocessor);
    qs.setValue(prefix + "stylepreprocessor", d->isStylePreprocessor);
    qs.setValue(prefix + "dollars", d->isDollarsAllowed);
    qs.setValue(prefix + "highlighttriple", d->isHighlightTriple);
    qs.setValue(prefix + "highlighthash", d->isHighlightHash);
}
