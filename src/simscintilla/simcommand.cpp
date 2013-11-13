#include "simscintilla.h"
#include "simcommand.h"

static int convertQt2Sci(int key)
{
    // Convert the modifiers.
    int sci_mod = 0;

    if (key & Qt::SHIFT)
        sci_mod |= SCMOD_SHIFT;

    if (key & Qt::CTRL)
        sci_mod |= SCMOD_CTRL;

    if (key & Qt::ALT)
        sci_mod |= SCMOD_ALT;

    if (key & Qt::META)
        sci_mod |= SCMOD_META;

    key &= ~Qt::MODIFIER_MASK;

    // Convert the key.
    int sci_key;

    if (key > 0x7f)
        switch (key)
        {
        case Qt::Key_Down:
            sci_key = SCK_DOWN;
            break;

        case Qt::Key_Up:
            sci_key = SCK_UP;
            break;

        case Qt::Key_Left:
            sci_key = SCK_LEFT;
            break;

        case Qt::Key_Right:
            sci_key = SCK_RIGHT;
            break;

        case Qt::Key_Home:
            sci_key = SCK_HOME;
            break;

        case Qt::Key_End:
            sci_key = SCK_END;
            break;

        case Qt::Key_PageUp:
            sci_key = SCK_PRIOR;
            break;

        case Qt::Key_PageDown:
            sci_key = SCK_NEXT;
            break;

        case Qt::Key_Delete:
            sci_key = SCK_DELETE;
            break;

        case Qt::Key_Insert:
            sci_key = SCK_INSERT;
            break;

        case Qt::Key_Escape:
            sci_key = SCK_ESCAPE;
            break;

        case Qt::Key_Backspace:
            sci_key = SCK_BACK;
            break;

        case Qt::Key_Tab:
            sci_key = SCK_TAB;
            break;

        case Qt::Key_Return:
            sci_key = SCK_RETURN;
            break;

        default:
            sci_key = 0;
        }
    else
        sci_key = key;

    if (sci_key)
        sci_key |= (sci_mod << 16);

    return sci_key;
}

struct SimCommandPrivate
{
    SimCommandPrivate(SimSci::Command cmd_, int key, int altKey, const QString& desc_, SimScintilla* scintilla_):
        cmd(cmd_), desc(desc_), qKey(key), qAltKey(altKey), scintilla(scintilla_) {

        sciKey = convertQt2Sci(qKey);
        if (sciKey) {
            scintilla_->assignCmdKey(sciKey, cmd_);
        }

        sciAltKey = convertQt2Sci(qAltKey);
        if (sciAltKey) {
            scintilla_->assignCmdKey(sciAltKey, cmd_);
        }
    }

    void execute();
    void bindKey(int key,int &qk,int &scik);
    void bindKey(int key);
    void bindAltKey(int key);
    SimSci::Command cmd;
    QString desc;
    int qKey;
    int sciKey;
    int qAltKey;
    int sciAltKey;
    SimScintilla* scintilla;
};

void SimCommandPrivate::execute()
{
    scintilla->send(cmd);
}

void SimCommandPrivate::bindKey(int key, int &qk, int &scik)
{
    int newSciKey;

    // Ignore if it is invalid, allowing for the fact that we might be
    // unbinding it.
    if (key) {
        newSciKey = convertQt2Sci(key);

        if (!newSciKey)
            return;
    } else
        newSciKey = 0;

    if (scik)
        scintilla->clearCmdKey(scik);

    qk = key;
    scik = newSciKey;

    if (scik)
        scintilla->assignCmdKey(scik, cmd);
}

void SimCommandPrivate::bindKey(int key)
{
    bindKey(key,qKey,sciKey);
}

void SimCommandPrivate::bindAltKey(int key)
{
    bindKey(key,qAltKey,sciAltKey);
}

SimCommand::SimCommand(SimSci::Command cmd, int key, int altKey, const QString& desc, SimScintilla* scintilla)
{
    d = new SimCommandPrivate(cmd, key, altKey, desc, scintilla);
}

SimCommand::~SimCommand()
{
    SAFE_RELEASE(d);
}

void SimCommand::execute()
{
    d->execute();
}

SimSci::Command SimCommand::command()
{
    return d->cmd;
}

void SimCommand::setKey(int key)
{
    d->bindKey(key);
}

int SimCommand::key() const
{
    return d->qKey;
}

void SimCommand::setAlternateKey(int akey)
{
    d->bindAltKey(akey);
}

int SimCommand::alternateKey() const
{
    return d->qAltKey;
}

bool SimCommand::isValidKey(int key)
{
    return convertQt2Sci(key);
}

QString SimCommand::description() const
{
    return d->desc;
}
