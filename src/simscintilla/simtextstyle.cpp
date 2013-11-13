#include "simtextstyle.h"

SimTextStyle::SimTextStyle(int style): SimAbstractTextStyle(style)
{

}

SimTextStyle::SimTextStyle(int style, const QString &description, const QColor &color,
                           const QColor &bgColor, const QFont &font, bool eolFill):
                           SimAbstractTextStyle(style, description, color, bgColor, font, eolFill)
{

}

SimTextStyle::~SimTextStyle()
{

}
