#ifndef LINEARGRADIENT_H
#define LINEARGRADIENT_H

#include <QObject>
#include <QColor>
#include <QLinearGradient>
#include <QPoint>
#include <QRadialGradient>

#define LINEARGRADIENT_INCREMENT 66
class LinearGradient : public QObject
{
    Q_OBJECT
public:
    explicit LinearGradient(QObject *parent = 0);

public:
    static QLinearGradient GetGradient(const QPointF& startPos,const QPointF& endPos,const QColor& clr);
    static QBrush GetSecondGradient(const QPointF& startPos,const QPointF& endPos,const QColor& clr);
    static QLinearGradient GetThirdGradient(const QPointF& startPos,const QPointF& endPos,const QColor& clr);
    static QColor GetStartColor(const QColor& clr);
    static QColor GetEndColor(const QColor& clr);
};

class RadialGradient
{
public:
    static QRadialGradient GetRadialGradient(const QPointF& center,qreal radius,const QColor& clr);
};

#endif // LINEARGRADIENT_H
