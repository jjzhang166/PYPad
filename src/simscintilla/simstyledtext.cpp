#include "simscintilla.h"

#include "simstyledtext.h"
#include "simabstracttextstyle.h"
#include "simscintilladefs.h"

struct SimStyledTextPrivate
{
    SimStyledTextPrivate(const QString& text_, int style_):
                         text(text_), style(style_), fancyStyle(nullptr) {}
    SimStyledTextPrivate(const QString& text_, const SimAbstractTextStyle& fancyStyle_): text(text_), style(-1) {
        fancyStyle = new SimAbstractTextStyle(style);
        fancyStyle->initFrom(fancyStyle_);
    }

    QString text;
    int style;
    SimAbstractTextStyle* fancyStyle;
};

SimStyledText::SimStyledText(const QString& text, int style)
{
    d = new SimStyledTextPrivate(text, style);
}

SimStyledText::SimStyledText(const QString& text, const SimAbstractTextStyle& fancyStyle)
{
    d = new SimStyledTextPrivate(text, fancyStyle);
}

SimStyledText::~SimStyledText()
{
    delete d;
}

const QString& SimStyledText::text() const
{
    return d->text;
}

int SimStyledText::style() const
{
    return d->style;
}

void SimStyledText::apply(SimScintilla *sci) const
{
    if (d->fancyStyle != nullptr) {
        d->fancyStyle->apply(sci);
    }
}
