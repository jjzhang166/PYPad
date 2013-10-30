#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QObject>
#include <QColor>
#include <QTime>
#define COLORPICKER_INCREMENT 5
#define COLORPICKER_INIT_INDEX 3
class ColorPicker
{
public:
    ColorPicker();
    QColor GetColor();
    void Reset();
    int m_nIndex;
    QList<QColor> m_clrList;
};

#endif // COLORPICKER_H
