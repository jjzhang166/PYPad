#include <QPainter>
#include <QMouseEvent>

#include "tctextruler.h"

static const QColor BgStartColor(221,221,221);
static const QColor BgStopColor(230,230,230);
static const QColor TextColor = QColor(89,89,89);
static const QColor MarkColor(165,165,165);
static const QColor IndicatorBrush(30,30,30);
static const int MaxHeight = 30;
static const int LongMarkLength = 7;
static const int ShortMarkLength = 5;
static const int TextMarkSpace = 1;
static const int IndicatorLength = 10;
static const int Top = 0;

class TcTextRuler::PrivateData {
public:
    PrivateData(): textWidth(0), column(0), spacing(0), startPos(0), mousePressed(false){}
    float textWidth;
    int column;
    int spacing;
    int startPos;
    bool mousePressed;
};

TcTextRuler::TcTextRuler(QWidget *parent) :
    QWidget(parent)
{
    mData = new PrivateData();
    mData->textWidth = fontMetrics().width('A') * 2;
    setFixedHeight(MaxHeight);
    setMouseTracking(true);
}

TcTextRuler::~TcTextRuler()
{
    delete mData;
}

void TcTextRuler::setTextWidth(qreal textWidth)
{
    mData->textWidth = textWidth;
    update();
}

void TcTextRuler::setColumn(int column)
{
    mData->column = column;
    update();
}

void TcTextRuler::setSpacing(int emptySpacing)
{
    mData->spacing = emptySpacing;
    update();
}

void TcTextRuler::setStartPosition(int xPos)
{
    mData->startPos = xPos;
    update();
}

void TcTextRuler::enterEvent(QEvent *)
{
    setCursor(Qt::IBeamCursor);
}

void TcTextRuler::leaveEvent(QEvent *)
{
    unsetCursor();
}

void TcTextRuler::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawBg(&painter);
    drawMarks(&painter);
    drawIndicator(&painter);
}

void TcTextRuler::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        mData->mousePressed = true;
        mData->column = (int)(e->pos().x() - mData->spacing )/ mData->textWidth;
        update();
    }
}

void TcTextRuler::mouseMoveEvent(QMouseEvent *e)
{
    if (mData->mousePressed) {
        int xPos = e->pos().x();
        if (xPos < 0 || xPos > width()) {
           return;
        }
        setColumn(xPos/mData->textWidth);
    }
}

void TcTextRuler::mouseReleaseEvent(QMouseEvent *)
{
    mData->mousePressed = false;
}

void TcTextRuler::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(MarkColor);

    QLinearGradient bgGradient(rect().topLeft(), rect().bottomLeft());
    bgGradient.setColorAt(0.0, BgStartColor);
    bgGradient.setColorAt(1.0, BgStopColor);
    painter->setBrush(bgGradient);

    painter->drawRect(rect());
    painter->restore();
}

void TcTextRuler::drawMarks(QPainter *painter)
{
    painter->save();
    qreal initX = mData->spacing;

    // draw underline
    QPointF leftPot(0, height() - 1);
    QPointF rightPot(width(), height() - 1);
    painter->setPen(MarkColor);
    painter->drawLine(leftPot, rightPot);

    int counter = 0;
    while (initX < (width() + mData->startPos)) {
        if (counter % 5 == 0) {
            // draw long mark
            QPointF upperPoint(initX - mData->startPos, height() - LongMarkLength);
            if (upperPoint.x() >= mData->spacing) {
                painter->setPen(MarkColor);
                QPointF bottomPoint(initX - mData->startPos, height());
                painter->drawLine(upperPoint, bottomPoint);
            }

            // draw text
            if (counter % 10 == 0) {
                painter->setPen(TextColor);
                const QString text = tr("%1").arg(counter);
                int textWidth = fontMetrics().width(text);
                QPointF textPoint(initX - mData->startPos - textWidth/2, upperPoint.y() - TextMarkSpace);
                if (textPoint.x() <= mData->spacing && mData->startPos == 0) {
                    textPoint.setX(mData->spacing);
                } else if (textPoint.x() + textWidth > width()) {
                    textPoint.setX(width() - textWidth);
                }

                if ((textPoint.x() + textWidth) >= mData->spacing) {
                    painter->drawText(textPoint, text);
                }
            }
        } else {
            // draw short mark
            if ((initX - mData->startPos) >= mData->spacing) {
                QPointF upperPoint(initX - mData->startPos, height() - ShortMarkLength);
                QPointF bottomPoint(initX - mData->startPos, height());
                painter->setPen(MarkColor);
                painter->drawLine(upperPoint, bottomPoint);
            }
        }

        // increment initX
        initX += mData->textWidth;
        counter++;
    }
    painter->restore();
}

void TcTextRuler::drawIndicator(QPainter *painter)
{
    int pivotXPos = mData->column * mData->textWidth + mData->spacing;
    if (pivotXPos + IndicatorLength/2 - mData->startPos < mData->spacing) {
        return;
    }

    painter->setPen(IndicatorBrush);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setBrush(IndicatorBrush);
    QPointF topLeft(pivotXPos - IndicatorLength/2 - mData->startPos, Top);
    QPointF topRight(pivotXPos + IndicatorLength/2 - mData->startPos, Top);
    QPointF bottomPoint(pivotXPos - mData->startPos, IndicatorLength);
    QPolygonF polygon;
    polygon.push_back(topLeft);
    polygon.push_back(topRight);
    polygon.push_back(bottomPoint);
    painter->drawPolygon(polygon);
    painter->restore();
}
