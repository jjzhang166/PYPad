#include "simmacro.h"
#include "simscintilla.h"
#include "simscintilladefs.h"

#ifdef USELOG
#include "logutil.h"
#endif

static int FromHex(unsigned char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

    return -1;
}

class SimMacroPrivate
{
public:
    SimMacroPrivate(SimScintilla* scintilla);
    void play();
    bool load(const QString& openfile);
    bool save(const QString& savedfile);

    void addMacro(int message, uptr_t wParam, sptr_t lParam);
    void runMacros();
    struct Macro {
        unsigned int msg;
        unsigned long wParam;
        QByteArray text;
    };

    QList<Macro> macros;
    SimScintilla* scintilla;
    bool record;
};

SimMacroPrivate::SimMacroPrivate(SimScintilla *scintilla_): scintilla(scintilla_), record(false)
{

}

void SimMacroPrivate::play()
{

}

bool SimMacroPrivate::load(const QString &openfile)
{
    // 从文件中反序列化创建宏对象并存储到mCmds中
    QFile inFile(openfile);
    if (!inFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    bool ok = true;
    QString asc = inFile.readAll();
    macros.clear();

    QStringList fields = asc.split(' ');

    int f = 0;

    while (f < fields.size()) {
        Macro cmd;
        unsigned len;

        // Extract the 3 fixed fields.
        if (f + 3 > fields.size()) {
            ok = false;
            break;
        }

        cmd.msg = fields[f++].toUInt(&ok);

        if (!ok)
            break;

        cmd.wParam = fields[f++].toULong(&ok);

        if (!ok)
            break;

        len = fields[f++].toUInt(&ok);

        if (!ok)
            break;

        // Extract any text.
        if (len) {
            if (f + 1 > fields.size()) {
                ok = false;
                break;
            }

            QByteArray ba = fields[f++].toLatin1();
            const char *sp = ba.data();

            if (!sp) {
                ok = false;
                break;
            }

            // Because of historical bugs the length field is unreliable.
            bool embedded_null = false;
            unsigned char ch;

            while ((ch = *sp++) != '\0') {
                if (ch == '"' || ch <= ' ' || ch >= 0x7f) {
                    ok = false;
                    break;
                }

                if (ch == '\\') {
                    int b1, b2;

                    if ((b1 = FromHex(*sp++)) < 0 ||
                        (b2 = FromHex(*sp++)) < 0) {
                        ok = false;
                        break;
                    }

                    ch = (b1 << 4) + b2;
                }

                if (ch == '\0') {
                    // Don't add it now as it may be the terminating '\0'.
                    embedded_null = true;
                } else {
                    if (embedded_null) {
                        // Add the pending embedded '\0'.
                        cmd.text += '\0';
                        embedded_null = false;
                    }

                    cmd.text += ch;
                }
            }

            if (!ok)
                break;

        }

        macros.append(cmd);
    }

    if (!ok)
        macros.clear();

    runMacros();

    return ok;
}

bool SimMacroPrivate::save(const QString &savedfile)
{
    // 把mCmds中的宏对象进行序列化存储到文件中
    QString ms;

    QList<Macro>::const_iterator it;

    for (it = macros.begin(); it != macros.end(); ++it) {
        if (!ms.isEmpty())
            ms += ' ';

        unsigned len = (*it).text.size();
        QString m;

        ms += m.sprintf("%u %lu %u", (*it).msg, (*it).wParam, len);

        if (len) {
            // In Qt v3, if the length is greater than zero then it also
            // includes the '\0', so we need to make sure that Qt v4 writes the
            // '\0'.  That the '\0' is written at all is a bug because
            // QCString::size() is used instead of QCString::length().  We
            // don't fix this so as not to break old macros.  However this is
            // still broken because we have already written the unadjusted
            // length.  So, in summary, the length field should be interpreted
            // as a zero/non-zero value, and the end of the data is either at
            // the next space or the very end of the data.
            ++len;

            ms += ' ';

            const char *cp = (*it).text.data();

            while (len--) {
                unsigned char ch = *cp++;

                if (ch == '\\' || ch == '"' || ch <= ' ' || ch >= 0x7f) {
                    QString buf;

                    ms += buf.sprintf("\\%02x", ch);
                } else {
                    ms += ch;
                }
            }
        }
    }

    QFile outFile(savedfile);
    if (!outFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QByteArray data;
    data.append(ms);
    outFile.write(data);
    outFile.close();

    return true;
}

void SimMacroPrivate::addMacro(int message, uptr_t wParam, sptr_t lParam)
{
    Macro m;

    m.msg = message;
    m.wParam = wParam;

    // Determine commands which need special handling of the parameters.
    switch (message)
    {
    case SimSci::CMD_AddText:
        m.text = QByteArray(reinterpret_cast<const char *>(lParam), wParam);
        break;

    case SimSci::CMD_ReplaceSel:
        if (!macros.isEmpty() && macros.last().msg == SimSci::CMD_ReplaceSel) {
            // This is the command used for ordinary user input so it's a
            // significant space reduction to append it to the previous
            // command.

            macros.last().text.append(reinterpret_cast<const char *>(lParam));
            return;
        }

        /* fall through. */
    case SimSci::CMD_InsertText:
    case SimSci::CMD_AppendText:
    case SimSci::CMD_SearchNext:
    case SimSci::CMD_SearchPrev:
        m.text.append(reinterpret_cast<const char *>(lParam));
        break;
    }

    macros.append(m);
}

void SimMacroPrivate::runMacros()
{
    if (macros.isEmpty()) {
        return;
    }

    QList<Macro>::const_iterator iter = macros.begin();
    while (iter != macros.end()) {
        scintilla->sends(iter->msg, iter->wParam, iter->text);
        ++iter;
    }
}

/****************************************************/
//! \class SimMacro                                 *|
//! \brief The SimScintilla's Macro class           *|
//!                                                 *|
/****************************************************/
SimMacro::SimMacro(SimScintilla* parent)
{
    d = new SimMacroPrivate(parent);
    connect(parent, SIGNAL(macroRecord(int,uptr_t,sptr_t)), this, SLOT(onMacroRecord(int,uptr_t,sptr_t)));
}

SimMacro::~SimMacro()
{
    SAFE_RELEASE(d);
}

void SimMacro::startRecording()
{
    d->record = true;
}

void SimMacro::stopRecording()
{
    d->record = false;
}

void SimMacro::play()
{
    d->play();
}

bool SimMacro::load(const QString &openfile)
{
    return d->load(openfile);
}

bool SimMacro::save(const QString &savedfile)
{
    return d->save(savedfile);
}

void SimMacro::onMacroRecord(int message, uptr_t wParam, sptr_t lParam)
{
#ifdef USELOG
    LOG_VAR(message);
    LOG_VAR(wParam);
    LOG_VAR(lParam);
#endif
    if (d->record) {
        d->addMacro(message, wParam, lParam);
    }
}
