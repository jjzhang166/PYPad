#include "pypad.h"
#include <QApplication>
#include "Util.h"
#include "pypadwrapper.h"
#include "pypadbutton.h"
#include "pypadconfig.h"

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    PYPad win;
    win.show();
    return app.exec();
}
