#include "pyfadewidget.h"

PYFadeWidget::PYFadeWidget(QWidget *parent) :
    QWidget(parent)
{
    initVariables();
}

/*!
  private utility functions
*/
void PYFadeWidget::initVariables()
{
    m_direction = Horizontal;
    m_dPixel = width()/PYF_INCREMENT_FACTOR;
    m_opacity = 0.0;
    m_bShow = true;
    m_timer = new QTimer(this);
    m_timer->setInterval(PYF_TIMER_INTERVAL);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slot_Update()));
}

/*!
  protected functions
*/
void PYFadeWidget::paintEvent(QPaintEvent *)
{

}

void PYFadeWidget::showEvent(QShowEvent *)
{
    m_bShow = true;
    setWindowOpacity(0);
    if(m_timer->isActive())
    {
        m_timer->stop();
    }
    m_timer->start();
}

void PYFadeWidget::closeEvent(QCloseEvent *e)
{
    m_bShow = false;
    if(m_timer->isActive())
    {
        m_timer->stop();
    }
    m_timer->start();
    e->ignore();
}

/*!
  private slot functions
*/
void PYFadeWidget::slot_Update()
{
    qreal dOpacity = 0;
    qreal intervalCnt = 0;
    if(m_direction  == Horizontal)
    {
        m_dPixel = width()/PYF_INCREMENT_FACTOR;
        intervalCnt = width()/m_dPixel;
    }else{
        m_dPixel = height()/PYF_INCREMENT_FACTOR;
        intervalCnt = height()/m_dPixel;
    }
    dOpacity = (qreal)1/intervalCnt;

    if(m_bShow)
    {
        m_opacity += dOpacity;

        if(m_opacity >= 1.0)
        {
            m_timer->stop();
        }
    }else{
        m_opacity -= dOpacity;

        if(m_opacity <= 0.0)
        {
            m_timer->stop();
        }
    }
    setWindowOpacity(m_opacity);
//    DEBUG_VAR(m_opacity);

}

/*!
  public interfaces
*/
void PYFadeWidget::setDirection(Direction d)
{
    m_direction = d;
}
