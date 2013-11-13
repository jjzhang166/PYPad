#ifndef SIMABSTRACTLEXER_H
#define SIMABSTRACTLEXER_H

#include "simscintilladefs.h"
#include "simscintilla_global.h"

#include <QObject>
#include <QColor>
#include <QFont>
#include <QSettings>

class SimScintilla;
class SimAbstractAPIs;
class SimAbstractLexerPrivate;

class SIMSCINTILLA_EXPORT SimAbstractLexer : public QObject
{
    Q_OBJECT
public:
    explicit SimAbstractLexer(SimScintilla *parent = 0);
    virtual ~SimAbstractLexer();

    SimAbstractAPIs *apis() const;
    SimSci::AutoIndentStyle autoIndentStyle() const;

    void setDefaultColor(const QColor& col);
    QColor defaultColor() const;

    void setDefaultFont(const QFont& font);
    QFont defaultFont() const;

    void setDefaultBackgroundColor(const QColor& col);
    QColor defaultBackgroundColor() const;

    virtual void setEditor(SimScintilla* editor);
    SimScintilla* editor() const;

    virtual int lang() const { return SCLEX_CONTAINER; }
    virtual QString lexerStr() const = 0;
    virtual QString description(int style) const = 0;

    virtual int braceStyle() const;
    virtual QString autoCompletionFillups() const;
    virtual QStringList autoCompletionWordSeparators() const;

    virtual bool isCaseSensitive() const;
    virtual QString blockStart(int* style = nullptr) const;
    virtual QString blockEnd(int* style = nullptr) const;

    virtual int blockLookback() const;
    virtual QString blockStartKeyword(int* style = nullptr) const;

    virtual int defaultStyle() const;

    virtual QColor defaultBackgroundColor(int style) const;
    virtual QColor defaultColor(int style) const;
    virtual QColor backgroundColor() const;
    virtual QColor backgroundColor(int style) const;
    virtual QColor color(int style) const;
    virtual QColor keywordColor(int set) const;
    virtual bool isKeywordBold(int set) const;
    virtual QFont defaultFont(int style) const;
    virtual QFont font(int style) const;
    virtual bool defaultEolFill(int style) const;
    virtual bool eolFill(int style) const;
    virtual QString keywords(int set) const;
    virtual QString wordCharacters() const;
    virtual int indentationGuideView() const;

    virtual void refreshProperties();
    virtual int styleBitsNeeded() const;


public slots:

    virtual void setAutoIndentStyle(SimSci::AutoIndentStyle autoindentstyle);

    virtual void setColor(int style, const QColor &c);

    virtual void setEolFill(int style, bool eolfill);

    virtual void setFont(int style, const QFont &f);

    virtual void setBackgroundColor(int style, const QColor &c);

signals:
    /*!
     * \brief 当指定风格的前景色改变的时候会发射此信号
     * \param style 风格
     * \param c color
     */
    void colorChanged(int style, const QColor &c);

    /*!
     * \brief 当指定风格的行尾填充改变时发射此信号
     * \param style 风格
     * \param eolfilled 是否填充
     */
    void eolFillChanged(int style, bool eolfilled);

    /*!
     * \brief 当指定风格的字体改变时发射此信号
     * \param style 风格
     * \param f 字体
     */
    void fontChanged(int style, const QFont &f);

    /*!
     * \brief 当指定风格的背景色改变时发射此信号
     * \param style 风格
     * \param c 颜色
     */
    void backgroundColorChanged(int style, const QColor &c);

    /*!
     * \brief 在属性值变化时发射此信号
     * \param property 属性
     * \param value 值
     */
    void propertyChanged(const QString& property, const QString& value);

protected:
    virtual void readProperties(QSettings &qs,const QString &prefix);
    virtual void writeProperties(QSettings &qs,const QString &prefix);

private:
    SimAbstractLexerPrivate* d;
    Q_DISABLE_COPY(SimAbstractLexer)
};

#endif // SIMABSTRACTLEXER_H
