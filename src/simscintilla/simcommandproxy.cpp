#include "simcommandproxy.h"
#include "simcommand.h"
#include "simscintilla.h"

SimScintilla* SimCommandProxy::m_scintilla = nullptr;
QList<SimCommand* > SimCommandProxy::m_cmds;

SimCommandProxy::SimCommandProxy(SimScintilla *parent)
{
    m_scintilla = parent;
    init();
}

SimCommandProxy::~SimCommandProxy()
{
    clear();
}

bool SimCommandProxy::readSettings(QSettings& qs, const QString& prefix)
{
    bool rc = true;
    QString skey;

    for (int i = 0; i < m_cmds.count(); ++i)
    {
        SimCommand  *cmd = m_cmds.at(i);

        skey = tr("%1/keymap/c%2/").arg(prefix).arg(static_cast<int>(cmd->command()));

        int key;
        bool ok;

        // Read the key.
        ok = qs.contains(skey + "key");
        key = qs.value(skey + "key", 0).toInt();

//        if (ok)
//            cmd->setKey(key);
//        else
//            rc = false;

        // Read the alternate key.
        ok = qs.contains(skey + "alt");
        key = qs.value(skey + "alt", 0).toInt();

//        if (ok)
//            cmd->setAlternateKey(key);
//        else
//            rc = false;
    }

    return rc;

}

bool SimCommandProxy::writeSettings(QSettings& qs, const QString& prefix)
{
    bool rc = true;
    QString skey;

    for (int i = 0; i < m_cmds.count(); ++i)
    {
        SimCommand *cmd = m_cmds.at(i);

        skey = tr("%1/keymap/c%2/").arg(prefix).arg(static_cast<int>(cmd->command()));

        // Write the key.
//        qs.setValue(skey + "key", cmd->key());

//        // Write the alternate key.
//        qs.setValue(skey + "alt", cmd->key());
    }

    return rc;
}

void SimCommandProxy::clearKeys()
{
    for (int i = 0; i < m_cmds.count(); ++i)
        m_cmds.at(i)->setKey(0);
}

void SimCommandProxy::clearAlternateKeys()
{
    for (int i = 0; i < m_cmds.count(); ++i)
        m_cmds.at(i)->setAlternateKey(0);
}

SimCommand *SimCommandProxy::boundTo(int key) const
{
    for (int i = 0; i < m_cmds.count(); ++i)
    {
        SimCommand *cmd = m_cmds.at(i);

        if (cmd->key() == key || cmd->alternateKey() == key)
            return cmd;
    }

    return 0;
}

SimCommand *SimCommandProxy::find(SimSci::Command command) const
{
    for (int i = 0; i < m_cmds.count(); ++i)
    {
        SimCommand *cmd = m_cmds.at(i);

        if (cmd->command() == command)
            return cmd;
    }

    return 0;
}

void SimCommandProxy::init()
{
    struct sciCmd {
        SimSci::Command cmd;
        int key;
        int altkey;
        QString desc;
    };

    static struct sciCmd cmdTable[] = {
        {
            SimSci::CMD_LineDown,
            Qt::Key_Down,
#if defined(USING_OSX_KEYS)
            Qt::Key_N | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Move down one line")
        },
        {
            SimSci::CMD_LineDownExtend,
            Qt::Key_Down | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_N | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Extend selection down one line")
        },
        {
            SimSci::CMD_LineDownRectExtend,
            Qt::Key_Down | Qt::ALT | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_N | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection down one line")
        },
        {
            SimSci::CMD_LineScrollDown,
            Qt::Key_Down | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Scroll view down one line")
        },
        {
            SimSci::CMD_LineUp,
            Qt::Key_Up,
#if defined(USING_OSX_KEYS)
            Qt::Key_P | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Move up one line")
        },
        {
            SimSci::CMD_LineUpExtend,
            Qt::Key_Up | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_P | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Extend selection up one line")
        },
        {
            SimSci::CMD_LineUpRectExtend,
            Qt::Key_Up | Qt::ALT | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_P | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection up one line")
        },
        {
            SimSci::CMD_LineScrollUp,
            Qt::Key_Up | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Scroll view up one line")
        },
        {
            SimSci::CMD_ScrollToStart,
#if defined(USING_OSX_KEYS)
            Qt::Key_Home,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Scroll to start of document")
        },
        {
            SimSci::CMD_ScrollToEnd,
#if defined(USING_OSX_KEYS)
            Qt::Key_End,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Scroll to end of document")
        },
        {
            SimSci::CMD_VerticalCentreCaret,
#if defined(USING_OSX_KEYS)
            Qt::Key_L | Qt::META,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Scroll vertically to centre current line")
        },
        {
            SimSci::CMD_ParaDown,
            Qt::Key_BracketRight | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move down one paragraph")
        },
        {
            SimSci::CMD_ParaDownExtend,
            Qt::Key_BracketRight | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection down one paragraph")
        },
        {
            SimSci::CMD_ParaUp,
            Qt::Key_BracketLeft | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move up one paragraph")
        },
        {
            SimSci::CMD_ParaUpExtend,
            Qt::Key_BracketLeft | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection up one paragraph")
        },
        {
            SimSci::CMD_CharLeft,
            Qt::Key_Left,
#if defined(USING_OSX_KEYS)
            Qt::Key_B | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Move left one character")
        },
        {
            SimSci::CMD_CharLeftExtend,
            Qt::Key_Left | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_B | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection left one character")
        },
        {
            SimSci::CMD_CharLeftRectExtend,
            Qt::Key_Left | Qt::ALT | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_B | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection left one character")
        },
        {
            SimSci::CMD_CharRight,
            Qt::Key_Right,
#if defined(USING_OSX_KEYS)
            Qt::Key_F | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Move right one character")
        },
        {
            SimSci::CMD_CharRightExtend,
            Qt::Key_Right | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_F | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection right one character")
        },
        {
            SimSci::CMD_CharRightRectExtend,
            Qt::Key_Right | Qt::ALT | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_F | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection right one character")
        },
        {
            SimSci::CMD_WordLeft,
#if defined(USING_OSX_KEYS)
            Qt::Key_Left | Qt::ALT,
#else
            Qt::Key_Left | Qt::CTRL,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move left one word")
        },
        {
            SimSci::CMD_WordLeftExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Left | Qt::ALT | Qt::SHIFT,
#else
            Qt::Key_Left | Qt::CTRL | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Extend selection left one word")
        },
        {
            SimSci::CMD_WordRight,
#if defined(USING_OSX_KEYS)
            0,
#else
            Qt::Key_Right | Qt::CTRL,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move right one word")
        },
        {
            SimSci::CMD_WordRightExtend,
            Qt::Key_Right | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Extend selection right one word")
        },
        {
            SimSci::CMD_WordLeftEnd,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to end of previous word")
        },
        {
            SimSci::CMD_WordLeftEndExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of previous word")
        },
        {
            SimSci::CMD_WordRightEnd,
#if defined(USING_OSX_KEYS)
            Qt::Key_Right | Qt::ALT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to end of next word")
        },
        {
            SimSci::CMD_WordRightEndExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Right | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of next word")
        },
        {
            SimSci::CMD_WordPartLeft,
            Qt::Key_Slash | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move left one word part")
        },
        {
            SimSci::CMD_WordPartLeftExtend,
            Qt::Key_Slash | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection left one word part")
        },
        {
            SimSci::CMD_WordPartRight,
            Qt::Key_Backslash | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move right one word part")
        },
        {
            SimSci::CMD_WordPartRightExtend,
            Qt::Key_Backslash | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection right one word part")
        },
        {
            SimSci::CMD_Home,
#if defined(USING_OSX_KEYS)
            Qt::Key_A | Qt::META,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to start of document line")
        },
        {
            SimSci::CMD_HomeExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_A | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to start of document line")
        },
        {
            SimSci::CMD_HomeRectExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_A | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection to start of document line")
        },
        {
            SimSci::CMD_HomeDisplay,
#if defined(USING_OSX_KEYS)
            Qt::Key_Left | Qt::CTRL,
#else
            Qt::Key_Home | Qt::ALT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to start of display line")
        },
        {
            SimSci::CMD_HomeDisplayExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Left | Qt::CTRL | Qt::SHIFT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to start of display line")
        },
        {
            SimSci::CMD_HomeWrap,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Move to start of display or document line")
        },
        {
            SimSci::CMD_HomeWrapExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to start of display or document line")
        },
        {
            SimSci::CMD_VCHome,
#if defined(USING_OSX_KEYS)
            0,
#else
            Qt::Key_Home,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                "Move to first visible character in document line")
        },
        {
            SimSci::CMD_VCHomeExtend,
#if defined(USING_OSX_KEYS)
            0,
#else
            Qt::Key_Home | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                "Extend selection to first visible character in document line")
        },
        {
            SimSci::CMD_VCHomeRectExtend,
#if defined(USING_OSX_KEYS)
            0,
#else
            Qt::Key_Home | Qt::ALT | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                "Extend rectangular selection to first visible character in document line")
        },
        {
            SimSci::CMD_VCHomeWrap,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Move to first visible character of display in document line")
        },
        {
            SimSci::CMD_VCHomeWrapExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to first visible character in display or document line")
        },
        {
            SimSci::CMD_LineEnd,
#if defined(USING_OSX_KEYS)
            Qt::Key_E | Qt::META,
#else
            Qt::Key_End,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to end of document line")
        },
        {
            SimSci::CMD_LineEndExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_E | Qt::META | Qt::SHIFT,
#else
            Qt::Key_End | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of document line")
        },
        {
            SimSci::CMD_LineEndRectExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_E | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            Qt::Key_End | Qt::ALT | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection to end of document line")
        },
        {
            SimSci::CMD_LineEndDisplay,
#if defined(USING_OSX_KEYS)
            Qt::Key_Right | Qt::CTRL,
#else
            Qt::Key_End | Qt::ALT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to end of display line")
        },
        {
            SimSci::CMD_LineEndDisplayExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Right | Qt::CTRL | Qt::SHIFT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of display line")
        },
        {
            SimSci::CMD_LineEndWrap,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Move to end of display or document line")
        },
        {
            SimSci::CMD_LineEndWrapExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of display or document line")
        },
        {
            SimSci::CMD_DocumentStart,
#if defined(USING_OSX_KEYS)
            Qt::Key_Up | Qt::CTRL,
#else
            Qt::Key_Home | Qt::CTRL,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to start of document")
        },
        {
            SimSci::CMD_DocumentStartExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Up | Qt::CTRL | Qt::SHIFT,
#else
            Qt::Key_Home | Qt::CTRL | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to start of document")
        },
        {
            SimSci::CMD_DocumentEnd,
#if defined(USING_OSX_KEYS)
            Qt::Key_Down | Qt::CTRL,
#else
            Qt::Key_End | Qt::CTRL,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move to end of document")
        },
        {
            SimSci::CMD_DocumentEndExtend,
#if defined(USING_OSX_KEYS)
            Qt::Key_Down | Qt::CTRL | Qt::SHIFT,
#else
            Qt::Key_End | Qt::CTRL | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend selection to end of document")
        },
        {
            SimSci::CMD_PageUp,
            Qt::Key_PageUp,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move up one page")
        },
        {
            SimSci::CMD_PageUpExtend,
            Qt::Key_PageUp | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Extend selection up one page")
        },
        {
            SimSci::CMD_PageUpRectExtend,
            Qt::Key_PageUp | Qt::ALT | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection up one page")
        },
        {
            SimSci::CMD_PageDown,
            Qt::Key_PageDown,
#if defined(USING_OSX_KEYS)
            Qt::Key_V | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Move down one page")
        },
        {
            SimSci::CMD_PageDownExtend,
            Qt::Key_PageDown | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_V | Qt::META | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Extend selection down one page")
        },
        {
            SimSci::CMD_PageDownRectExtend,
            Qt::Key_PageDown | Qt::ALT | Qt::SHIFT,
#if defined(USING_OSX_KEYS)
            Qt::Key_V | Qt::META | Qt::ALT | Qt::SHIFT,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci",
                    "Extend rectangular selection down one page")
        },
        {
            SimSci::CMD_StutteredPageUp,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Stuttered move up one page")
        },
        {
            SimSci::CMD_StutteredPageUpExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Stuttered extend selection up one page")
        },
        {
            SimSci::CMD_StutteredPageDown,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Stuttered move down one page")
        },
        {
            SimSci::CMD_StutteredPageDownExtend,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Stuttered extend selection down one page")
        },
        {
            SimSci::CMD_Delete,
            Qt::Key_Delete,
#if defined(USING_OSX_KEYS)
            Qt::Key_D | Qt::META,
#else
            0,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Delete current character")
        },
        {
            SimSci::CMD_DeleteBack,
            Qt::Key_Backspace,
#if defined(USING_OSX_KEYS)
            Qt::Key_H | Qt::META,
#else
            Qt::Key_Backspace | Qt::SHIFT,
#endif
            QT_TRANSLATE_NOOP("SimSci", "Delete previous character")
        },
        {
            SimSci::CMD_DeleteBackNotLine,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                "Delete previous character if not at start of line")
        },
        {
            SimSci::CMD_DeleteWordLeft,
            Qt::Key_Backspace | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Delete word to left")
        },
        {
            SimSci::CMD_DeleteWordRight,
            Qt::Key_Delete | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Delete word to right")
        },
        {
            SimSci::CMD_DeleteWordRightEnd,
#if defined(USING_OSX_KEYS)
            Qt::Key_Delete | Qt::ALT,
#else
            0,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Delete right to end of next word")
        },
        {
            SimSci::CMD_DeleteLineLeft,
            Qt::Key_Backspace | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Delete line to left")
        },
        {
            SimSci::CMD_DeleteLineRight,
#if defined(USING_OSX_KEYS)
            Qt::Key_K | Qt::META,
#else
            Qt::Key_Delete | Qt::CTRL | Qt::SHIFT,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Delete line to right")
        },
        {
            SimSci::CMD_LineDelete,
            Qt::Key_L | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Delete current line")
        },
        {
            SimSci::CMD_LineCut,
            Qt::Key_L | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Cut current line")
        },
        {
            SimSci::CMD_LineCopy,
            Qt::Key_T | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Copy current line")
        },
        {
            SimSci::CMD_LineTranspose,
            Qt::Key_T | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Transpose current and previous lines")
        },
        {
            SimSci::CMD_LineDuplicate,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Duplicate the current line")
        },
        {
            SimSci::CMD_SelectAll,
            Qt::Key_A | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Select all")
        },
        {
            SimSci::CMD_MoveSelectedLinesUp,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Move selected lines up one line")
        },
        {
            SimSci::CMD_MoveSelectedLinesDown,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci",
                    "Move selected lines down one line")
        },
        {
            SimSci::CMD_SelectionDuplicate,
            Qt::Key_D | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Duplicate selection")
        },
        {
            SimSci::CMD_SelectionLowerCase,
            Qt::Key_U | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Convert selection to lower case")
        },
        {
            SimSci::CMD_SelectionUpperCase,
            Qt::Key_U | Qt::CTRL | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Convert selection to upper case")
        },
        {
            SimSci::CMD_SelectionCut,
            Qt::Key_X | Qt::CTRL,
            Qt::Key_Delete | Qt::SHIFT,
            QT_TRANSLATE_NOOP("SimSci", "Cut selection")
        },
        {
            SimSci::CMD_SelectionCopy,
            Qt::Key_C | Qt::CTRL,
            Qt::Key_Insert | Qt::CTRL,
            QT_TRANSLATE_NOOP("SimSci", "Copy selection")
        },
        {
            SimSci::CMD_Paste,
            Qt::Key_V | Qt::CTRL,
            Qt::Key_Insert | Qt::SHIFT,
            QT_TRANSLATE_NOOP("SimSci", "Paste")
        },
        {
            SimSci::CMD_EditToggleOvertype,
            Qt::Key_Insert,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Toggle insert/overtype")
        },
        {
            SimSci::CMD_Newline,
            Qt::Key_Return,
            Qt::Key_Return | Qt::SHIFT,
            QT_TRANSLATE_NOOP("SimSci", "Insert newline")
        },
        {
            SimSci::CMD_Formfeed,
            0,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Formfeed")
        },
        {
            SimSci::CMD_Tab,
            Qt::Key_Tab,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Indent one level")
        },
        {
            SimSci::CMD_Backtab,
            Qt::Key_Tab | Qt::SHIFT,
            0,
            QT_TRANSLATE_NOOP("SimSci", "De-indent one level")
        },
        {
            SimSci::CMD_Cancel,
            Qt::Key_Escape,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Cancel")
        },
        {
            SimSci::CMD_Undo,
            Qt::Key_Z | Qt::CTRL,
            Qt::Key_Backspace | Qt::ALT,
            QT_TRANSLATE_NOOP("SimSci", "Undo last command")
        },
        {
            SimSci::CMD_Redo,
#if defined(USING_OSX_KEYS)
            Qt::Key_Z | Qt::CTRL | Qt::SHIFT,
#else
            Qt::Key_Y | Qt::CTRL,
#endif
            0,
            QT_TRANSLATE_NOOP("SimSci", "Redo last command")
        },
        {
            SimSci::CMD_ZoomIn,
            Qt::Key_Plus | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Zoom in")
        },
        {
            SimSci::CMD_ZoomOut,
            Qt::Key_Minus | Qt::CTRL,
            0,
            QT_TRANSLATE_NOOP("SimSci", "Zoom out")
        },
    };
    m_scintilla->clearAllCmdKeys();

    // By default control characters don't do anything (rather than insert the
    // control character into the text).
    for (int k = 'A'; k <= 'Z'; ++k)
        m_scintilla->assignCmdKey(k + SCMOD_CTRL << 16, SCI_NULL);

    for (int i = 0; i < sizeof (cmdTable) / sizeof (cmdTable[0]); ++i)
        m_cmds.append(new SimCommand(cmdTable[i].cmd, cmdTable[i].key,cmdTable[i].altkey, cmdTable[i].desc, m_scintilla));
}

void SimCommandProxy::clear()
{
    qDeleteAll(m_cmds);
    m_cmds.clear();
}
