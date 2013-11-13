#ifndef SIMSTYLEDTEXT_H
#define SIMSTYLEDTEXT_H

#include "simscintilla_global.h"

class SimScintilla;
class SimAbstractTextStyle;

class SIMSCINTILLA_EXPORT SimStyledText
{
public:
    explicit SimStyledText(const QString& text, int style);
    explicit SimStyledText(const QString& text, const SimAbstractTextStyle& fancyStyle);
    ~SimStyledText();

    const QString& text() const;
    int style() const;

    void apply(SimScintilla* sci) const;

private:
    class SimStyledTextPrivate* d;

    SimStyledText(const SimStyledText& );
    SimStyledText& operator=(const SimStyledText& );
};

#endif // SIMSTYLEDTEXT_H
