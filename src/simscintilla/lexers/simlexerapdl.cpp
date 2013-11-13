#include "simlexerapdl.h"
#include "../simdbmanager.h"

#ifdef USELOG
#include "../logutil.h"
#endif

SimLexerApdl::SimLexerApdl(SimScintilla* parent): SimAbstractLexer(parent)
{
    m_dbMgr = SimDbManager::instance();
    m_dbMgr->setDbFile("E:/apdl.db3");
}

/*!
 * reimp
 */
QString SimLexerApdl::description(int style) const {
    switch(style) {
    case Comment:
        return tr("comments");

    case CommentBlock:
        return tr("commentblocks");

    case Number:
        return tr("numbers");

    case String:
        return tr("strings");

    case Operator:
        return tr("operators");

    case Word:
        return tr("words");

    case Processor:
        return tr("processors");

    case Command:
        return tr("commands");

    case SlashCommand:
        return tr("slashcommands");

    case StarCommand:
        return tr("starcommands");

    case Argument:
        return tr("arguments");

    case Function:
        return tr("functions");
    }

    return QString();
}

/*!
 * reimp
 */
QString SimLexerApdl::keywords(int set) const {
    QString strKws;

    if (set == 1) {
        QStringList funcAndCmds = m_dbMgr->allCommands("121") + m_dbMgr->allFunctions("121");
        strKws = funcAndCmds.join(" ");
    } else if(set == 2) {
        QStringList funcAndCmds = m_dbMgr->allCommands("130") + m_dbMgr->allFunctions("130");
        strKws = funcAndCmds.join(" ");
    } else if(set == 3) {
        QStringList funcAndCmds = m_dbMgr->allCommands("140") + m_dbMgr->allFunctions("140");
        strKws = funcAndCmds.join(" ");
    } else if(set == 4) {
        QStringList funcAndCmds = m_dbMgr->allCommands("150") + m_dbMgr->allFunctions("150");
        strKws = funcAndCmds.join(" ");
    }

#ifdef USELOG
    LOG_VAR(strKws);
#endif
    return strKws;
}

/*!
 * reimp
 */
QColor SimLexerApdl::defaultColor(int style) const {
    switch (style)
    {
    case Default:
    case Number:
    case String:
    case Word:
        return SimAbstractLexer::defaultColor();

    case Comment:
        return QColor(0x00, 0x7f, 0x00);

    case CommentBlock:
        return QColor(0x3f, 0x70, 0x3f);

    case Processor:
        return QColor(0x7f, 0x7f, 0x00);

    case Operator:
        return QColor(0x00, 0x00, 0x00);

    case Command:
    case SlashCommand:
    case StarCommand:
        return QColor(0x00, 0x00, 0x9f);

    case Argument:
        return QColor(0x80, 0x40, 0x20);

    case Function:
        return QColor(0x65, 0x99, 0x00);
    }

    return SimAbstractLexer::defaultColor(style);
}

/*!
 * reimp
 */
bool SimLexerApdl::defaultEolFill(int style) const {
    switch (style)
    {
    case Default:
    case Comment:
    case CommentBlock:
    case Processor:
    case Command:
    case SlashCommand:
    case StarCommand:
        return true;
    }

    return SimAbstractLexer::defaultEolFill(style);
}

/*!
 * reimp
 */
QFont SimLexerApdl::defaultFont(int style) const {
    QFont f;
    switch(style) {
    case Comment:
    case CommentBlock:

#if defined(Q_OS_WIN)
        f = QFont("Comic Sans MS",9);
#elif defined(Q_OS_MAC)
        f = QFont("Comic Sans MS", 12);
#else
        f = QFont("Bitstream Vera Serif",9);
#endif

    case Processor:
    case Command:
    case SlashCommand:
    case StarCommand:
        f = SimAbstractLexer::defaultFont(style);
        f.setBold(true);
        break;

    default:
        f = SimAbstractLexer::defaultFont(style);
    }
    return f;
}

/*!
 * reimp
 */
QColor SimLexerApdl::defaultBackgroundColor(int style) const {
    switch (style)
    {
    case Default:
    case String:
    case Word:
    case Number:
        return SimAbstractLexer::defaultBackgroundColor();

    case Comment:
    case CommentBlock:
        return QColor(0xff,0xf3,0xff);
    }

    return SimAbstractLexer::defaultBackgroundColor(style);
}
