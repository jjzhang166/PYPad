#include "pypadlineedit.h"

PYPadLineEdit::PYPadLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}


void PYPadLineEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        emit sig_SetText(text());
    }

    QLineEdit::keyPressEvent(e);
}
