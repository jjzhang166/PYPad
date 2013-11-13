#include <QPainter>
#include <QStack>

#include "simprinter.h"
#include "simscintilla.h"

class SimPrinterPrivate
{
public:
    SimScintilla* scintilla;
    SimSci::Wrap wrapMode;
    int magnification;

    sptr_t print(unsigned int msg, unsigned long wParam, QPainter *hdc,
               const QRect &rc, long cpMin, long cpMax) {
        RangeToFormat rf;

        rf.hdc = rf.hdcTarget = reinterpret_cast<SurfaceID>(hdc);

        rf.rc.left = rc.left();
        rf.rc.top = rc.top();
        rf.rc.right = rc.right() + 1;
        rf.rc.bottom = rc.bottom() + 1;

        rf.chrg.cpMin = cpMin;
        rf.chrg.cpMax = cpMax;

        return scintilla->send(msg, wParam, reinterpret_cast<sptr_t>(&rf));
    }
};

SimPrinter::SimPrinter(SimScintilla* scintilla, QPrinter::PrinterMode mode): QPrinter(mode)
{
    d = new SimPrinterPrivate;
    d->scintilla = scintilla;
}

SimPrinter::~SimPrinter()
{
    SAFE_RELEASE(d)
}

/*!
 * \brief setWrapMode
 * 设置绕行模式
 * \param wrap
 *
 */
void SimPrinter::setWrapMode(SimSci::Wrap wrap)
{
    d->wrapMode = wrap;
}

SimSci::Wrap SimPrinter::getWrapMode() const
{
    return d->wrapMode;
}

void SimPrinter::setMagnification(int mag)
{
    d->magnification = mag;
}

int SimPrinter::getMagnification() const
{
    return d->magnification;
}

bool SimPrinter::print(int from, int to)
{
    if (d->scintilla == nullptr) {
        return false;
    }

    // Setup the printing area.
    QRect printingArea;

    printingArea.setX(0);
    printingArea.setY(0);

    printingArea.setWidth(width());
    printingArea.setHeight(height());

    // Get the page range.
    int theFromPage, theToPage;

    theFromPage = fromPage();
    theToPage = toPage();

    // Find the position range.
    long startPos, endPos;

    endPos = d->scintilla->length();

    startPos = (from > 0 ? d->scintilla->positionFromLine(from) : 0);

    if (to >= 0) {
        long toPos = d->scintilla->positionFromLine(to + 1);

        if (endPos > toPos)
            endPos = toPos;
    }

    if (startPos >= endPos)
        return false;

    QPainter painter(this);
    bool reverseOrder = (pageOrder() == LastPageFirst);
    bool needNewPage = false;

    d->scintilla->setPrintMagnification(d->magnification);
    d->scintilla->setPrintWrapMode(d->wrapMode);

    for (int i = 1; i <= numCopies(); ++i) {
        // If we are printing in reverse order page order then remember the start
        // position of each page.
        QStack<long> pageStarts;

        int currPage = 1;
        long pos = startPos;

        while (pos < endPos) {
            // See if we have finished the requested page range.
            if (theToPage > 0 && theToPage < currPage)
                break;

            // See if we are going to render this page, or just see how much
            // would fit onto it.
            bool render = false;

            if (theFromPage == 0 || theFromPage <= currPage) {
                if (reverseOrder)
                    pageStarts.push(pos);
                else {
                    render = true;

                    if (needNewPage) {
                        if (!newPage())
                            return false;
                    }
                    else
                        needNewPage = true;
                }
            }

            QRect area = printingArea;

#ifdef RESERVED
            formatPage(painter,render,area,currPage);
#endif
            pos = d->print(SimSci::CMD_FormatRange, render, &painter, area, pos, endPos);
            ++currPage;
        }

        // All done if we are printing in normal page order.
        if (!reverseOrder)
            continue;

        // Now go through each page on the stack and really print it.
        while (!pageStarts.isEmpty()) {
            --currPage;

            long ePos = pos;
            pos = pageStarts.pop();

            if (needNewPage) {
                if (!newPage())
                    return false;
            }
            else
                needNewPage = true;

            QRect area = printingArea;

#ifdef RESERVED
            formatPage(painter,true,area,currPage);
#endif
            pos = d->print(SimSci::CMD_FormatRange, true, &painter, area, pos, ePos);
        }
    }

    return true;
}

bool SimPrinter::printAll()
{
    return print(-1, -1);
}

