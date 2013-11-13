#ifndef SIMSCINTILLAEX_H
#define SIMSCINTILLAEX_H

#include "tctextruler.h"
#include "simscintilla.h"

#include <QWidget>

class SIMSCINTILLA_EXPORT SimScintillaEx : public QWidget
{
    Q_OBJECT
public:
    explicit SimScintillaEx(QWidget *parent = 0);

    SimScintilla* scintilla() const;

    void setRulerVisible(bool visible);
    bool isRulerVisible() const;

private Q_SLOTS:
    void onZoomed(int zf);
    void onPositionChanged(int line, int index);
    void onTextWidthChanged(qreal textWidth);
    void onMarginWidthChanged(int width);
    void onHScrollBarChanged(int value);

private:
    void createUi();

    SimScintilla* m_scintilla;
    TcTextRuler* m_ruler;
};

#endif // SIMSCINTILLAEX_H
