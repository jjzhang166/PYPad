#ifndef PYPADLINEEDIT_H
#define PYPADLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QWidget>

class PYPadLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit PYPadLineEdit(QWidget *parent = 0);


protected:
    void keyPressEvent(QKeyEvent *);
Q_SIGNALS:
    void sig_SetText(const QString& str);

};

#endif // PYPADLINEEDIT_H
