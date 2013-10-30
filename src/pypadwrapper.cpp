#include "pypadwrapper.h"

PYPadWrapper::PYPadWrapper(QWidget *parent) :
    QWidget(parent)
{
    mainLayout=new QHBoxLayout;
    leftWidget =new  QWidget(this);
    pyPad = new PYPad(this);
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(pyPad);
    setLayout(mainLayout);

    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

}
