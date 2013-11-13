#include "simscintillaex.h"
#include "tctextruler.h"

#ifdef USELOG
#include "logutil.h"
#endif

#include <QVBoxLayout>

SimScintillaEx::SimScintillaEx(QWidget *parent) :
    QWidget(parent)
{
    createUi();
}

SimScintilla* SimScintillaEx::scintilla() const
{
    return m_scintilla;
}

void SimScintillaEx::setRulerVisible(bool visible)
{
    m_ruler->setVisible(visible);
}

bool SimScintillaEx::isRulerVisible() const
{
    return m_ruler->isVisible();
}

void SimScintillaEx::createUi()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    m_scintilla = new SimScintilla(this);
    m_ruler = new TcTextRuler(this);
    layout->addWidget(m_ruler);
    layout->addWidget(m_scintilla);
    setLayout(layout);

    connect(m_scintilla, SIGNAL(zoom(int)), this, SLOT(onZoomed(int)));
    connect(m_scintilla, SIGNAL(marginWidthChanged(int)), this, SLOT(onMarginWidthChanged(int)));
    connect(m_scintilla, SIGNAL(positionChanged(int,int)), this, SLOT(onPositionChanged(int,int)));
    connect(m_scintilla, SIGNAL(textWidthChanged(qreal)), this, SLOT(onTextWidthChanged(qreal)));
    connect(m_scintilla, SIGNAL(hScrollBarValueChanged(int)), this, SLOT(onHScrollBarChanged(int)));
}

void SimScintillaEx::onZoomed(int zf)
{
    Q_UNUSED(zf);
}

void SimScintillaEx::onPositionChanged(int line, int index)
{
    Q_UNUSED(line);
    m_ruler->setColumn(index);
}

void SimScintillaEx::onTextWidthChanged(qreal textWidth)
{
    LOG_VAR(textWidth);
    m_ruler->setTextWidth(textWidth);
}

void SimScintillaEx::onMarginWidthChanged(int width)
{
    m_ruler->setSpacing(width);
}

void SimScintillaEx::onHScrollBarChanged(int value)
{
    LOG_VAR(value);
    m_ruler->setStartPosition(value);
}
