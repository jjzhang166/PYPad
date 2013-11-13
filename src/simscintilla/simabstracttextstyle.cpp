#include "simabstracttextstyle.h"
#include "simscintilla.h"

#include <QColor>
#include <QFont>
#include <QApplication>


struct SimAbstractTextStylePrivate
{
    explicit SimAbstractTextStylePrivate(int style_): style(style_), eolFill(false), visible(true),
                                                      changeable(true), textCase(SimSci::TC_Original)
    {
        initDefaults(style_);
    }
    explicit SimAbstractTextStylePrivate(int style_, const QString &description_, const QColor &color_,
                                         const QColor &bgColor_, const QFont &font_, bool eolFill_): style(style_),
                                         description(description_), color(color_), bgColor(bgColor_),
                                         font(font_), eolFill(eolFill_), textCase(SimSci::TC_Original)
    {
        initDefaults(style_);
    }

    void init(const SimAbstractTextStylePrivate& other)
    {
        style = other.style;
        description = other.description;
        color = other.color;
        bgColor = other.bgColor;
        font = other.font;
        eolFill = other.eolFill;
        visible = other.visible;
        changeable = other.changeable;
        hotspot = other.hotspot;
        textCase = other.textCase;
    }

    void initDefaults(int style_)
    {
        // The next style number to allocate.  The initial values corresponds to
        // the amount of space that Scintilla initially creates for styles.
        static int next_style_nr = 63;

        // See if a new style should be allocated.  Note that we allow styles to be
        // passed in that are bigger than STYLE_MAX because the styles used for
        // annotations are allowed to be.
        if (style_ < 0) {
            // Note that we don't deal with the situation where the newly allocated
            // style number has already been used explicitly.
            if (next_style_nr > STYLE_LASTPREDEFINED)
                style = next_style_nr--;
        }

        QPalette pal = QApplication::palette();
        color = pal.text().color();
        bgColor = pal.base().color();

        font = QApplication::font();
    }

    int style;
    QString description;
    QColor color;
    QColor bgColor;
    QFont font;
    bool eolFill : 1;
    bool visible : 1;
    bool changeable : 1;
    bool hotspot : 1;
    SimSci::TextCase textCase;
};

SimAbstractTextStyle::SimAbstractTextStyle(int style)
{
    d = new SimAbstractTextStylePrivate(style);
}

SimAbstractTextStyle::SimAbstractTextStyle(int style, const QString &description, const QColor &color,
                                           const QColor &bgColor, const QFont &font, bool eolFill)
{
    d = new SimAbstractTextStylePrivate(style, description, color, bgColor, font, eolFill);
}

SimAbstractTextStyle::~SimAbstractTextStyle()
{
    SAFE_RELEASE(d)
}

extern sptr_t GetValueFromColor(const QColor& col);
void SimAbstractTextStyle::apply(const SimScintilla *sci) const
{
    SimScintilla* sci_ = const_cast<SimScintilla*>(sci);
    sci_->styleSetFore(d->style, GetValueFromColor(d->color));
    sci_->styleSetBack(d->style, GetValueFromColor(d->bgColor));
    sci_->styleSetSize(d->style, d->font.pointSize());
    sci_->styleSetWeight(d->style, -d->font.weight());
    sci_->styleSetItalic(d->style, d->font.italic());
    sci_->styleSetUnderline(d->style, d->font.underline());
    sci_->styleSetEOLFilled(d->style, d->eolFill);
    sci_->styleSetCase(d->style, static_cast<long>(d->textCase));
    sci_->styleSetVisible(d->style, d->visible);
    sci_->styleSetChangeable(d->style, d->changeable);
    sci_->styleSetHotSpot(d->style, d->hotspot);
    sci_->styleSetFont(d->style, d->font.family().toLatin1().data());
    sci_->styleSetBold(d->style, d->font.bold());
}

int SimAbstractTextStyle::style() const
{
    return d->style;
}

void SimAbstractTextStyle::initFrom(const SimAbstractTextStyle &other)
{
    d->init(*other.d);
}

void SimAbstractTextStyle::setDescription(const QString &description)
{
    d->description = description;
}

QString SimAbstractTextStyle::description() const
{
    return d->description;
}

void SimAbstractTextStyle::setColor(const QColor &color)
{
    d->color = color;
}

QColor SimAbstractTextStyle::color() const
{
    return d->color;
}

void SimAbstractTextStyle::setBackgroundColor(const QColor &bgColor)
{
    d->bgColor = bgColor;
}

QColor SimAbstractTextStyle::backgroundColor() const
{
    return d->bgColor;
}

void SimAbstractTextStyle::setFont(const QFont &font)
{
    d->font = font;
}

QFont SimAbstractTextStyle::font() const
{
    return d->font;
}

void SimAbstractTextStyle::setEolFill(bool fill)
{
    d->eolFill = fill;
}

bool SimAbstractTextStyle::isEolFill() const
{
    return d->eolFill;
}

void SimAbstractTextStyle::setTextCase(SimSci::TextCase textCase)
{
    d->textCase = textCase;
}

SimSci::TextCase SimAbstractTextStyle::textCase() const
{
    return d->textCase;
}

void SimAbstractTextStyle::setVisible(bool visible)
{
    d->visible = visible;
}

bool SimAbstractTextStyle::isVisible() const
{
    return d->visible;
}

void SimAbstractTextStyle::setChangeable(bool changeable)
{
    d->changeable = changeable;
}

bool SimAbstractTextStyle::isChangeable() const
{
    return d->changeable;
}

void SimAbstractTextStyle::setHotspot(bool hotspot)
{
    d->hotspot = hotspot;
}

bool SimAbstractTextStyle::isHotspot() const
{
    return d->hotspot;
}

