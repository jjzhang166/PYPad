#ifndef PYFADEWIDGET_H
#define PYFADEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

#include "Util.h"

#define PYF_TIMER_INTERVAL 50
#define PYF_PIXEL_INCREMENT 10
#define PYF_INCREMENT_FACTOR 10

class PYFadeWidget : public QWidget
{
    Q_OBJECT
public:
    enum Direction { Horizontal , Vertical } ;
    explicit PYFadeWidget(QWidget *parent = 0);

public:
    /// public interfaces
    void setDirection(Direction d);

protected:
    void paintEvent(QPaintEvent *);

    void showEvent(QShowEvent *);

    void closeEvent(QCloseEvent *);

    QSize sizeHint() const
    {
        return QSize(200,200);
    }

private:
    Direction m_direction;
    qreal m_opacity;
    qreal m_dPixel;
    bool m_bShow;
    QTimer* m_timer;


private:
    void initVariables();

private slots:
    void slot_Update();
};

#endif // PYFADEWIDGET_H
