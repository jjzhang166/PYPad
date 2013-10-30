#include "colorpicker.h"

ColorPicker::ColorPicker()
{
    m_nIndex=COLORPICKER_INIT_INDEX;
//    m_clrList << QColor(79,134,69)
//              << QColor(137,58,63)
//              << QColor(97,166,155)
//              << QColor(136,46,30)
//              << QColor(144,38,80)
//              << QColor(44,93,10)
//              << QColor(105,111,125)
//              << QColor(143,123,23)
//              << QColor(24,84,133)
//              << QColor(62,135,166)
//              << QColor(38,109,85)
//              << QColor(72,32,68)
//              << QColor(65,150,78)
//              << QColor(132,1,14)
//              << QColor(98,139,21)
//              << QColor(137,98,118)
//              << QColor(175,105,47)
//              << QColor(111,82,19)
//              << QColor(84,92,34)
//              << QColor(192,139,70);

    m_clrList<< QColor(69,77,153)
            << QColor(132,1,14)
            << QColor(44,93,10)
            << QColor(175,105,47)
            << QColor(111,82,19)
            << QColor(84,92,34)
            << QColor(8,21,13)
            << QColor(54,62,49)
            << QColor(192,139,70)
            << QColor(105,111,125)
            << QColor(143,123,23)
            << QColor(24,84,133)
            << QColor(77,59,17)
            << QColor(29,37,42)
            << QColor(98,139,21)
            << QColor(137,98,118)
            << QColor(180,154,113)
             << QColor(79,134,69)
             << QColor(137,58,63)
             << QColor(97,166,155)
             << QColor(136,46,30)
             << QColor(114,38,80)
             << QColor(80,31,6)
             << QColor(62,135,166)
             << QColor(38,109,85)
             << QColor(9,22,51)
             << QColor(72,32,68)
             << QColor(62,150,78);

}
QColor ColorPicker::GetColor()
{
    ++m_nIndex;
    return QColor(m_clrList.at( (m_nIndex+COLORPICKER_INCREMENT) % m_clrList.count()));
}

void ColorPicker::Reset()
{
    m_nIndex=COLORPICKER_INIT_INDEX;
}
