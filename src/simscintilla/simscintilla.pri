INCLUDEPATH = $$PWD
DEPENDPATH = $$PWD

include($$PWD/lexers/lexers.pri)
include($$PWD/scintillaedit/scintillaedit.pri)

QT += printsupport sql
DEFINES += SIMSCINTILLA_LIBRARY

SOURCES += \
            $$PWD/simscintilla.cpp \
            $$PWD/simdocument.cpp \
            $$PWD/simabstracttextstyle.cpp \
            $$PWD/simtextstyle.cpp \
            $$PWD/simprinter.cpp \
            $$PWD/simmacro.cpp \
            $$PWD/simcommand.cpp \
            $$PWD/simcommandproxy.cpp \
            $$PWD/simabstractlexer.cpp \
            $$PWD/simstyledtext.cpp \
            $$PWD/simabstractapis.cpp \
            $$PWD/simapis.cpp \
            $$PWD/simdbmanager.cpp \
            $$PWD/simlexerfactory.cpp \
            $$PWD/logutil.cpp \
            $$PWD/simscintillaex.cpp \
            $$PWD/tctextruler.cpp

HEADERS +=\
            $$PWD/simscintilladefs.h \
            $$PWD/simscintilla_global.h \
            $$PWD/simscintilla.h \
            $$PWD/simdocument.h \
            $$PWD/simabstracttextstyle.h \
            $$PWD/simtextstyle.h \
            $$PWD/simprinter.h \
            $$PWD/simmacro.h \
            $$PWD/simcommand.h \
            $$PWD/simcommandproxy.h \
            $$PWD/simabstractlexer.h \
            $$PWD/simstyledtext.h \
            $$PWD/simscintilla_global.h \
            $$PWD/simabstractapis.h \
            $$PWD/simapis.h \
            $$PWD/simdbmanager.h \
            $$PWD/simlexerfactory.h \
            $$PWD/logutil.h \
            $$PWD/simscintillaex.h \
            $$PWD/tctextruler.h
