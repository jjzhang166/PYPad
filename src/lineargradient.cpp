#include "lineargradient.h"

LinearGradient::LinearGradient(QObject *parent) :
    QObject(parent)
{

}

QLinearGradient LinearGradient::GetGradient(const QPointF &startPos, const QPointF &endPos, const QColor &clr)
{
    QLinearGradient gradient(startPos,endPos);
    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();

    int startRed=red-LINEARGRADIENT_INCREMENT>=0 ? red-LINEARGRADIENT_INCREMENT : 0;
    int startGreen=green-LINEARGRADIENT_INCREMENT>=0 ? green-LINEARGRADIENT_INCREMENT : 0;
    int startBlue=blue-LINEARGRADIENT_INCREMENT>=0 ? blue-LINEARGRADIENT_INCREMENT : 0;

    QColor startClr(startRed,startGreen,startBlue);

    int endRed=red+LINEARGRADIENT_INCREMENT < 255 ? red+LINEARGRADIENT_INCREMENT : 255;
    int endGreen= green+LINEARGRADIENT_INCREMENT < 255 ? green+LINEARGRADIENT_INCREMENT : 255 ;
    int endBlue= blue+LINEARGRADIENT_INCREMENT < 255 ? blue+LINEARGRADIENT_INCREMENT : 255;

    QColor endClr(endRed,endGreen,endBlue);

    gradient.setColorAt(0.0,endClr);
    gradient.setColorAt(0.5,clr);
    gradient.setColorAt(1.0,endClr);

    return gradient;
}
QBrush LinearGradient::GetSecondGradient(const QPointF &startPos, const QPointF &endPos, const QColor &clr)
{
    QLinearGradient gradient(startPos,endPos);
    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();

    int startRed=red-LINEARGRADIENT_INCREMENT>=0 ? red-LINEARGRADIENT_INCREMENT : 0;
    int startGreen=green-LINEARGRADIENT_INCREMENT>=0 ? green-LINEARGRADIENT_INCREMENT : 0;
    int startBlue=blue-LINEARGRADIENT_INCREMENT>=0 ? blue-LINEARGRADIENT_INCREMENT : 0;

    QColor startClr(startRed,startGreen,startBlue);

    int endRed=red+LINEARGRADIENT_INCREMENT < 255 ? red+LINEARGRADIENT_INCREMENT : 255;
    int endGreen= green+LINEARGRADIENT_INCREMENT < 255 ? green+LINEARGRADIENT_INCREMENT : 255 ;
    int endBlue= blue+LINEARGRADIENT_INCREMENT < 255 ? blue+LINEARGRADIENT_INCREMENT : 255;

    QColor endClr(endRed,endGreen,endBlue);

    gradient.setColorAt(0.0,endClr);
    gradient.setColorAt(0.3,clr);
    gradient.setColorAt(0.7,clr);
    gradient.setColorAt(1.0,endClr);

    return gradient;
}

QLinearGradient LinearGradient::GetThirdGradient(const QPointF &startPos, const QPointF &endPos, const QColor &clr)
{
    QLinearGradient gradient(startPos,endPos);
    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();

    int startRed=red-LINEARGRADIENT_INCREMENT>=0 ? red-LINEARGRADIENT_INCREMENT : 0;
    int startGreen=green-LINEARGRADIENT_INCREMENT>=0 ? green-LINEARGRADIENT_INCREMENT : 0;
    int startBlue=blue-LINEARGRADIENT_INCREMENT>=0 ? blue-LINEARGRADIENT_INCREMENT : 0;

    QColor startClr(startRed,startGreen,startBlue);

    int endRed=red+LINEARGRADIENT_INCREMENT < 255 ? red+LINEARGRADIENT_INCREMENT : 255;
    int endGreen= green+LINEARGRADIENT_INCREMENT < 255 ? green+LINEARGRADIENT_INCREMENT : 255 ;
    int endBlue= blue+LINEARGRADIENT_INCREMENT < 255 ? blue+LINEARGRADIENT_INCREMENT : 255;

    QColor endClr(endRed,endGreen,endBlue);

    gradient.setColorAt(0.0,endClr);
    gradient.setColorAt(0.3,clr);
    gradient.setColorAt(1.0,endClr);

    return gradient;
}

QColor LinearGradient::GetStartColor(const QColor &clr)
{
    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();

    int startRed=red-LINEARGRADIENT_INCREMENT>=0 ? red-LINEARGRADIENT_INCREMENT : 0;
    int startGreen=green-LINEARGRADIENT_INCREMENT>=0 ? green-LINEARGRADIENT_INCREMENT : 0;
    int startBlue=blue-LINEARGRADIENT_INCREMENT>=0 ? blue-LINEARGRADIENT_INCREMENT : 0;

    QColor startClr(startRed,startGreen,startBlue);
    return startClr;
}

QColor LinearGradient::GetEndColor(const QColor &clr)
{
    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();
    int endRed=red+LINEARGRADIENT_INCREMENT < 255 ? red+LINEARGRADIENT_INCREMENT : 255;
    int endGreen= green+LINEARGRADIENT_INCREMENT < 255 ? green+LINEARGRADIENT_INCREMENT : 255 ;
    int endBlue= blue+LINEARGRADIENT_INCREMENT < 255 ? blue+LINEARGRADIENT_INCREMENT : 255;

    QColor endClr(endRed,endGreen,endBlue);
    return endClr;
}

/*********************************************/
/*!   RadialGradient                        !*/
/*********************************************/
QRadialGradient RadialGradient::GetRadialGradient(const QPointF &center, qreal radius, const QColor &clr)
{
    QPointF topPot(center.x(),center.y() - radius/2);
    QPointF bottomPot(center.x(),center.y() + radius/2);
    QRadialGradient Gradient(center,radius,topPot);


    int red=clr.red();
    int green=clr.green();
    int blue=clr.blue();

    int startRed=red-LINEARGRADIENT_INCREMENT>=0 ? red-LINEARGRADIENT_INCREMENT : 0;
    int startGreen=green-LINEARGRADIENT_INCREMENT>=0 ? green-LINEARGRADIENT_INCREMENT : 0;
    int startBlue=blue-LINEARGRADIENT_INCREMENT>=0 ? blue-LINEARGRADIENT_INCREMENT : 0;

    QColor startClr(startRed,startGreen,startBlue);

    int endRed=red+LINEARGRADIENT_INCREMENT < 255 ? red+LINEARGRADIENT_INCREMENT : 255;
    int endGreen= green+LINEARGRADIENT_INCREMENT < 255 ? green+LINEARGRADIENT_INCREMENT : 255 ;
    int endBlue= blue+LINEARGRADIENT_INCREMENT < 255 ? blue+LINEARGRADIENT_INCREMENT : 255;

    QColor endClr(endRed,endGreen,endBlue);

    Gradient.setColorAt(0.0,endClr);
    Gradient.setColorAt(1.0,startClr);

    return Gradient;

}

