#include "pypad.h"
#include <QToolBar>


PYPadPrivate::PYPadPrivate(QWidget *parent):PYFadeWidget(parent)
{
    initVariables();
    initWidgets();
    initSettings();
    initMessages();
    calcGeo();

}

/*!
  private utility functions
*/
void PYPadPrivate::initVariables()
{
    m_Width = sizeHint().width();
    m_Height = sizeHint().height();
    m_Y = 0;
    m_strTitle = tr("PYPad");

    m_AngleWidth = 0;
    m_BgClr = m_CP.GetColor();

    m_bShowEdit = false;
    m_bPin = false;
    m_bPressed = false;
    m_bShowBar = false;
    m_bSaved = true;
    m_bOneShotFlag = true;

}

void PYPadPrivate::initWidgets()
{
    EditBtn = CreateButton(tr("Edit"),QIcon(":/images/edit.png"),tr("edit title"));
    PinBtn = CreateButton(tr("Pin"),QIcon(":/images/unpin.png"),tr("Pin the window on the top"));
    ShowBarBtn = CreateButton(tr("Show/Hide Bar"),QIcon(":/images/show_bar.png"),tr("show bar"));
    CloseBtn = CreateButton(tr("Close"),QIcon(":/images/close.png"),tr("close pad"));

    OpenBtn = CreateButton(tr("Open"),QIcon(":/images/open.png"),tr("Open file"));
    SaveBtn = CreateButton(tr("Save"),QIcon(":/images/save.png"),tr("save file"));
    ClearBtn = CreateButton(tr("Clear"),QIcon(":/images/clear.png"),tr("Clear"));
    ZoomInBtn = CreateButton(tr("Zoom in"),QIcon(":/images/zoom_in.png"),tr("Zoom in"));
    ZoomOutBtn = CreateButton(tr("Zoom out"),QIcon(":/images/zoom_out.png"),tr("Zoom out"));
    RenameTitleEdit =new PYPadLineEdit(this);
    RenameTitleEdit->setText(tr("PYPad"));
    RenameTitleEdit->hide();

    StyleLabel = new QLabel(tr("Format:"),this);
    StyleCombo = new QComboBox(this);
    StyleCombo->addItems(QStringList() << "cpp" << "php" << "html");

    ToolBar = new QToolBar(this);
    ToolBar->setAttribute(Qt::WA_TranslucentBackground);
    ToolBar->hide();

    Edit =new CodeDocument(this);
    rules = new Highlighter(QString("cpp"),Edit->document());

    ToolBar->addWidget(OpenBtn);
    ToolBar->addWidget(SaveBtn);
    ToolBar->addWidget(ClearBtn);
    ToolBar->addWidget(ZoomInBtn);
    ToolBar->addWidget(ZoomOutBtn);
    ToolBar->addWidget(StyleLabel);
    ToolBar->addWidget(StyleCombo);
}

void PYPadPrivate::initSettings()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

}

void PYPadPrivate::initMessages()
{
    // setup BtnHandler's signal-slot connections
    connect(EditBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(PinBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(ShowBarBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
    connect(CloseBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));

    // setup BarBtnHandler's signal-slot connections
    connect(OpenBtn,SIGNAL(clicked()),this,SLOT(BarBtnHandler()));
    connect(SaveBtn,SIGNAL(clicked()),this,SLOT(BarBtnHandler()));
    connect(ClearBtn,SIGNAL(clicked()),this,SLOT(BarBtnHandler()));
    connect(ZoomInBtn,SIGNAL(clicked()),this,SLOT(BarBtnHandler()));
    connect(ZoomOutBtn,SIGNAL(clicked()),this,SLOT(BarBtnHandler()));

    //
    connect(RenameTitleEdit,SIGNAL(sig_SetText(QString)),this,SLOT(SetTitle(QString)));

    // setup edit and this 's signals-slot connections
    connect(Edit,SIGNAL(sig_openFile()),this,SLOT(OpenFile()));
    connect(Edit,SIGNAL(sig_saveFile()),this,SLOT(SaveFile()));
    connect(Edit,SIGNAL(textChanged()),this,SLOT(TextChanged()));

    connect(StyleCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(StyleChanged(QString)));
}

void PYPadPrivate::calcGeo()
{
    qreal initY = PYPP_WIDGET_SPACE;
    qreal staticInitY = PYPP_WIDGET_SPACE;
    qreal initX = PYPP_WIDGET_SPACE ;
    int gEditWidth = m_Width - PYPP_WIDGET_SPACE * 2 - PYPP_CONFIG_BUTTON_COUNT * PYP_BTN_SPACE - PYPP_CONFIG_BUTTON_COUNT * PYP_BTN_WIDTH;

    if(m_bShowEdit)
    {
        RenameTitleEdit->show();
        QPointF RenameTitleEditMovePot(PYPP_RENAME_EDIT_HEIGHT,initY + PYPP_EXTRA_SPACE);
        int EditWidth = gEditWidth;
        gEditWidth = EditWidth;
        RenameTitleEdit->setFixedWidth(EditWidth);
        RenameTitleEdit->move(RenameTitleEditMovePot.toPoint());
        RenameTitleEdit->setFocus(Qt::ActiveWindowFocusReason);
        RenameTitleEdit->selectAll();
        initY += PYPP_WIDGET_SPACE;
    }else{
        RenameTitleEdit->hide();
    }

//    DEBUG_VAR(gEditWidth);

    QPointF TitleTopLeft(initX + PYPP_EXTRA_SPACE,PYPP_WIDGET_SPACE);
    QPointF TitleBottomRight(initX + gEditWidth,RenameTitleEdit->height() + PYPP_WIDGET_SPACE);
    QRectF TitleRect(TitleTopLeft,TitleBottomRight);
    m_TitleRect = TitleRect;

    // reposition EditBtn
    initX += gEditWidth;
    QPointF EditBtnMovePot(initX + PYPP_WIDGET_SPACE,staticInitY);
    EditBtn->move(EditBtnMovePot.toPoint());

    // reposition PinBtn
    initX += PYPP_WIDGET_SPACE + PYP_BTN_WIDTH;
    QPointF PinBtnMovePot(initX,staticInitY);
    PinBtn->move(PinBtnMovePot.toPoint());

    // reposition ShowBarBtn
    initX += PYPP_WIDGET_SPACE + PYP_BTN_WIDTH;
    QPointF ShowBarBtnMovePot(initX,staticInitY);
    ShowBarBtn->move(ShowBarBtnMovePot.toPoint());

    // reposition CloseBtn
    initX += PYPP_WIDGET_SPACE + PYP_BTN_WIDTH;
    QPointF CloseBtnMovePot(initX,staticInitY);
    CloseBtn->move(CloseBtnMovePot.toPoint());
    // reposition Edit
    QPointF EditMovePot(PYPP_WIDGET_SPACE + PYPP_EXTRA_SPACE,PYPP_WIDGET_SPACE * 2 + PYP_BTN_WIDTH + PYPP_EXTRA_SPACE);

    if(m_bShowBar)
    {
        ToolBar->show();
        QSize EditSize(m_Width - 2 * PYPP_WIDGET_SPACE - 2 * PYPP_EXTRA_SPACE,m_Height - 4 * PYPP_WIDGET_SPACE - ToolBar->height() - PYP_BTN_WIDTH );
        Edit->setFixedSize(EditSize);
    }else{
        ToolBar->hide();
        QSize EditSize(m_Width - 2 * PYPP_WIDGET_SPACE - 2* PYPP_EXTRA_SPACE,m_Height - 4 * PYPP_WIDGET_SPACE - PYP_BTN_WIDTH);
        Edit->setFixedSize(EditSize);
    }

//    DEBUG_VAR(m_bShowBar);

    Edit->move(EditMovePot.toPoint());


    if(m_bShowBar)
    {
        QPointF ToolBarMovePot(PYPP_WIDGET_SPACE,m_Height - ToolBar->height() - PYPP_WIDGET_SPACE * 2);
        ToolBar->move(ToolBarMovePot.toPoint());
    }





}

QToolButton* PYPadPrivate::CreateButton(const QString &strText, const QIcon &icon, const QString &strTip)
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
  private slot functions
*/
void PYPadPrivate::SetTitle(const QString &str)
{
    m_strTitle = str;
    m_bShowEdit = false;
    calcGeo();
    update();
}
void PYPadPrivate::StyleChanged(const QString &strStyle)
{
    m_bOneShotFlag = true;
    rules->setStyle(strStyle);
    rules->rehighlight();
}

void PYPadPrivate::BtnHandler()
{
    QToolButton* who = qobject_cast<QToolButton*>(sender());

    if(who == EditBtn)
    {
        m_bShowEdit = !m_bShowEdit;
        calcGeo();
        return ;
    }

    if(who == PinBtn)
    {
        m_bPin = !m_bPin;

        if(m_bPin)
        {
            PinBtn->setIcon(QIcon(":/images/pin.png"));
        }else{
            PinBtn->setIcon(QIcon(":/images/unpin.png"));
        }
        calcGeo();
        return ;
    }

    if(who == ShowBarBtn)
    {
        m_bShowBar = !m_bShowBar;

        if(m_bShowBar)
        {
            ShowBarBtn->setIcon(QIcon(":/images/hide_bar.png"));
            ShowBarBtn->setToolTip(tr("hide toolbar"));
            ToolBar->show();
        }else{
            ShowBarBtn->setIcon(QIcon(":/images/show_bar.png"));
            ShowBarBtn->setToolTip(tr("show toolbar"));
            ToolBar->hide();
        }
        calcGeo();
        return ;
    }

    if(who == CloseBtn)
    {
        if(!m_bPin)
        {
            hide();
        }
    }
}


void PYPadPrivate::BarBtnHandler()
{
    QToolButton* who =qobject_cast<QToolButton*>(sender());


    if(who == OpenBtn)
    {
        OpenFile();
        return ;
    }

    if(who == SaveBtn)
    {
        SaveFile();
    }

    if(who == ClearBtn)
    {
        Edit->clear();
    }

    if(who == ZoomInBtn)
    {
//        Edit->zoomIn();
    }

    if(who == ZoomOutBtn)
    {
//        Edit->zoomOut();
    }
}

void PYPadPrivate::OpenFile()
{
    QString strOpenFile = QFileDialog::getOpenFileName(this,tr("Open File"),".");

    if(strOpenFile.isEmpty())
    {
        return ;
    }

    m_bOneShotFlag = true;
    m_strSaveFilePath = strOpenFile;
    QString FileContent = Util::FSO::FileContent(strOpenFile);
    Edit->setPlainText(FileContent);

}

void PYPadPrivate::SaveFile()
{
    if(m_strSaveFilePath.isEmpty())
    {
        m_strSaveFilePath = QFileDialog::getSaveFileName(this,tr("Save File"));

        if(m_strSaveFilePath.isEmpty())
        {
            return ;
        }
    }

    QString strFileContent = Edit->toPlainText();
    Util::FSO::Write2File(m_strSaveFilePath,strFileContent);

    m_bSaved = true;
    if(m_strTitle.contains("*"))
    {
        m_strTitle = m_strTitle.left(m_strTitle.length() - 1);
        update();
    }
}

void PYPadPrivate::TextChanged()
{
    if(m_bOneShotFlag)
    {
        m_bOneShotFlag = false;
        return ;
    }
    m_bSaved = false;

    if(!m_strTitle.contains("*"))
    {
        m_strTitle += "*";
        update();
    }
}

/*!
  reimplementing paintEvent
*/

void PYPadPrivate::resizeEvent(QResizeEvent *)
{
    calcGeo();
}

void PYPadPrivate::showEvent(QShowEvent *)
{
    calcGeo();
}

void PYPadPrivate::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        m_bPressed = true;
        m_lastPot = e->pos();
    }
}

void PYPadPrivate::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bPressed)
    {
        QPoint MovePot = e->globalPos() - m_lastPot;
        move(MovePot);
    }
}

void PYPadPrivate::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(m_TitleRect.contains(e->pos()))
    {
        m_bShowEdit = true;
        calcGeo();
    }
}

void PYPadPrivate::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    m_bPressed = false;
}

void PYPadPrivate::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if(PYPP_USE_ANTIALIASING)
    {
        painter.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);
    }
    drawBg(&painter);
    drawAngle(&painter);
    drawTitle(&painter);

}

void PYPadPrivate::drawTitle(QPainter *painter)
{
    painter->save();
//    DEBUG_VAR(m_strTitle);

    QFont TextFont;
    TextFont.setBold(true);
    painter->setFont(TextFont);
    painter->setPen(PYPP_TITLE_COLOR);
    painter->drawText(m_TitleRect,m_strTitle,Qt::AlignVCenter|Qt::AlignLeft);
    painter->restore();
}

void PYPadPrivate::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen/*QPen(m_BgClr,PYPP_PEN_WIDTH)*/);

    QLinearGradient BgGradient(QPoint(0,0),QPoint(0,height()));
    BgGradient.setColorAt(0.0,m_LG.GetEndColor(m_BgClr));
    BgGradient.setColorAt(0.1,m_LG.GetStartColor(m_BgClr));
    BgGradient.setColorAt(0.9,m_LG.GetStartColor(m_BgClr));
    BgGradient.setColorAt(1.0,m_LG.GetEndColor(m_BgClr) );
    painter->setBrush(BgGradient);

    QRectF ThisRect(0,0,m_Width,m_Height);
    painter->drawRoundedRect(ThisRect,PYPP_RECT_RADIUS,PYPP_RECT_RADIUS);

    painter->restore();
}

void PYPadPrivate::drawAngle(QPainter *painter)
{
    if(m_Y == 0)
    {
        return ;
    }
    painter->save();

    int HalfAngleWidth = m_AngleWidth/3;

    QVector<QPointF> PotVec;
    QPointF TopPot(m_Width,m_Y - HalfAngleWidth);
    QPointF RightPot(m_Width + m_AngleWidth,m_Y);
    QPointF BottomPot(m_Width,m_Y + HalfAngleWidth);


    if(BottomPot.y() > m_Height)
    {
        painter->restore();
        return ;
    }
    PotVec.push_back(TopPot);
    PotVec.push_back(RightPot);
    PotVec.push_back(BottomPot);
    painter->setPen(Qt::NoPen);
    QLinearGradient AngleGradient(QPoint(m_Width+m_AngleWidth,0),QPoint(m_Width,0));
    AngleGradient.setColorAt(0.0, m_LG.GetEndColor(m_BgClr));
    AngleGradient.setColorAt(1.0,m_LG.GetStartColor(m_BgClr));
    painter->setBrush(AngleGradient);
    painter->drawPolygon(QPolygonF(PotVec));
    painter->restore();
}

/*!
  public interfaces
*/

void PYPadPrivate::Resize(int wid, int hei)
{
    m_Width = wid;
    m_Height = hei;
    setFixedSize(QSize(m_Width,m_Height));
    update();
}

void PYPadPrivate::SetBgColor(const QColor &clr)
{
    m_BgClr = clr;
    update();
}

void PYPadPrivate::SetY(int Y)
{
    m_Y = Y;
    update();
}

void PYPadPrivate::SetAngleWidth(qreal wid)
{
    m_AngleWidth = wid;
    qreal TotalWidth = m_Width + m_AngleWidth;
    setFixedWidth(TotalWidth);
}

int PYPadPrivate::GetAngleWidth() const
{
    return m_AngleWidth;
}

void PYPadPrivate::SetTopLeft(const QPoint &pot)
{
    QPoint ThisTopLeft(pot.x() - m_Width - PYP_WIDGET_SPACE,pot.y());
    move(ThisTopLeft);
}

QString PYPadPrivate::GetTitle() const
{
    return m_strTitle;
}

bool PYPadPrivate::IsSaved() const
{
    return m_bSaved;
}
/*!
  public slot functions
*/
void PYPadPrivate::Hide()
{
    if(!m_bPin)
    {
        hide();
    }
}
