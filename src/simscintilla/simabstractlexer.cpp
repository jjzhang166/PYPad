#include "simabstractlexer.h"
#include "simscintilladefs.h"
#include "simscintilla.h"

struct SimAbstractLexerPrivate
{

    Q_DECLARE_PUBLIC(SimAbstractLexer)

    struct SimStyleData
    {
        QFont font;
        QColor color;
        QColor backgroundColor;
        bool eolFill;
    };

    typedef QMap<int, SimStyleData> Style2DataMap;

    SimAbstractLexerPrivate(SimAbstractLexer* parent, SimScintilla* s):
                    q_ptr(parent), scintilla(s), hasInitStyleData(false),
                    defaultColor(Qt::black), defaultBackgroundColor(Qt::white)
    {
        defaultFont = QFont();
    }
    SimScintilla* scintilla;
    SimAbstractAPIs *apiSet;
    SimSci::AutoIndentStyle autoIndentStyle;
    QFont defaultFont;
    QColor defaultColor;
    QColor defaultBackgroundColor;
    bool hasInitStyleData;
    Style2DataMap style2dataMap;

    void initDefaultStyles();
    SimStyleData& getStyleData(int style);
    void setColor(int style, const QColor &c);
    void setEolFill(int style, bool eoffill);
    void setFont(int style, const QFont &f);
    void setBackgroundColor(int style, const QColor &c);

    SimAbstractLexer* q_ptr;
};

void SimAbstractLexerPrivate::initDefaultStyles()
{
    Q_Q(SimAbstractLexer);

    if (!hasInitStyleData) {
        for (int i = 0; i < SIM_STYLE_MAX; ++i)
            if (!q->description(i).isEmpty())
                getStyleData(i);

        hasInitStyleData = true;
    }
}

SimAbstractLexerPrivate::SimStyleData& SimAbstractLexerPrivate::getStyleData(int style)
{
    Q_Q(SimAbstractLexer);

    SimStyleData &styleData = style2dataMap[style];

    // 如果这个StyleData的color是无效的, 那么意味着这个StyleData是最新创建的,
    // 然后初始化所有字段为默认值
    if (!styleData.color.isValid()) {
        styleData.color = q->defaultColor(style);
        styleData.backgroundColor = q->defaultBackgroundColor();
        styleData.font = q->defaultFont();
        styleData.eolFill = q->defaultEolFill(style);
    }

    return styleData;
}

void SimAbstractLexerPrivate::setColor(int style, const QColor &c)
{
    Q_Q(SimAbstractLexer);

    if (style >= 0) {
        getStyleData(style).color = c;
        emit q->colorChanged(style, c);
    }
    else
        for (int i = 0; i < SIM_STYLE_MAX; ++i)
            if (!q->description(i).isEmpty())
                setColor(i, c);
}

void SimAbstractLexerPrivate::setEolFill(int style, bool eoffill)
{
    Q_Q(SimAbstractLexer);

    if (style >= 0) {
        getStyleData(style).eolFill = eoffill;
        emit q->eolFillChanged(style, eoffill);
    }
    else
        for (int i = 0; i < SIM_STYLE_MAX; ++i)
            if (!q->description(i).isEmpty())
                setEolFill(i, eoffill);
}

void SimAbstractLexerPrivate::setFont(int style, const QFont &f)
{
    Q_Q(SimAbstractLexer);

    if (style >= 0) {
        getStyleData(style).font = f;
        emit q->fontChanged(style, f);
    }
    else
        for (int i = 0; i < SIM_STYLE_MAX; ++i)
            if (!q->description(i).isEmpty())
                setFont(i, f);
}

void SimAbstractLexerPrivate::setBackgroundColor(int style, const QColor &c)
{
    Q_Q(SimAbstractLexer);

    if (style >= 0) {
        getStyleData(style).backgroundColor = c;
        emit q->backgroundColorChanged(style, c);
    }
    else
        for (int i = 0; i < SIM_STYLE_MAX; ++i)
            if (!q->description(i).isEmpty())
                setBackgroundColor(i, c);
}

SimAbstractLexer::SimAbstractLexer(SimScintilla *parent) :
    QObject(parent)
{
    d = new SimAbstractLexerPrivate(this, parent);
}

SimAbstractLexer::~SimAbstractLexer()
{
    SAFE_RELEASE(d)
}

int SimAbstractLexer::braceStyle() const
{
    return -1;
}

QString SimAbstractLexer::autoCompletionFillups() const
{
    return "(";
}


QStringList SimAbstractLexer::autoCompletionWordSeparators() const
{
    return QStringList();
}

/*!
 * \brief 获取lexer是否是大小写敏感.
 */
bool SimAbstractLexer::isCaseSensitive() const
{
    return false;
}

/*!
 * \brief 返回API对象指针.
 * \return
 */
SimAbstractAPIs *SimAbstractLexer::apis() const
{
    return d->apiSet;
}

SimSci::AutoIndentStyle SimAbstractLexer::autoIndentStyle() const
{
    if (d->autoIndentStyle < 0)
        d->autoIndentStyle = (!blockStartKeyword().isEmpty() || !blockStart().isEmpty()  || !blockEnd().isEmpty() ) ?
                    SimSci::AIS_None : SimSci::AIS_Maintain;

    return d->autoIndentStyle;
}

QString SimAbstractLexer::blockStart(int* style) const
{
    Q_UNUSED(style);
    return QString();
}

QString SimAbstractLexer::blockEnd(int* style) const
{
    Q_UNUSED(style);
    return QString();
}

int SimAbstractLexer::blockLookback() const
{
    return 20;
}

QString SimAbstractLexer::blockStartKeyword(int* style) const
{
    Q_UNUSED(style);
    return QString();
}

/*!
 * \brief 设置默认颜色.
 * \param col 颜色值
 */
void SimAbstractLexer::setDefaultColor(const QColor& col)
{
    d->defaultColor = col;
}

/*!
 * \brief 获取默认颜色.
 * \return 颜色值
 */
QColor SimAbstractLexer::defaultColor() const
{
    return d->defaultColor;
}

/*!
 * \brief 设置默认字体.
 * \param font 字体
 */
void SimAbstractLexer::setDefaultFont(const QFont& font)
{
    d->defaultFont = font;
}

/*!
 * \brief 获取默认字体.
 * \return 字体
 */
QFont SimAbstractLexer::defaultFont() const
{
    return d->defaultFont;
}

/*!
 * \brief 设置默认背景色.
 * \param col 颜色值
 */
void SimAbstractLexer::setDefaultBackgroundColor(const QColor& col)
{
    d->defaultBackgroundColor = col;
}

/*!
 * \brief 获取默认背景颜色值.
 * \return 颜色值
 */
QColor SimAbstractLexer::defaultBackgroundColor() const
{
    return d->defaultBackgroundColor;
}

void SimAbstractLexer::setEditor(SimScintilla *editor)
{
    d->scintilla = editor;
    if (editor != nullptr) {
        editor->setStyleBits(styleBitsNeeded());
    }
}

SimScintilla* SimAbstractLexer::editor() const
{
    return d->scintilla;
}

/*!
 * \brief 获取默认风格.
 * \return 风格
 */
int SimAbstractLexer::defaultStyle() const
{
    return 0;
}

/*!
 * \brief 获取默认风格的背景颜色.
 * \param style 风格
 * \return 颜色值
 */
QColor SimAbstractLexer::defaultBackgroundColor(int style) const
{
    Q_UNUSED(style);
    return d->defaultBackgroundColor;
}

/*!
 * \brief 获取指定风格的字体.
 * \param style 风格
 * \return 字体
 */
QFont SimAbstractLexer::defaultFont(int style) const
{
    Q_UNUSED(style);
    return d->defaultFont;
}

/*!
 * \brief 获取默认行尾填充.
 * \param style 风格
 * \return 是否填充
 */
bool SimAbstractLexer::defaultEolFill(int style) const
{
    Q_UNUSED(style);
    return false;
}

/*!
 * \brief 获取默认风格的颜色.
 * \param style 风格
 * \return 颜色值
 */
QColor SimAbstractLexer::defaultColor(int style) const
{
    Q_UNUSED(style);
    return d->defaultColor;
}

/*!
 * \brief 获取背景颜色值.
 * \return 颜色值
 */
QColor SimAbstractLexer::backgroundColor() const
{
    return d->defaultBackgroundColor;
}

/*!
 * \brief 获取指定风格背景颜色值.
 * \return 颜色值
 */
QColor SimAbstractLexer::backgroundColor(int style) const
{
    return d->getStyleData(style).backgroundColor;
}

/*!
 * \brief 获取指定字符集的关键字.
 * \param set 字符集
 * \return 关键字
 */
QString SimAbstractLexer::keywords(int set) const
{
    Q_UNUSED(set);
    return QString();
}

QString SimAbstractLexer::wordCharacters() const
{
    return QString();
}

int SimAbstractLexer::indentationGuideView() const
{
    return SimSci::IGV_LookBoth;
}

/*!
 * \brief 刷新属性.
 */
void SimAbstractLexer::refreshProperties()
{

}

int SimAbstractLexer::styleBitsNeeded() const
{
    if (d->scintilla == nullptr) {
        return 5;
    }
    return d->scintilla->styleBitsNeeded();
}
/*!
 * \brief 获取指定风格的前景色即文字颜色.
 * \param 风格
 * \return 颜色
 * \sa setColor(int style)
 */
QColor SimAbstractLexer::color(int style) const
{
    Q_UNUSED(style);
    return defaultColor();
}

/*!
 * \brief 获取关键字的颜色
 * \param set 字符集
 * \return 颜色值
 */
QColor SimAbstractLexer::keywordColor(int set) const
{
    Q_UNUSED(set);
    return QColor(92, 194, 45);
}

/*!
 * \brief 获取关键字是否加粗
 * \param set 字符集
 * \return 是否加粗
 */
bool SimAbstractLexer::isKeywordBold(int set) const
{
    Q_UNUSED(set);
    return true;
}

/*!
 * \brief 获取行尾是否填充.
 * \param style 风格
 * \return 是否填充
 * \sa setEolFill(int style)
 */
bool SimAbstractLexer::eolFill(int style) const
{
    return d->getStyleData(style).eolFill;
}

/*!
 * \brief 获取指定风格的字体.
 * \param style 风格
 * \return 字体
 * \sa setFont(int style)
 */
QFont SimAbstractLexer::font(int style) const
{
    return d->getStyleData(style).font;
}

/*!
 * \brief 设置风格的自动缩进.
 * \param autoindentstyle 自动缩进的风格
 * \sa getAutoIndentStyle()
 */
void SimAbstractLexer::setAutoIndentStyle(SimSci::AutoIndentStyle autoindentstyle)
{
    d->autoIndentStyle = autoindentstyle;
}

/*!
 * \brief 设置指定风格的前景色.
 * \param style 风格
 * \param c 颜色
 * \sa getColor(int style)
 */
void SimAbstractLexer::setColor(int style, const QColor &c)
{
    d->setColor(style, c);
}

/*!
 * \brief 设置指定风格行尾是否填充.
 * \param style 风格
 * \param eoffill 是否填充
 * \sa isEolFill(int style)
 */
void SimAbstractLexer::setEolFill(int style, bool eolfill)
{
    d->setEolFill(style, eolfill);
}

/*!
 * \brief 设置指定风格的字体.
 * \param style 风格
 * \param f 字体
 * \sa getFont(int style)
 */
void SimAbstractLexer::setFont(int style, const QFont &f)
{
    d->setFont(style, f);
}

/*!
 * \brief 设置指定风格的背景色.
 * \param style 风格
 * \param c 颜色
 * \sa getBackgroundColor(int style)
 */
void SimAbstractLexer::setBackgroundColor(int style, const QColor &c)
{
    d->setBackgroundColor(style, c);
}

/*!
 * \brief 从QSettings中读取lexer的属性值.
 * \param qs
 * \param prefix 前缀
 * \sa writeProperties()
 */
void SimAbstractLexer::readProperties(QSettings &qs, const QString &prefix)
{
    Q_UNUSED(qs);
    Q_UNUSED(prefix);
}

/*!
 * \brief SimAbstractLexer::writeProperties
 * \param qs
 * \param prefix 前缀
 * \sa readProperties(QSettings& qs, const QString& prefix)
 */
void SimAbstractLexer::writeProperties(QSettings &qs, const QString &prefix)
{
    Q_UNUSED(qs);
    Q_UNUSED(prefix);
}
