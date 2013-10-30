#ifndef PYPADWRAPPER_H
#define PYPADWRAPPER_H

#include <QWidget>

#include "pypad.h"
#include <QHBoxLayout>

class PYPadWrapper : public QWidget
{
    Q_OBJECT
public:
    explicit PYPadWrapper(QWidget *parent = 0);


private:
    QHBoxLayout* mainLayout;
    QWidget* leftWidget;
    PYPad* pyPad;

private:
    void initWidgets();
};

#endif // PYPADWRAPPER_H
