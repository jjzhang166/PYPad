#include "pypad.h"
#include <QToolTip>

#define PYP_OPER_SYMBOL +

QColor LighterColor(const QColor& clr)
{
     int Red = clr.red();
     int Green = clr.green();
     int Blue = clr.blue();

     Red = (Red PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) > 255 ? 255 :(Red PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) ;
     Green = (Green PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) > 255 ? 255 :(Green PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) ;
     Blue = (Blue PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) > 255 ? 255 :(Blue PYP_OPER_SYMBOL PYP_COLOR_DECREMENT) ;

     return QColor(Red,Green,Blue);


}

PYPad::PYPad(QWidget *parent) :
    PYFadeWidget(parent)
{
    initVariables();
    initWidgets();
    initSettings();
    initMessages();
}

/*!
  private utility functions
*/
void PYPad::initVariables()
{
    m_Height =sizeHint().height();
    m_StaticHeight = sizeHint().height();
    m_Width = sizeHint().width();
    m_BallRadius = 0;
    m_nBallIndex = 0;
    m_nCenterX = 0;
    m_nCurrentBallIndex = -1;

    m_bPressed = false;
    m_bShowBall = true;
    m_bStickToFather = true;

    m_FadeTimer =new QTimer(this);
    m_FadeTimer->setInterval(PYP_TIMER_INTERVAL);
    connect(m_FadeTimer,SIGNAL(timeout()),this,SLOT(DoFade()));

    m_ShrinkTimer = new QTimer(this);
    m_ShrinkTimer->setInterval(PYP_SHRINK_INTERVAL);
    connect(m_ShrinkTimer,SIGNAL(timeout()),this,SLOT(DoShrink()));
}

void PYPad::initWidgets()
{
    AddBtn = CreateButton(tr("Add Ball"),QIcon(":/images/add.png"),tr("add a pad"));
    DeleteBtn = CreateButton(tr("Delete Ball"),QIcon(":/images/delete.png"),tr("delet a ball"));
    ConfigBtn = CreateButton(tr("Config PYPad"),QIcon(":/images/config.png"),tr("config PYPad"));
    ExitBtn = CreateButton(tr("Exit PYPad"),QIcon(":/images/exit.png"),tr("exit PYPad"));
    RestoreBtn = CreateButton(tr("Goto Top Right"),QIcon(":/images/restore.png"),tr("restore"));
    ShowBtn = CreateButton(tr("Show/Hide Balls"),QIcon(":/images/show.png"),tr("show/hide balls"));

    ConfigWindow = new PYPadConfig(this);
    ConfigWindow->hide();
}

void PYPad::initSettings()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
}

void PYPad::initMessages()
{
    connect(AddBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(DeleteBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(ConfigBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(RestoreBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(ExitBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(ShowBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));

    //
    connect(ConfigWindow,SIGNAL(sig_SetStickToFather(bool)),this,SLOT(SetStickToFather(bool)));
    connect(ConfigWindow,SIGNAL(sig_SetPadSize(QSize)),this,SLOT(SetPadSize(QSize)));


}

void PYPad::calcGeo()
{
    // RESIZE BUTTON'S SIZE AND THIS WIDTH
    AddBtn->setFixedSize(PYP_BTN_SIZE);
    DeleteBtn->setFixedSize(PYP_BTN_SIZE);
    ConfigBtn->setFixedSize(PYP_BTN_SIZE);
    ExitBtn->setFixedSize(PYP_BTN_SIZE);
    RestoreBtn->setFixedSize(PYP_BTN_SIZE);
    ShowBtn->setFixedSize(PYP_BTN_SIZE);

    int TotalWidth = PYP_CTRL_BTN_COUNT * PYP_BTN_WIDTH + (PYP_CTRL_BTN_COUNT-1) * PYP_BTN_SPACE/* + 2 * PYP_LEFT_SPACE*/;
    setFixedWidth(TotalWidth);
    m_Width = TotalWidth;

    if(m_BallInfoVec.count() > PYP_BALL_COUNT_LIMIT)
    {
        m_BallRadius = TotalWidth/5;
    }else if (m_BallInfoVec.count() > PYP_BALL_COUNT_LIMIT * 2){
        m_BallRadius = TotalWidth/9;
    }else if(m_BallInfoVec.count() > PYP_BALL_COUNT_LIMIT * 4){
        m_BallRadius = TotalWidth/15;
    }else{
        m_BallRadius = TotalWidth/3;
    }
    m_nCenterX = TotalWidth/2;

    /// REPOSITION BUTTONS
    // REPOSITION ADDBTN
    int initX = PYP_LEFT_SPACE;
    int initY = PYP_HEADER_HEIGHT + PYP_BTN_SPACE;

    QPointF AddBtnMovePot(initX,initY);
    AddBtn->move(AddBtnMovePot.toPoint());
    initX +=PYP_BTN_SPACE + PYP_BTN_WIDTH;

    QPointF DeleteBtnMovePot(initX,initY);
    DeleteBtn->move(DeleteBtnMovePot.toPoint());
    initX +=PYP_BTN_SPACE + PYP_BTN_WIDTH;

    QPointF ConfigBtnMovePot(initX,initY);
    ConfigBtn->move(ConfigBtnMovePot.toPoint());
    initX +=PYP_BTN_SPACE + PYP_BTN_WIDTH;

    QPointF ShowBtnMovePot(initX,initY);
    ShowBtn->move(ShowBtnMovePot.toPoint());
    initX +=PYP_BTN_SPACE + PYP_BTN_WIDTH;

    QPointF RestoreBtnMovePot(initX,initY);
    RestoreBtn->move(RestoreBtnMovePot.toPoint());
    initX +=PYP_BTN_SPACE + PYP_BTN_WIDTH;


    QPointF ExitBtnMovePot(initX,initY);
    ExitBtn->move(ExitBtnMovePot.toPoint());

}

void PYPad::gotoTopRight()
{
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry();
    QPoint ThisMovePot(desktopRect.width() - m_Width - PYP_LEFT_SPACE,PYP_LEFT_SPACE);
    move(ThisMovePot);
}

void PYPad::repositionAllPads()
{
    int BallCnt = m_BallInfoVec.count();

    for(int BallIndex = 0; BallIndex < BallCnt ;BallIndex)
    {
         PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
         QPoint gTopLeft = mapToGlobal(rect().topLeft());
         int TopX = gTopLeft.x();
         int PrivateWidth = pPrivate->width();
         QPoint PrivateMovePot(TopX - PrivateWidth + pPrivate->GetAngleWidth() -PYP_WIDGET_SPACE,gTopLeft.y());
         pPrivate->move(PrivateMovePot);
         pPrivate->show();
     }
}

void PYPad::resetPrivate()
{
    int BallCnt = m_BallInfoVec.count();
    for(int BallIndex = 0; BallIndex < BallCnt ;BallIndex++)
    {
        BallInfo* pBI = m_BallInfoVec.at(BallIndex);
        PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
        pPrivate->SetY(pBI->GetRect().center().y());
        pPrivate->SetAngleWidth(m_nCenterX - m_BallRadius);

    }
}
bool PYPad::hasUnsavedDoc()
{
    int padCount = m_BallInfoVec.count();
    for(int padIndex = 0;padIndex < padCount ; padIndex++)
    {
        if(!m_BallInfoVec.at(padIndex)->GetPrivate()->IsSaved())
        {
            return true;
        }
    }
    return false;
}

QToolButton* PYPad::CreateButton(const QString &strText, const QIcon &icon, const QString &strTip)
{
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setText(strText);
    toolButton->setIcon(icon);
    toolButton->setIconSize(PYP_BTN_SIZE);
    toolButton->setToolTip(strTip);
    toolButton->setAutoRaise(true);
    toolButton->setAutoRepeat(true);


    return toolButton;
}

/*!
  reimplementing protected functions
*/
void PYPad::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if(PYP_USE_ANTIALIASING)
    {
        painter.setRenderHints(QPainter::Antialiasing);
    }
    drawHeader(&painter);
    drawBalls(&painter);

}

void PYPad::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        m_bPressed = true;
        m_lastPot = e->pos();
    }

    PYP_HIGHLIGHT_BALL;

    for(int BallIndex = 0;BallIndex < BallCnt ;BallIndex++)
    {
         if(m_BallInfoVec.at(BallIndex)->GetRect().contains(e->pos()))
         {
             PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
             int Y = m_BallInfoVec.at(BallIndex)->GetRect().center().y();
             int AngleWidth = m_nCenterX - m_BallRadius;
             pPrivate->SetY(Y);
             pPrivate->SetAngleWidth(AngleWidth);
             pPrivate->SetBgColor(m_BallInfoVec.at(BallIndex)->GetColor());

             if(m_bStickToFather)
             {
                REPOSITION_PRIVATE;
            }else{
                pPrivate->show();
            }
         }else{
             PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
             pPrivate->Hide();
         }
     }




}

void PYPad::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton && m_bPressed)
    {
        QPoint MovePot = e->globalPos() - m_lastPot;
        move(MovePot);
    }
    PYP_HIGHLIGHT_BALL;

    //
    int TipCnt = m_RectTitleVec.count();
    for(int BallIndex = 0;BallIndex < TipCnt ; BallIndex++)
    {
        if(m_RectTitleVec.at(BallIndex).first.contains(e->pos()))
        {
            QToolTip::showText(e->globalPos(),m_RectTitleVec.at(BallIndex).second);
            break;
        }
    }

    //
    if(m_IndicatorPath.contains(e->pos()))
    {
        QToolTip::showText(e->globalPos(),tr("Double-click to remove this ball !"));
    }

}

void PYPad::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        m_bPressed = false;
    }
}

void PYPad::mouseDoubleClickEvent(QMouseEvent *e)
{
    // remove selected ball
    if(m_IndicatorPath.contains(e->pos()))
    {
        DEBUG_VAR(m_nCurrentBallIndex);
        m_BallInfoVec.at(m_nCurrentBallIndex)->GetPrivate()->close();
        m_BallInfoVec.remove(m_nCurrentBallIndex);
        resetPrivate();
        update();
    }

}

void PYPad::resizeEvent(QResizeEvent *)
{
    calcGeo();
}

void PYPad::showEvent(QShowEvent *e)
{
    calcGeo();
    gotoTopRight();

}

void PYPad::closeEvent(QCloseEvent *e)
{
    int BallCnt = m_BallInfoVec.count();

    for(int BallIndex = 0;BallIndex < BallCnt ; BallIndex++)
    {
        m_BallInfoVec.at(BallIndex)->Destroy();
    }

    e->accept();
}

void PYPad::moveEvent(QMoveEvent *e)
{
//    repositionAllPads();
    QPoint TopLeft = mapToGlobal(rect().topLeft());
    emit sig_SetMovePoint(TopLeft);

    if(m_bStickToFather)
    {
        int BallCnt = m_BallInfoVec.count();
        for(int BallIndex = 0; BallIndex < BallCnt ;BallIndex++)
        {
            PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
            pPrivate->SetTopLeft(TopLeft);
        }
    }
    QWidget::moveEvent(e);
}

/*!
  painting functions
*/
void PYPad::drawHeader(QPainter *painter)
{
    painter->save();


    QPointF HeaderTopLeft(0,0);
    QPointF HeaderBottomRight(width(),PYP_HEADER_HEIGHT+PYP_CTRL_PANEL_HEIGHT);
    QRectF HeaderRect(HeaderTopLeft,HeaderBottomRight);
    painter->setPen(Qt::NoPen);
    painter->setBrush(LinearGradient::GetThirdGradient(HeaderRect.topLeft(),HeaderRect.bottomLeft(),PYP_HEADER_COLOR));
    painter->drawRoundedRect(HeaderRect,PYP_HEADER_RECT_RADIUS,PYP_HEADER_RECT_RADIUS);

    m_HeaderRect = HeaderRect;

    //
    QPointF HeaderTextTopLeft(0,0);
    QPointF HeaderTextBottomRight(width(),PYP_HEADER_HEIGHT);
    QRectF HeaderTextRect(HeaderTextTopLeft,HeaderTextBottomRight);
    painter->setPen(Qt::white);
    QFont TextFont;
    TextFont.setBold(true);
    painter->setFont(TextFont);
    painter->drawText(HeaderTextRect,tr("PYPad"),Qt::AlignVCenter|Qt::AlignHCenter);

    painter->restore();
}

void PYPad::drawBalls(QPainter *painter)
{
    if(m_BallInfoVec.count() == 0 || !m_bShowBall)
    {
        return ;
    }

    m_RectTitleVec.clear();
    painter->save();

    qreal initY = PYP_HEADER_HEIGHT + PYP_CTRL_PANEL_HEIGHT ;

    int BallCnt = m_BallInfoVec.count();

    for(int BallIndex = 0; BallIndex < BallCnt ;BallIndex++)
    {
            painter->save();
            // draw line
            painter->save();
            QPointF topPot(m_nCenterX,initY);
            QPointF bottomPot(m_nCenterX,initY + PYP_BALL_SPACE);
            painter->setPen(QPen(m_BallInfoVec.at(BallIndex)->GetColor(),PYP_BALL_PEN_WIDTH));
            painter->drawLine(topPot,bottomPot);
            painter->restore();

            // draw ball
            QPointF BallTopLeft(0,initY + PYP_BALL_SPACE);
            QPointF BallBottomRight(width(),initY + PYP_BALL_SPACE + m_BallRadius * 2);
            QRectF BallRect(BallTopLeft,BallBottomRight);
            m_BallInfoVec[BallIndex]->SetRect(BallRect);
            painter->setOpacity(m_BallInfoVec.at(BallIndex)->GetOpacity());
            painter->setPen(Qt::NoPen);

            if(!m_BallInfoVec.at(BallIndex)->IsPressed())
            {
                painter->setBrush(m_RG.GetRadialGradient(BallRect.center(),m_BallRadius,m_BallInfoVec.at(BallIndex)->GetColor()));

            }else{
//                QLinearGradient BallGradient(BallRect.topLeft(),BallRect.bottomLeft());
//                BallGradient.setColorAt(0.0,m_BallInfoVec.at(BallIndex)->GetColor().lighter());
//                BallGradient.setColorAt(1.0,m_BallInfoVec.at(BallIndex)->GetColor());
//                painter->setBrush(BallGradient);
                painter->setBrush(m_RG.GetRadialGradient(BallRect.center(),m_BallRadius,LighterColor(m_BallInfoVec.at(BallIndex)->GetColor())));

                // draw pressed indicator
                qreal IndicatorWidth = m_BallRadius /2;
                qreal IndicatorY = m_BallInfoVec.at(BallIndex)->GetRect().center().y();
                qreal IndicatorX = m_nCenterX + m_BallRadius + PYP_BTN_SPACE;

                QPainterPath IndicatorPath;
                QVector<QPointF> PotVec;
                QPointF LeftPot(IndicatorX,IndicatorY);
                QPointF TopPot(IndicatorX + IndicatorWidth,IndicatorY - IndicatorWidth/2);
                QPointF BottomPot(IndicatorX + IndicatorWidth,IndicatorY + IndicatorWidth/2);
                PotVec.push_back(TopPot);
                PotVec.push_back(LeftPot);
                PotVec.push_back(BottomPot);
                painter->save();
                painter->setOpacity(0.8);
                painter->setPen(Qt::black);
                painter->setBrush(Qt::black);
                painter->drawPolygon(PotVec);
                painter->restore();
                IndicatorPath.addPolygon(QPolygonF(PotVec));
                m_IndicatorPath = IndicatorPath;

            }

            m_RectTitleVec.push_back(qMakePair(BallRect,m_BallInfoVec.at(BallIndex)->GetPrivate()->GetTitle()));
            painter->drawEllipse(BallRect.center(),m_BallRadius,m_BallRadius);

            /// increment initY
            initY += PYP_BALL_SPACE + 2 * m_BallRadius ;
            painter->restore();

    }

    m_Height = initY ;
    setFixedHeight(m_Height);
    painter->restore();
}


/*!
  private slot functions
*/
void PYPad::BtnHandler()
{
    QToolButton* who = qobject_cast<QToolButton*>(sender());


    if(who == ExitBtn)
    {
        if(hasUnsavedDoc())
        {
            QMessageBox::warning(this,tr("Warning"),tr("You have unsaved document !"));
            return ;
        }
        close();
    }

    if(who == AddBtn)
    {
        AddBall();
    }

    if(who == DeleteBtn)
    {
        DeleteBall();
    }

    if(who == RestoreBtn)
    {
        gotoTopRight();
    }

    if(who == ShowBtn)
    {
        ShowBalls();
    }

    if(who == ConfigBtn)
    {
        QDesktopWidget desktop;
        QRect desktopRect = desktop.availableGeometry();
        QPoint ConfigWindowMovePot(desktopRect.center().x() - ConfigWindow->width()/2,desktopRect.center().y());
        ConfigWindow->move(ConfigWindowMovePot);
        ConfigWindow->show();
    }
}

void PYPad::AddBall()
{
    BallInfo* newBI  =new BallInfo;
    newBI->SetColor(m_CP.GetColor());
    newBI->SetIndex(m_nBallIndex++);
    PYPadPrivate* newPYPP = new PYPadPrivate(0);
    newBI->SetPrivate(newPYPP);

    m_BallInfoVec.push_back(newBI);

    if(m_FadeTimer->isActive())
    {
        m_FadeTimer->stop();
    }
    m_FadeTimer->start();
    update();

    GET_STATIC_HEIGHT;
    resetPrivate();



}

void PYPad::DeleteBall()
{
    if(m_BallInfoVec.count() != 0)
    {
        BallInfo* pBI = m_BallInfoVec[m_BallInfoVec.count() - 1];
        pBI->GetPrivate()->close();
        delete pBI;
        pBI = 0;
        m_BallInfoVec.pop_back();
        update();
    }
    GET_STATIC_HEIGHT;
    resetPrivate();

}

void PYPad::DoFade()
{
    int BallCnt = m_BallInfoVec.count();
    bool bAllOpaque = true;

    for(int BallIndex = 0;BallIndex < BallCnt ;BallIndex++)
    {
        if(m_BallInfoVec.at(BallIndex)->GetOpacity() >= 1.0)
        {
            continue;
        }else{
            qreal opacity = m_BallInfoVec.at(BallIndex)->GetOpacity();
            DEBUG_VAR(opacity);
            opacity += PYP_OPACITY_INCREMENT;
            DEBUG_VAR(opacity);
            m_BallInfoVec[BallIndex]->SetOpacity(opacity);
            bAllOpaque  = false;
        }
    }

    if(bAllOpaque)
    {
        m_FadeTimer->stop();
        DEBUG_VAR(m_FadeTimer->isActive());
    }

    update();
}

void PYPad::DoShrink()
{
    DEBUG_VAR(m_Height);
    DEBUG_VAR(m_StaticHeight);

    if(m_bShowBall)
    {
        m_Height += PYP_SHRINK_INCREMENT;

        if(m_Height >= m_StaticHeight)
        {
            m_ShrinkTimer->stop();
        }
    }else{
        m_Height -= PYP_SHRINK_INCREMENT;

        if(m_Height <=  PYP_TOTAL_HEADER_HEIGHT)
        {
            m_ShrinkTimer->stop();
        }
    }

    setFixedHeight(m_Height);

//    update();
}

void PYPad::ShowBalls()
{
    m_bShowBall = !m_bShowBall;

    int BallCnt = m_BallInfoVec.count();

    for(int BallIndex = 0;BallIndex < BallCnt ; BallIndex++)
    {
        if(m_bShowBall)
        {

        }else{
            m_BallInfoVec.at(BallIndex)->GetPrivate()->Hide();
        }
    }

    update();

//    if(m_ShrinkTimer->isActive())
//    {
//        m_ShrinkTimer->stop();
//    }
//    m_ShrinkTimer->start();
}

void PYPad::SetStickToFather(bool stick)
{
    m_bStickToFather = stick;
}

void PYPad::SetPadSize(const QSize &size)
{
    int BallCnt = m_BallInfoVec.count();
    for(int BallIndex = 0;BallIndex < BallCnt ;BallIndex++)
    {
        PYPadPrivate* pPrivate = m_BallInfoVec.at(BallIndex)->GetPrivate();
        pPrivate->Resize(size.width(),size.height());
    }
}
