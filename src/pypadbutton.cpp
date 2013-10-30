#include "pypadbutton.h"
extern QColor LighterColor(const QColor& clr);

PYPadButton::PYPadButton(QWidget *parent) :
    QWidget(parent)
{
    initVariables();
}

/*!
  private utility functions
*/
void PYPadButton::initVariables()
{
    m_bPressed = false;

//    setAttribute(Qt::WA_TranslucentBackground);
//    setWindowFlags(Qt::FramelessWindowHint);
}

/*!
  reimplementing protected functions
*/
void PYPadButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawButton(&painter);
}

void PYPadButton::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        m_bPressed = true;
        update();
        emit sig_clicked();
    }
}

void PYPadButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_bPressed = false;
    emit sig_released();
    update();
}

/*!
  painting functions
*/
void PYPadButton::drawButton(QPainter *painter)
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);
    painter->setPen(Qt::NoPen);

    QLinearGradient BgGradient(QPoint(0,0),QPoint(0,height()));

    if(m_bPressed)
    {
        BgGradient.setColorAt(0.0,m_LG.GetStartColor(PYP_HEADER_COLOR));
        BgGradient.setColorAt(0.5,m_LG.GetEndColor(PYP_HEADER_COLOR));
//        BgGradient.setColorAt(0.8,m_LG.GetEndColor(PYP_HEADER_COLOR));
        BgGradient.setColorAt(1.0,m_LG.GetStartColor(PYP_HEADER_COLOR));
    }else{
        BgGradient.setColorAt(0.0,LighterColor(m_LG.GetStartColor(PYP_HEADER_COLOR)));
//        BgGradient.setColorAt(0.2,LighterColor(m_LG.GetEndColor(PYP_HEADER_COLOR)));
        BgGradient.setColorAt(0.5,LighterColor(m_LG.GetEndColor(PYP_HEADER_COLOR)));
        BgGradient.setColorAt(1.0,LighterColor(m_LG.GetStartColor(PYP_HEADER_COLOR)));
    }
    painter->setBrush(BgGradient);
    painter->drawRoundedRect(rect(),PYPB_RECT_RADIUS,PYPB_RECT_RADIUS);

    painter->save();
    QFont TextFont;
    TextFont.setBold(true);
    painter->setFont(TextFont);

    if(!m_bPressed)
    {
        painter->setPen(Qt::black);
    }else{
        painter->setPen(Qt::white);
    }

    painter->drawText(rect(),m_strText,Qt::AlignVCenter|Qt::AlignHCenter);
    painter->restore();

    painter->restore();
}



/*!
  public interfaces
*/
void PYPadButton::SetText(const QString &str)
{
    m_strText =str;
    update();
}
