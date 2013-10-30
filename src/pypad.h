#ifndef PYPAD_H
#define PYPAD_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QToolButton>
#include <QTimer>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QComboBox>

#include "lineargradient.h"
#include "colorpicker.h"
#include "Util.h"
#include "pypadconfig.h"
#include "highlighter.h"

#define PYP_HEADER_HEIGHT 30
#define PYP_CTRL_PANEL_HEIGHT 26
#define PYP_TOTAL_HEADER_HEIGHT PYP_HEADER_HEIGHT + PYP_CTRL_PANEL_HEIGHT
#define PYP_LEFT_SPACE 4
#define PYP_BALL_PEN_WIDTH 4
#define PYP_BTN_SPACE 2
#define PYP_BTN_WIDTH PYP_CTRL_PANEL_HEIGHT - 2*PYP_BTN_SPACE
#define PYP_BTN_SIZE QSize(PYP_BTN_WIDTH,PYP_BTN_WIDTH)
#define PYP_CTRL_BTN_COUNT 6
#define PYP_BALL_SPACE 3
#define PYP_OPACITY_INCREMENT 0.1
#define PYP_BALL_COUNT_LIMIT 5
#define PYP_WIDGET_SPACE 4

#define PYP_COLOR_DECREMENT 70

#define PYP_USE_ANTIALIASING true
#define PYP_HEADER_COLOR QColor(24,84,133)
#define PYP_HEADER_RECT_RADIUS 10

#define PYP_TIMER_INTERVAL 50
#define PYP_SHRINK_INTERVAL 50
#define PYP_SHRINK_INCREMENT 2

#define PYP_HIGHLIGHT_BALL     if(m_HeaderRect.contains(e->pos())) \
                               { \
                                    return ; \
                               } \
                               int BallCnt = m_BallInfoVec.count(); \
                               for(int BallIndex = 0;BallIndex < BallCnt ;BallIndex++) \
                               { \
                                    if(m_BallInfoVec.at(BallIndex)->GetRect().contains(e->pos())) \
                                    { \
                                        m_BallInfoVec.at(BallIndex)->SetPressed(true); \
                                        m_nCurrentBallIndex  = BallIndex ; \
                                    }else{ \
                                        m_BallInfoVec.at(BallIndex)->SetPressed(false); \
                                    } \
                                } \
                                    update();

#define REPOSITION_PRIVATE     QPoint gTopLeft = mapToGlobal(rect().topLeft()); \
                               int TopX = gTopLeft.x(); \
                               int PrivateWidth = pPrivate->width(); \
                               QPoint PrivateMovePot(TopX - PrivateWidth + pPrivate->GetAngleWidth() -PYP_WIDGET_SPACE,gTopLeft.y()); \
                               pPrivate->move(PrivateMovePot); \
                               pPrivate->show();


#define GET_STATIC_HEIGHT     m_StaticHeight  = PYP_HEADER_HEIGHT +  PYP_CTRL_PANEL_HEIGHT + m_BallInfoVec.count() * 2 * m_BallRadius + PYP_BTN_SPACE * (m_BallInfoVec.count() - 1);



class PYPadPrivate;
/*****************************************/
/*!     PYPadPrivate section            !*/
/*****************************************/

#include <QToolBar>
#include <QFileDialog>
#include "Util.h"
#include "pypadlineedit.h"
#include "pyfadewidget.h"
#include "codedocument.h"

#define PYPP_WIDGET_SPACE 4
#define PYPP_HEADER_HEIGHT 25
#define PYPP_RECT_RADIUS 14
#define PYPP_CONFIG_BUTTON_COUNT 4
#define PYPP_PEN_WIDTH 1
#define PYPP_EXTRA_SPACE 2
#define PYPP_RENAME_EDIT_HEIGHT PYPP_WIDGET_SPACE+PYPP_EXTRA_SPACE

#define PYPP_USE_ANTIALIASING true

#define PYPP_TITLE_COLOR  Qt::black


class PYPadPrivate : public PYFadeWidget
{
    Q_OBJECT
public:
    PYPadPrivate(QWidget* parent = 0);

public:
    void Resize(int wid,int hei);

    void SetBgColor(const QColor& clr);

    void SetY(int Y);

    void SetAngleWidth(qreal wid);

    int GetAngleWidth() const;

    void SetTopLeft(const QPoint& pot);

    QString GetTitle() const;

    bool IsSaved() const;


public Q_SLOTS:

    void Hide();

protected:
    QSize sizeHint() const
    {
        return QSize(400,500);
    }


    void paintEvent(QPaintEvent *);

    void resizeEvent(QResizeEvent *);

    void showEvent(QShowEvent *);

    void mousePressEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *);

    void mouseMoveEvent(QMouseEvent *);

    void mouseDoubleClickEvent(QMouseEvent *);

private: /// painting functions
    void drawTitle(QPainter* painter);
    void drawBg(QPainter* painter);
    void drawAngle(QPainter* painter);


private: /// private variables
    int m_Width;
    int m_Height;
    int m_Y;

    qreal m_AngleWidth;

    QColor m_BgClr;
    LinearGradient m_LG;
    ColorPicker m_CP;

    QString m_strTitle;
    QString m_strSaveFilePath;

    QPoint m_lastPot;

    bool m_bShowEdit;
    bool m_bPin;
    bool m_bShowBar;
    bool m_bPressed;
    bool m_bSaved;
    bool m_bOneShotFlag;

    QRectF m_TitleRect;


private: /// private widget variables
    QToolButton* EditBtn;
    QToolButton* PinBtn;
    QToolButton* ShowBarBtn;
    QToolButton* CloseBtn;

    QToolButton* OpenBtn;
    QToolButton* SaveBtn;
    QToolButton* ClearBtn;
    QToolButton* ZoomInBtn;
    QToolButton* ZoomOutBtn;
    PYPadLineEdit* RenameTitleEdit;
    QLabel* StyleLabel;
    QComboBox* StyleCombo;
    QToolBar* ToolBar;
    CodeDocument* Edit;
    Highlighter* rules;
private: /// private utility functions

    void initVariables();
    void initWidgets();
    void initSettings();
    void initMessages();
    QToolButton* CreateButton(const QString& strText,const QIcon& icon,const QString& strTip);


    void calcGeo();

private Q_SLOTS:
    void SetTitle(const QString& str);
    void StyleChanged(const QString& strStyle);
    void BtnHandler();
    void BarBtnHandler();

    void OpenFile();
    void SaveFile();
    void TextChanged();
};

/*************************************/
/*!    PYPad section                !*/
/*************************************/

class PYPad : public PYFadeWidget
{


    class BallInfo
    {
    public:
        explicit BallInfo(): m_nIndex(0),m_Private(0),m_Opacity(0),m_bPressed(false),m_bVisible(true){}
        ~BallInfo(){ if(m_Private){ delete m_Private; m_Private = 0;}}

    public:
        void SetIndex(int index)
        {
            m_nIndex = index;
        }

        int GetIndex() const
        {
            return m_nIndex;
        }


        void SetPrivate(PYPadPrivate* p)
        {
            if(m_Private)
            {
                delete m_Private;
            }

            m_Private = p;

        }

        void Destroy()
        {
            m_Private->close();
//            if(m_Private)
//            {
//                delete m_Private;
//                m_Private = 0;
//            }
        }

        PYPadPrivate* GetPrivate()
        {
            return m_Private;
        }

        void SetColor(const QColor& clr)
        {
            m_Clr = clr;
        }

        QColor GetColor() const
        {
            return m_Clr;
        }

        void SetOpacity(qreal opacity)
        {
            m_Opacity = opacity;
        }

        qreal GetOpacity() const
        {
            return m_Opacity;
        }

        void SetRect(const QRectF& rect)
        {
            m_Rect = rect;
        }

        QRectF GetRect() const
        {
            return m_Rect;
        }

        void SetPressed(bool pressed)
        {
            m_bPressed = pressed;
        }

        bool IsPressed() const
        {
            return m_bPressed;
        }

        void SetVisible(bool visible)
        {
            m_bVisible = visible;
        }

        bool IsVisible() const
        {
            return m_bVisible;
        }

    private:
        int m_nIndex;
        PYPadPrivate* m_Private;
        QColor m_Clr;
        qreal m_Opacity;
        QRectF m_Rect;
        bool m_bPressed;
        bool m_bVisible;
    };

    Q_OBJECT
public:
    explicit PYPad(QWidget *parent = 0);


protected:
    void paintEvent(QPaintEvent *);

    QSize sizeHint() const
    {
        return QSize(200,400);
    }

    void mousePressEvent(QMouseEvent *);

    void mouseMoveEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *);

    void mouseDoubleClickEvent(QMouseEvent *);

    void resizeEvent(QResizeEvent *);

    void showEvent(QShowEvent *);

    void closeEvent(QCloseEvent *);

    void moveEvent(QMoveEvent *);

private:/// painting functions

    void drawHeader(QPainter* painter);

    void drawBalls(QPainter* painter);




private : /// private utility functions
       void initVariables();
       void initWidgets();
       void initSettings();
       void initMessages();
       void calcGeo();
       void gotoTopRight();
       void repositionAllPads();
       void resetPrivate();
       bool hasUnsavedDoc();


       ///
       QToolButton* CreateButton(const QString& strText,const QIcon& icon,const QString& strTip);


private: /// private widget variables
    QToolButton* AddBtn;
    QToolButton* DeleteBtn;
    QToolButton* ConfigBtn;
    QToolButton* ExitBtn;
    QToolButton* RestoreBtn;
    QToolButton* ShowBtn;

    PYPadConfig* ConfigWindow;

private: /// private variables
    qreal m_Height;
    qreal m_StaticHeight;
    qreal m_Width;
    qreal m_BallRadius;

    bool m_bPressed;
    bool m_bShowBall;
    bool m_bStickToFather;

    QPoint m_lastPot;

    int m_nBallIndex;
    int m_nCenterX;
    int m_nCurrentBallIndex;

    ColorPicker m_CP;
    RadialGradient m_RG;

    QVector<BallInfo*> m_BallInfoVec;

    QTimer* m_FadeTimer;
    QTimer* m_ShrinkTimer;

    QRectF m_HeaderRect;

    QPainterPath m_IndicatorPath;
    QVector< QPair<QRectF,QString> > m_RectTitleVec;


private Q_SLOTS:
    void BtnHandler();

    void AddBall();

    void DeleteBall();

    void DoFade();

    void DoShrink();

    void ShowBalls();

    void SetStickToFather(bool stick);

    void SetPadSize(const QSize& size);

Q_SIGNALS:
    void sig_SetMovePoint(const QPoint& pot);
};


QColor LighterColor(const QColor& clr);
#endif // PYPAD_H
