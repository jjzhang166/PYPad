#ifndef SIMPRINTER_H
#define SIMPRINTER_H

#include "simscintilladefs.h"

#include <QPrinter>
#include <QObject>

class SimScintilla;
class SimPrinterPrivate;
class SimPrinter : public QPrinter
{
public:
    SimPrinter(SimScintilla* scintilla, QPrinter::PrinterMode mode = ScreenResolution);
    virtual ~SimPrinter();

    void setWrapMode(SimSci::Wrap wrap);
    SimSci::Wrap getWrapMode() const;

    void setMagnification(int mag);
    int getMagnification() const;

    bool print(int from, int to);
    bool printAll();

private:
    SimPrinterPrivate* d;
    Q_DISABLE_COPY(SimPrinter)
};

#endif // SIMPRINTER_H
