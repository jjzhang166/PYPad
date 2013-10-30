#ifndef PYPADBUTTON_H
#define PYPADBUTTON_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "pypad.h"
#define PYPB_RECT_RADIUS 7

class PYPadButton : public QWidget
{
    Q_OBJECT
public:
    explicit PYPadButton(QWidget *parent = 0);


public:
    void SetText(const QString& str);

protected:
    QSize sizeHint() const
    {
        return QSize(80,40);
    }

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *);


private:
    void drawButton(QPainter* painter);


private:
    void initVariables();

private:
    QString m_strText;
    bool m_bPressed ;
    LinearGradient m_LG;

Q_SIGNALS:
    void sig_clicked();
    void sig_released();

};

#endif // PYPADBUTTON_H
