#ifndef SIMTEXTSTYLE_H
#define SIMTEXTSTYLE_H

#include "simscintilla_global.h"
#include "simabstracttextstyle.h"

class SIMSCINTILLA_EXPORT SimTextStyle : public SimAbstractTextStyle
{
public:
    explicit SimTextStyle(int style = -1);
    explicit SimTextStyle(int style, const QString &description, const QColor &color,
    const QColor &bgColor, const QFont &font, bool eolFill = false);
    ~SimTextStyle();
};

#endif // SIMTEXTSTYLE_H
