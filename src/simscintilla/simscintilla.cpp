#include "simscintilla.h"
#include "simdocument.h"
#include "simabstracttextstyle.h"
#include "simstyledtext.h"
#include "simcommandproxy.h"
#include "simapis.h"
#include "simlexerfactory.h"
#include "simmacro.h"
#include "simcommand.h"
#include "logutil.h"
#include "minus.xpm"
#include "plus.xpm"

#include <QFileDialog>
#include <QAction>
#include <QScrollBar>

const char AutoCSeparator = '\x03';
const QString DefaultWordChars = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const int MinZoomFactor = -3;

inline sptr_t SIMSCINTILLA_EXPORT GetValueFromColor(const QColor& col)
{
    sptr_t value = (col.blue() << 16) | (col.green() << 8) | col.red();
    return value;
}

inline QColor SIMSCINTILLA_EXPORT GetColorFromValue(sptr_t value)
{
    int r = value % 256;
    int g = (value / 256) % 256;
    int b = (value / 256 / 256) % 256;
    return QColor(r, g, b);
}

struct SimScintillaPrivate
{
    struct FindState
    {
        enum Status
        {
            Finding,
            FindingInSelection,
            Idle
        };

        FindState() : status(Idle) {}

        Status status;
        QString expr;
        bool wrap;
        bool forward;
        int flags;
        long startpos, startpos_orig;
        long endpos, endpos_orig;
        bool show;
    };

    enum IndentState {
        isNone,
        isKeywordStart,
        isBlockStart,
        isBlockEnd
    };

    SimScintillaPrivate(SimScintilla* qptr): q(qptr), lang(SimSci::Lang_None), autoIndent(false), lineNumberVisible(false),
        modified(false), autoCompletionFillupsEnabled(false), isMousePressed(false), isCtrlPressed(false),
                           textColor(Qt::black), backgroundColor(Qt::white), allocatedMarkers(0),
                           allocatedIndicators(7), wordchars(DefaultWordChars), lexer(nullptr), autoCompletionSource(SimSci::ACS_None),
                           autoCompletionUseSingle(SimSci::ACUS_Never), autoCompletionThreshhold(-1), macro(nullptr)
    {
        cmdProxy = new SimCommandProxy(qptr);
    }
    ~SimScintillaPrivate();
    SimScintilla* q;
    SimSci::Language lang;
    bool autoIndent : 1;
    bool lineNumberVisible : 1;
    bool modified : 1;
    bool autoCompletionFillupsEnabled : 1;
    bool isMousePressed : 1;
    bool isCtrlPressed : 1;
    QColor selectionBgColor;
    QColor selectionFgColor;
    QColor backgroundColor;
    QColor whitespaceBgColor;
    QColor whitespaceFgColor;
    QColor textColor;
    QColor indentationBgColor;
    QColor indentationFgColor;
    QColor calltipsBgColor;
    QColor calltipsFgColor;
    QColor calltipsHiColor;
    QColor marginsBgColor;
    QColor marginsFgColor;
    QFont marginsFont;
    SimDocument document;
    SimSci::CallTipsPosition calltipPos;
    SimSci::CallTipsStyle calltipStyle;
    int nCallTipsPos;
    SimSci::BraceMatch braceMode;
    int maxCallTips;
    int calltipsCursor;
    QList<int> calltipsShifts;
    QStringList calltipsEntries;
    unsigned allocatedMarkers;
    unsigned allocatedIndicators;
    SimAbstractLexer* lexer;
    SimCommandProxy* cmdProxy;
    QString wordchars;

    SimSci::AutoCompletionSource autoCompletionSource;
    SimSci::AutoCompletionUseSingle autoCompletionUseSingle;
    int autoCompletionThreshhold;
    QString autoCompletionFillups;
    QStringList autoCompletionSeparators;

    SimSci::FoldStyle foldStyle;
    int foldMargin;
    static const int sDefaultFoldMarginWidth = 14;

    FindState findState;
    char addedChar;

    SimMacro* macro;

    IndentState getIndentState(int line);
    int findStyledWord(const char *text, int style, const char *words);
};

SimScintillaPrivate::~SimScintillaPrivate()
{
    SAFE_RELEASE(macro);
    SAFE_RELEASE(cmdProxy);
}

SimScintillaPrivate::IndentState SimScintillaPrivate::getIndentState(int line)
{
    IndentState istate;

    // Get the styled text.
    long spos = q->positionFromLine(line);
    long epos = q->positionFromLine(line + 1);

    char *text = new char[(epos - spos + 1) * 2];

    TextRange tr;

    tr.chrg.cpMin = spos;
    tr.chrg.cpMax = epos;
    tr.lpstrText = text;

    q->send(SimSci::CMD_GetStyledText, static_cast<uptr_t>(0), reinterpret_cast<sptr_t>(&tr));

    int style, bstart_off, bend_off;

    // Block start/end takes precedence over keywords.
    const char *bstart_words = q->lexer()->blockStart(&style).toStdString().c_str();
    bstart_off = findStyledWord(text, style, bstart_words);

    const char *bend_words = q->lexer()->blockEnd(&style).toStdString().c_str();
    bend_off = findStyledWord(text, style, bend_words);

    // If there is a block start but no block end characters then ignore it
    // unless the block start is the last significant thing on the line, ie.
    // assume Python-like blocking.
    if (bstart_off >= 0 && !bend_words)
        for (int i = bstart_off * 2; text[i] != '\0'; i += 2)
            if (!QChar(text[i]).isSpace())
                return isNone;

    if (bstart_off > bend_off)
        istate = isBlockStart;
    else if (bend_off > bstart_off)
        istate = isBlockEnd;
    else {
        const char *words = q->lexer()->blockStartKeyword(&style).toStdString().c_str();

        istate = (findStyledWord(text,style,words) >= 0) ? isKeywordStart : isNone;
    }

    delete[] text;

    return istate;

}

int SimScintillaPrivate::findStyledWord(const char *text, int style, const char *words)
{
    if (!words)
        return -1;

    // Find the range of text with the style we are looking for.
    const char *stext;

    for (stext = text; stext[1] != style; stext += 2)
        if (stext[0] == '\0')
            return -1;

    // Move to the last character.
    const char *etext = stext;

    while (etext[2] != '\0')
        etext += 2;

    // Backtrack until we find the style.  There will be one.
    while (etext[1] != style)
        etext -= 2;

    // Look for each word in turn.
    while (words[0] != '\0') {
        // Find the end of the word.
        const char *eword = words;

        while (eword[1] != ' ' && eword[1] != '\0')
            ++eword;

        // Now search the text backwards.
        const char *wp = eword;

        for (const char *tp = etext; tp >= stext; tp -= 2) {
            if (tp[0] != wp[0] || tp[1] != style) {
                // Reset the search.
                wp = eword;
                continue;
            }

            // See if all the word has matched.
            if (wp-- == words)
                return ((tp - text) / 2) + (eword - words) + 1;
        }

        // Move to the start of the next word if there is one.
        words = eword + 1;

        if (words[0] == ' ')
            ++words;
    }

    return -1;
}

SimScintilla::SimScintilla(QWidget *parent) :
    ScintillaEdit(parent)
{
    d = new SimScintillaPrivate(this);
    d->document.display(this, nullptr);

    // 这条语句决定了底层是否会发射dwellXXX相关的信息, 很重要!!!
    ScintillaEdit::setMouseDwellTime(250);

    initStyles();
    setupConns();
}

SimScintilla::~SimScintilla()
{
    d->document.undisplay(this);
    SAFE_RELEASE(d)
}

void SimScintilla::setReadOnly(bool readOnly)
{
    ScintillaEdit::setReadOnly(readOnly);
}

bool SimScintilla::isReadOnly() const
{
    return readOnly();
}

void SimScintilla::setTabWidth(int tabWidth)
{
    ScintillaEdit::setTabWidth(tabWidth);
}

int SimScintilla::tabWidth() const
{
    return ScintillaEdit::tabWidth();
}

void SimScintilla::setText(const QString &text)
{
    bool ro = isReadOnly();
    if (ro) {
        setReadOnly(false);
    }

    ScintillaEdit::send(SimSci::CMD_SetText, static_cast<uptr_t>(0), reinterpret_cast<sptr_t>(convertString2ByteArray(text).constData()));
    ScintillaEdit::send(SimSci::CMD_EmptyUndoBuffer);
    setReadOnly(ro);
    adjustLineNumberMarginWidth();
}

void SimScintilla::append(const QString &text)
{
    appendText(length(), convertString2ByteArray(text));
    adjustLineNumberMarginWidth();
}

QString SimScintilla::text()
{
    QByteArray contentArr = ScintillaEdit::getText(ScintillaEdit::length());
    return QString(contentArr);
}

QString SimScintilla::textFromLine(int line)
{
    return QString(getLine(line));
}

void SimScintilla::insert(const QString& text)
{
    ScintillaEdit::insertText(currentPos(), convertString2ByteArray(text));
}

void SimScintilla::insertAt(int line, int index, const QString& text)
{
    ScintillaEdit::insertText(positionFromLineIndex(line, index), convertString2ByteArray(text));
}

void SimScintilla::replace(const QString &replaceStr)
{
    if (d->findState.status == SimScintillaPrivate::FindState::Idle)
        return;

    long start = ScintillaEdit::send(SimSci::CMD_GetSelectionNStart);
    long orig_len = ScintillaEdit::send(SimSci::CMD_GetSelectionEnd) - start;

    ScintillaEdit::send(SimSci::CMD_TargetFromSelection);

    int cmd = (d->findState.flags & SCFIND_REGEXP) ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;

    long len = sends(cmd, -1, convertString2ByteArray(replaceStr).constData());

    // Reset the selection.
    ScintillaEdit::send(SimSci::CMD_SetSelectionStart, start);
    ScintillaEdit::send(SimSci::CMD_SetSelectionEnd, start + len);

    // Fix the original selection.
    d->findState.endpos_orig += (len - orig_len);

    if (d->findState.forward)
        d->findState.startpos = start + len;
}

void SimScintilla::clear()
{
    setText(QString());
}

void SimScintilla::zoomIn()
{
    ScintillaEdit::zoomIn();
}

void SimScintilla::zoomOut()
{
    if (zoom() < MinZoomFactor) {
        return;
    }
    ScintillaEdit::zoomOut();
}

void SimScintilla::zoomFit()
{
    setZoom(1);
}

void SimScintilla::setZoomFactor(int zoomFactor)
{
    if (zoomFactor < MinZoomFactor) {
        return;
    }
    setZoom(zoomFactor);
}

int SimScintilla::zoomFactor() const
{
    return zoom();
}

void SimScintilla::setWrapVisualFlags(SimSci::WrapVisualFlag endFlag, SimSci::WrapVisualFlag startFlag, int indent)
{
    int flags = SC_WRAPVISUALFLAG_NONE;
    int loc = SC_WRAPVISUALFLAGLOC_DEFAULT;

    switch (endFlag)
    {
    case SimSci::WVF_None:
        flags |= SC_WRAPVISUALFLAG_NONE;
        break;

    case SimSci::WVF_ByText:
        flags |= SC_WRAPVISUALFLAG_END;
        loc |= SC_WRAPVISUALFLAGLOC_END_BY_TEXT;
        break;

    case SimSci::WVF_ByBorder:
        flags |= SC_WRAPVISUALFLAG_END;
        break;

    case SimSci::WVF_InMargin:
        flags |= SC_WRAPVISUALFLAG_MARGIN;
        break;
    }

    switch (startFlag)
    {
    case SimSci::WVF_None:
        flags |= SC_WRAPVISUALFLAG_NONE;
        break;

    case SimSci::WVF_ByText:
        flags |= SC_WRAPVISUALFLAG_START;
        loc |= SC_WRAPVISUALFLAGLOC_START_BY_TEXT;
        break;

    case SimSci::WVF_ByBorder:
        flags |= SC_WRAPVISUALFLAG_START;
        break;

    case SimSci::WVF_InMargin:
        flags |= SC_WRAPVISUALFLAG_MARGIN;
        break;
    }

    ScintillaEdit::send(SimSci::CMD_SetWrapVisualFlags, flags);
    ScintillaEdit::send(SimSci::CMD_SetWrapVisualFlagsLocation, loc);
    ScintillaEdit::send(SimSci::CMD_SetWrapStartIndent, indent);
}

bool SimScintilla::findFirst(const QString &expr, bool re, bool cs, bool wo,
                             bool wrap, bool forward, int line,
                             int index, bool show, bool posix)
{
    return false;
}

bool SimScintilla::findFirstInSelection(const QString &expr, bool re, bool cs,bool wo,
                                        bool forward, bool show, bool posix)
{
    return false;
}

bool SimScintilla::findNext()
{
    return false;
}

void SimScintilla::setWhitespaceVisibility(SimSci::WhitespaceVisibility visibility)
{
    setViewWS(visibility);
}

SimSci::WhitespaceVisibility SimScintilla::whitespaceVisibility() const
{
    return static_cast<SimSci::WhitespaceVisibility>(viewWS());
}

void SimScintilla::setEolVisible(bool visible)
{
    setViewEOL(visible);
}

bool SimScintilla::isEolVisible() const
{
    return viewEOL();
}

void SimScintilla::setEolMode(SimSci::EolMode eol)
{
    ScintillaEdit::setEOLMode(eol);
}

SimSci::EolMode SimScintilla::eolMode() const
{
    return static_cast<SimSci::EolMode>(eOLMode());
}

void SimScintilla::convertEols(SimSci::EolMode eol)
{
    ScintillaEdit::convertEOLs(eol);
}

void SimScintilla::setSelectionToEol(bool fill)
{
    setSelEOLFilled(fill);
}

bool SimScintilla::isSelectionToEol() const
{
    return selEOLFilled();
}

void SimScintilla::setLanguage(SimSci::Language lang)
{
    if (d->lang != lang) {
        d->lang = lang;
        SimAbstractLexer* lexer = SimLexerFactory::createLexer(lang, this);
        setLexer(lexer);
    }
}

SimSci::Language SimScintilla::language() const
{
    return d->lang;
}

void SimScintilla::setWrapMode(SimSci::Wrap wrap)
{
    ScintillaEdit::setWrapMode(wrap);
}

SimSci::Wrap SimScintilla::wrapMode() const
{
    return static_cast<SimSci::Wrap>(wrapMode());
}

void SimScintilla::setWrapIndentMode(SimSci::WrapIndent wi)
{
    ScintillaEdit::setWrapIndentMode(wi);
}

SimSci::WrapIndent SimScintilla::wrapIndentMode() const
{
    return static_cast<SimSci::WrapIndent>(ScintillaEdit::wrapIndentMode());
}

void SimScintilla::setCursor(SimSci::Cursor cursor)
{
    ScintillaEdit::setCursor(cursor);
}

SimSci::Cursor SimScintilla::cursor() const
{
    return static_cast<SimSci::Cursor>(cursor());
}

void SimScintilla::setCacheMode(SimSci::Cache cache)
{
    setLayoutCache(cache);
}

SimSci::Cache SimScintilla::cacheMode() const
{
    return static_cast<SimSci::Cache>(layoutCache());
}

void SimScintilla::setCaretStyle(SimSci::CaretStyle style)
{
    ScintillaEdit::setCaretStyle(style);
}

SimSci::CaretStyle SimScintilla::caretStyle() const
{
    return static_cast<SimSci::CaretStyle>(caretStyle());
}

void SimScintilla::setAnnotationVisible(bool visible)
{
    ScintillaEdit::annotationSetVisible(visible);
}

bool SimScintilla::isAnnotationVisible() const
{
    return ScintillaEdit::annotationVisible();
}

void SimScintilla::setAnnotationStyle(int line, SimSci::AnnotationStyle style)
{
    annotationSetStyle(line, style);
}

SimSci::AnnotationStyle SimScintilla::annotationStyle(int line) const
{
    return static_cast<SimSci::AnnotationStyle>(annotationStyle(line));
}

void SimScintilla::setAnnotationText(int line, const QString& text)
{
    annotationSetText(line, convertString2ByteArray(text));
}

void SimScintilla::setAnnotationText(int line, const QList<SimStyledText> &text)
{
    TextStylePair textAndStyle = styleText(text, annotationStyleOffset());

    ScintillaEdit::annotationSetText(line, textAndStyle.first.constData());
    ScintillaEdit::annotationSetStyles(line, textAndStyle.second.constData());
}

QString SimScintilla::annotationText(int line) const
{
    return QString(annotationText(line));
}

void SimScintilla::setAnnotationText(int line, const QString& text, SimSci::AnnotationStyle style)
{
    ScintillaEdit::annotationSetStyle(line, style - ScintillaEdit::annotationStyleOffset());
    ScintillaEdit::annotationSetText(line, convertString2ByteArray(text));
}

void SimScintilla::setAnnotationText(int line, const QString& text, const SimAbstractTextStyle& style)
{
    setAnnotationText(line, text, static_cast<SimSci::AnnotationStyle>(style.style()));
    style.apply(this);
}

void SimScintilla::setAnnotationText(int line, const SimStyledText& text)
{
    setAnnotationText(line, text.text(), text.style());
    text.apply(this);
}

void SimScintilla::clearAnnotations(int line)
{
    if (line < 0) {
        annotationClearAll();
        return;
    }
    annotationSetText(line,"");
}

void SimScintilla::setTechnology(SimSci::Technology tech)
{
    ScintillaEdit::setTechnology(tech);
}

SimSci::Technology SimScintilla::technology() const
{
    return static_cast<SimSci::Technology>(ScintillaEdit::technology());
}

void SimScintilla::setEdgeMode(SimSci::Edge edge)
{
    ScintillaEdit::setEdgeMode(edge);
}

SimSci::Edge SimScintilla::edgeMode() const
{
    return static_cast<SimSci::Edge>(ScintillaEdit::edgeMode());
}

void SimScintilla::setEdgeColumn(int column)
{
    ScintillaEdit::setEdgeColumn(column);
}

int SimScintilla::edgeColumn() const
{
    return ScintillaEdit::edgeColumn();
}

void SimScintilla::setEdgeColor(const QColor& clr)
{
    ScintillaEdit::setEdgeColour(GetValueFromColor(clr));
}

QColor SimScintilla::edgeColor() const
{
    return ScintillaEdit::edgeColour();
}

void SimScintilla::setEnableMultiSelection(bool enable)
{
    clearSelections();
    setMultipleSelection(enable);
}

bool SimScintilla::isMultiSelectionEnabled() const
{
    return multipleSelection();
}

void SimScintilla::setLineNumberVisible(bool visible)
{
    d->lineNumberVisible = visible;
    ScintillaEdit::setMarginTypeN(1, visible ? SC_MARGIN_NUMBER : SC_MARGIN_SYMBOL);
    adjustLineNumberMarginWidth();
    notifyMarginWidthChanged();
}

bool SimScintilla::isLineNumberVisible() const
{
    return d->lineNumberVisible;
}

void SimScintilla::setAutoIndent(bool autoIndent)
{
    d->autoIndent = autoIndent;
}

bool SimScintilla::isAutoIndent() const
{
    return d->autoIndent;
}

void SimScintilla::setOverwriteMode(bool overwrite)
{
    setOvertype(overwrite);
}

bool SimScintilla::isOverwriteMode() const
{
    return overtype();
}

bool SimScintilla::isRedoEnabled()
{
    return canRedo();
}

bool SimScintilla::isUndoEnalbed()
{
    return canUndo();
}

bool SimScintilla::isModified()
{
    return modify();
}

int SimScintilla::lineCount() const
{
    return ScintillaEdit::lineCount();
}

int SimScintilla::lineAt(const QPoint &point)
{
    //    return ScintillaEdit::lineScrollUp()
    return 0;
}

int SimScintilla::lineLength(int line)
{
    return ScintillaEdit::lineLength(line);
}

int SimScintilla::length() const {
    return ScintillaEdit::length();
}

QString SimScintilla::selectedText()
{
    return QString(ScintillaEdit::getSelText());
}

void SimScintilla::indent(int line)
{
    ScintillaEdit::setLineIndentation(line, lineIndentation(line) + indentationWidth());
}

void SimScintilla::unindent(int line)
{
    int lineIndent = lineIndentation(line);
    lineIndent -= ScintillaEdit::indent();
    if (lineIndent < 0) {
        lineIndent = 0;
    }
    setLineIndentation(line, lineIndent);
}

int SimScintilla::textHeight(int line)
{
    return ScintillaEdit::textHeight(line);
}

void SimScintilla::setSelectionBackgroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }

    ScintillaEdit::send(SCI_SETSELBACK, 1, GetValueFromColor(color));
    ScintillaEdit::send(SCI_SETSELFORE, static_cast<uptr_t>(color.alpha()), static_cast<sptr_t>(0));
    d->selectionBgColor = color;
}

QColor SimScintilla::selectionBackgroundColor() const
{
    return d->selectionBgColor;
}

void SimScintilla::resetSelectionBackgroundColor()
{
    ScintillaEdit::send(SCI_SETSELALPHA, static_cast<uptr_t>(SC_ALPHA_NOALPHA),static_cast<sptr_t>(0));
}

void SimScintilla::setSelectionForegroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    ScintillaEdit::send(SCI_SETSELFORE, 1, GetValueFromColor(color));
    d->selectionFgColor = color;
}

QColor SimScintilla::selectionForegroundColor() const
{
    return d->selectionFgColor;
}

void SimScintilla::resetSelectionForegroundColor()
{
    ScintillaEdit::send(SCI_SETSELBACK, 0UL);
}

void SimScintilla::setBackgroundColor(const QColor &clr)
{
    d->backgroundColor = clr;
    ScintillaEdit::send(SCI_STYLESETBACK, 0, GetValueFromColor(clr));
    ScintillaEdit::send(SCI_STYLESETBACK, STYLE_DEFAULT, GetValueFromColor(clr));
}

QColor SimScintilla::backgroundColor() const
{
    return d->backgroundColor;
}

void SimScintilla::setTextColor(const QColor &color)
{
    if (!color.isValid()) {
        return;
    }
    d->textColor = color;
    ScintillaEdit::send(SimSci::CMD_StyleSetFore, 0, GetValueFromColor(color));
}

QColor SimScintilla::textColor() const
{
    return d->textColor;
}

void SimScintilla::setModified(bool m)
{
    d->modified = m;
    if (m) {
        ScintillaEdit::send(SimSci::CMD_SetSavePoint);
    }
}

bool SimScintilla::isModified() const
{
    return d->modified;
}

void SimScintilla::setUtf8(bool u) {
    setAttribute(Qt::WA_InputMethodEnabled, u);
    ScintillaEdit::send(SimSci::CMD_SetCodePage, (u ? SC_CP_UTF8 : 0));
}

bool SimScintilla::isUtf8() const
{
    return ScintillaEdit::send(SCI_GETCODEPAGE) == SC_CP_UTF8;
}

void SimScintilla::setFont(const QFont &font)
{

}

QFont SimScintilla::font() const
{
    return QFont();
}

void SimScintilla::removeSelectedText()
{
    ScintillaEdit::send(SimSci::CMD_ReplaceSel, reinterpret_cast<sptr_t>(""));
}

void SimScintilla::replaceSeletectedText(const QString &text)
{
    ScintillaEdit::send(SimSci::CMD_ReplaceSel, reinterpret_cast<sptr_t>(convertString2ByteArray(text).constData()));
}

void SimScintilla::setCursorPosition(int line, int index)
{
    int linePos = positionFromLine(line);
    setCurrentPos(linePos + index);
}

void SimScintilla::cursorPosition(int &line, int &index)
{
    int curPos = currentPos();
    line = lineFromPosition(curPos);
    int linePos = positionFromLine(line);
    int indx = 0;

    // Allow for multi-byte characters.
    while (linePos < curPos) {
        int new_linpos = ScintillaEdit::send(SimSci::CMD_PositionAfter, linePos);

        // If the position hasn't moved then we must be at the end of the text
        // (which implies that the position passed was beyond the end of the
        // text).
        if (new_linpos == linePos)
            break;

        linePos = new_linpos;
        ++indx;
    }
    index = indx;
}

void SimScintilla::lineIndexFromPosition(int pos, int &line, int &index)
{
    int curPos = pos;
    line = lineFromPosition(curPos);
    int linePos = positionFromLine(line);
    int indx = 0;

    // Allow for multi-byte characters.
    while (linePos < curPos) {
        int new_linpos = ScintillaEdit::send(SimSci::CMD_PositionAfter, linePos);

        // If the position hasn't moved then we must be at the end of the text
        // (which implies that the position passed was beyond the end of the
        // text).
        if (new_linpos == linePos)
            break;

        linePos = new_linpos;
        ++indx;
    }
    index = indx;
}

int SimScintilla::positionFromLineIndex(int line, int index)
{
    int pos = positionFromLine(line);

    // Allow for multi-byte characters.
    for(int i = 0; i < index; i++)
        pos = ScintillaEdit::send(SimSci::CMD_PositionAfter, pos);

    return pos;
}

bool SimScintilla::isFirstLineVisible() const
{
    return firstVisibleLine();
}

bool SimScintilla::hasSelectedText()
{
    return !selectedText().isEmpty();
}

void SimScintilla::setLineIndentation(int line, int indentSize)
{
    ScintillaEdit::setLineIndentation(line, indentSize);
}

int SimScintilla::lineIndentation(int line)
{
    return ScintillaEdit::lineIndentation(line);
}

void SimScintilla::setIndentationGuides(bool enable)
{
    ScintillaEdit::setIndentationGuides(enable);
}

bool SimScintilla::isIndentationGuides() const
{
    return indentationGuides();
}

void SimScintilla::setIndentationGuidesBackgroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->indentationBgColor = color;
    ScintillaEdit::send(SimSci::CMD_StyleSetBack, STYLE_INDENTGUIDE, GetValueFromColor(color));
}

QColor SimScintilla::indentationGuidesBackgroundColor() const
{
    return d->indentationBgColor;
}

void SimScintilla::setIndentationGuidesForegroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->indentationFgColor = color;
    ScintillaEdit::send(SimSci::CMD_StyleSetFore, STYLE_INDENTGUIDE, GetValueFromColor(color));
}

QColor SimScintilla::indentationGuidesForegroundColor() const
{
    return d->indentationFgColor;
}

void SimScintilla::setIndentationWidth(int indentWidth)
{
    setIndent(indentWidth);
}

int SimScintilla::indentationWidth() const
{
    return ScintillaEdit::indent();
}

void SimScintilla::setIndentationUseTabs(bool use)
{
    setTabIndents(use);
}

bool SimScintilla::isIndentationUseTabs() const
{
    return tabIndents();
}

void SimScintilla::setFirstVisibleLine(int line)
{
    ScintillaEdit::setFirstVisibleLine(line);
}

int SimScintilla::firstVisibleLine() const
{
    return ScintillaEdit::firstVisibleLine();
}

bool SimScintilla::isRedoAvailable()
{
    return canRedo();
}

bool SimScintilla::isUndoAvailable()
{
    return canUndo();
}

void SimScintilla::setDocument(const SimDocument &doc)
{
    if (d->document.d != doc.d) {
        d->document.undisplay(this);
        d->document.attach(doc);
        d->document.display(this,&doc);
    }
}

SimDocument SimScintilla::document() const
{
    return d->document;
}

void SimScintilla::setWhitespaceBackgroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }

    d->whitespaceBgColor = color;
    ScintillaEdit::send(SimSci::CMD_SetWhitespaceBack, color.isValid(), GetValueFromColor(color));
}

QColor SimScintilla::whitespaceBackgroundColor() const
{
    return d->whitespaceBgColor;
}

void SimScintilla::setWhitespaceForegroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->whitespaceFgColor = color;
    ScintillaEdit::send(SimSci::CMD_SetWhitespaceFore, color.isValid(), GetValueFromColor(color));
}

QColor SimScintilla::whitespaceForegroundColor() const
{
    return d->whitespaceFgColor;
}

void SimScintilla::setTabIndents(bool tabIndents)
{
    ScintillaEdit::setTabIndents(tabIndents);
}

bool SimScintilla::isTabIndents()
{
    return ScintillaEdit::tabIndents();
}

QString SimScintilla::wordAtLineIndex(int line, int index)
{
    return wordAtPosition(positionFromLineIndex(line, index));
}

QString SimScintilla::wordAtPoint(const QPoint &pot)
{
    long closePos = ScintillaEdit::send(SimSci::CMD_PositionFromPointClose, pot.x(), pot.y());
    return wordAtPosition(closePos);
}

QString SimScintilla::wordAtPosition(int pos)
{
    if (pos < 0)
        return QString();

    long startPos = ScintillaEdit::send(SimSci::CMD_WordStartPosition, pos, true);
    long endPos = ScintillaEdit::send(SimSci::CMD_WordEndPosition, pos, true);
    int wordLen = endPos - startPos;

    if (wordLen <= 0)
        return QString();

    char *buf = new char[wordLen + 1];

    TextRange tr;
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    tr.lpstrText = buf;
    ScintillaEdit::send(SimSci::CMD_GetTextRange, static_cast<uptr_t>(0), reinterpret_cast<sptr_t>(&tr));
    QString word = convertText2String(buf);
    delete[] buf;
    return word;
}

void SimScintilla::setExtraAscent(int extraAscent)
{
    ScintillaEdit::setExtraAscent(extraAscent);
}

int SimScintilla::extraAscent() const
{
    return ScintillaEdit::extraAscent();
}

void SimScintilla::setExtraDescent(int extraDescent)
{
    ScintillaEdit::setExtraDescent(extraDescent);
}

int SimScintilla::extraDescent() const
{
    return ScintillaEdit::extraDescent();
}

void SimScintilla::setCallTipsBackgroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->calltipsBgColor = color;
    ScintillaEdit::callTipSetBack(GetValueFromColor(color));
}

QColor SimScintilla::callTipsBackgroundColor() const
{
    return d->calltipsBgColor;
}

void SimScintilla::setCallTipsForegroundColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->calltipsFgColor = color;
    ScintillaEdit::callTipSetFore(GetValueFromColor(color));
}

QColor SimScintilla::callTipsForegroundColor() const
{
    return d->calltipsFgColor;
}

void SimScintilla::setCallTipsHighlightColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    d->calltipsHiColor = color;
    ScintillaEdit::send(SimSci::CMD_CallTipSEtHlt, GetValueFromColor(color));
}

QColor SimScintilla::callTipsHighlightColor() const
{
    return d->calltipsHiColor;
}

void SimScintilla::setCallTipsPosition(SimSci::CallTipsPosition position)
{
    ScintillaEdit::callTipSetPosition(position == SimSci::CTP_AboveText);
    d->calltipPos = position;
}

SimSci::CallTipsPosition SimScintilla::callTipsPosition() const
{
    return d->calltipPos;
}

void SimScintilla::setCallTipsStyle(SimSci::CallTipsStyle style)
{
    d->calltipStyle = style;
}

SimSci::CallTipsStyle SimScintilla::callTipsStyle() const
{
    return d->calltipStyle;
}

void SimScintilla::setCallTipsVisible(int num)
{
    d->maxCallTips = num;
}

int SimScintilla::callTipsVisible() const
{
    return d->maxCallTips;
}

bool SimScintilla::isCallTipsActive()
{
    return callTipActive();
}

void SimScintilla::setCaretForegroundColor(const QColor &col)
{
    setCaretFore(GetValueFromColor(col));
}

void SimScintilla::setCaretLineBackgroundColor(const QColor &col)
{
    setCaretLineBack(GetValueFromColor(col));
}

void SimScintilla::setCaretLineVisible(bool enable)
{
    ScintillaEdit::setCaretLineVisible(enable);
}

void SimScintilla::setCaretWidth(int width)
{
    ScintillaEdit::setCaretWidth(width);
}

void SimScintilla::setSelection(int lineFrom, int indexFrom, int lineTo, int indexTo)
{
    setSelectionStart(positionFromLineIndex(lineFrom, indexFrom));
    setSelectionEnd(positionFromLineIndex(lineTo, indexTo));
}

void SimScintilla::selection(int& lineFrom, int& indexFrom, int& lineTo,int& indexTo) const
{

}

void SimScintilla::ensureCursorVisible()
{
    ScintillaEdit::send(SimSci::CMD_ScrollCaret);
}

void SimScintilla::ensureLineVisible(int line)
{
    ensureVisible(line);
}

void SimScintilla::setMarginsBackgroundColor(const QColor &col)
{
    if (!col.isValid()) {
        return;
    }
    d->marginsBgColor = col;
    changeBackgroundStyle(STYLE_LINENUMBER, col);
}

QColor SimScintilla::marginsBackgroundColor() const
{
    return d->marginsBgColor;
}

void SimScintilla::setMarginsFont(const QFont &f)
{
    d->marginsFont = f;
    changeStyleFont(STYLE_LINENUMBER, f);
}

QFont SimScintilla::marginsFont() const
{
    return d->marginsFont;
}

void SimScintilla::setMarginsForegroundColor(const QColor &col)
{
    if (!col.isValid()) {
        return;
    }
    d->marginsFgColor = col;
    changeForegroundStyle(STYLE_LINENUMBER, col);
}

QColor SimScintilla::marginsForegroundColor() const
{
    return d->marginsFgColor;
}

void SimScintilla::setMarginLineNumbers(int margin, bool lnrs)
{
    ScintillaEdit::send(SimSci::CMD_SetMarginTypeN, margin, lnrs ? SC_MARGIN_NUMBER : SC_MARGIN_SYMBOL);
}

bool SimScintilla::isMarginLineNumbers(int margin)
{
    ScintillaEdit::send(SimSci::CMD_GetMarginTypeN, margin);
    return false;
}

void SimScintilla::setMarginMarkerMask(int margin, int mask)
{
    ScintillaEdit::send(SimSci::CMD_SetMarginMaskN, margin, mask);
}

int SimScintilla::marginMarkerMask(int margin)
{
    return marginMaskN(margin);
}

void SimScintilla::setMarginSensitivity(int margin, bool sens)
{
    setMarginSensitiveN(margin, sens);
}

bool SimScintilla::marginSensitivity(int margin)
{
    return marginSensitiveN(margin);
}

void SimScintilla::setMarginWidth(int margin, int width)
{
    setMarginWidthN(margin, width);
}

void SimScintilla::setMarginWidth(int margin, const QString &s)
{
    int textWidth = fontMetrics().width(s);
    setMarginWidthN(margin, textWidth);
}

int SimScintilla::marginWidth(int margin)
{
    return marginWidthN(margin);
}

void SimScintilla::setMarginType(int margin, SimSci::MarginType type)
{
    setMarginTypeN(margin, type);
}

SimSci::MarginType SimScintilla::marginType(int margin)
{
    return static_cast<SimSci::MarginType>(marginTypeN(margin));
}

void SimScintilla::setMarginOptions(int options)
{
    ScintillaEdit::setMarginOptions(options);
}

int SimScintilla::marginOptions() const
{
    return ScintillaEdit::marginOptions();
}

void SimScintilla::setMarginText(int line, const QString &text, int style)
{
    int styleOffset = ScintillaEdit::send(SimSci::CMD_MarginGetStyleOffset);
    sends(SimSci::CMD_MarginSetText, line, convertString2ByteArray(text));
    ScintillaEdit::send(SimSci::CMD_MarginSetStyle, line, style - styleOffset);
}

void SimScintilla::setMarginText(int line, const QString &text, const SimAbstractTextStyle &style)
{
    setMarginText(line, text, style.style());
}

void SimScintilla::setMarginText(int line, const SimStyledText &text)
{
    setMarginText(line, text.text(), text.style());
}

void SimScintilla::setMarginText(int line, const QList<SimStyledText> &text)
{
    TextStylePair textAndStyle = styleText(text, annotationStyleOffset());
    ScintillaEdit::marginSetText(line, textAndStyle.first.constData());
    ScintillaEdit::marginSetStyles(line, textAndStyle.second.constData());
}

QString SimScintilla::marginText(int line)
{
    return ScintillaEdit::marginText(line);
}

void SimScintilla::clearMarginText(int line)
{
    if (line < 0) {
        marginTextClearAll();
        return;
    } else {
        sends(SimSci::CMD_MarginSetText, line, "");
    }
}

void SimScintilla::read(QIODevice* io)
{
    QByteArray data = io->readAll();
    setText(data);
}

void SimScintilla::write(QIODevice* io)
{
    const QString kText = text();
    if (isUtf8()) {
        io->write(kText.toUtf8());
    } else {
        io->write(kText.toLatin1());
    }
}

void SimScintilla::registerImage(int id, const QPixmap &pm)
{
    ScintillaEdit::send(SimSci::CMD_RegisterImage, id, reinterpret_cast<sptr_t>(&pm));
}

void SimScintilla::registerImage(int id, const QImage &im)
{
    ScintillaEdit::send(SimSci::CMD_RgbImageSetHeight, im.height());
    ScintillaEdit::send(SimSci::CMD_RgbImageSetWidth, im.width());
    ScintillaEdit::send(SimSci::CMD_RegisterRgbaImage, id, reinterpret_cast<sptr_t>(&im));
}

void SimScintilla::clearRegisteredImages()
{
    ScintillaEdit::send(SimSci::CMD_ClearRegisteredImages);
}

void SimScintilla::setFoldMarginColors(const QColor &fore, const QColor &back)
{
    ScintillaEdit::send(SimSci::CMD_SetFoldMarginHiColour, 1, GetValueFromColor(fore));
    ScintillaEdit::send(SimSci::CMD_SetFoldMarginColour, 1, GetValueFromColor(back));
}

void SimScintilla::resetFoldMarginColors()
{
    ScintillaEdit::send(SimSci::CMD_SetFoldMarginHiColour, 1, 0);
    ScintillaEdit::send(SimSci::CMD_SetFoldMarginColour, 1, 0);
}

void SimScintilla::recolor(int start, int end)
{
    ScintillaEdit::colourise(start, end);
}

bool SimScintilla::isUserListActive() const
{
    return ScintillaEdit::send(SimSci::CMD_AutoCActive);
}

void SimScintilla::showUserList(int id, const QStringList &list)
{
    ScintillaEdit::userListShow(id, convertString2ByteArray(list.join('\n')));
}

void SimScintilla::cancelUserList()
{
    ScintillaEdit::send(SimSci::CMD_AutoCCancel);
}

bool SimScintilla::isWordCharacter(char ch) const
{
    return d->wordchars.contains(ch, Qt::CaseInsensitive);
}

QString SimScintilla::wordCharacters() const
{
    return d->wordchars;
}
void SimScintilla::setLexer(SimAbstractLexer *lexer)
{
    detachLexer();
    d->lexer = lexer;

    if (lexer != nullptr) {

#ifdef USELOG
        LOG_VAR(lexer->lang());
#endif
        ScintillaEdit::setLexer(lexer->lang());
        ScintillaEdit::setLexerLanguage(lexer->lexerStr().toStdString().c_str());
        lexer->setEditor(this);

        setLexerKeywords();
        handleLexerSignals(lexer);

        ScintillaEdit::send(SimSci::CMD_SetProperty, reinterpret_cast<uptr_t>("fold"), reinterpret_cast<uptr_t>("1"));
//        ScintillaEdit::send(SimSci::CMD_SetProperty, "fold.html", "1");

        //! 首先设置默认风格
        setLexerStyle(STYLE_DEFAULT);

        //! 然后根据不同的风格应用不同的设置
        applyStyles();

        //! 初始化属性.
        lexer->refreshProperties();

        setAutoCompletionFillupsEnabled(d->autoCompletionFillupsEnabled);
        getLexerProps();

        ScintillaEdit::autoCSetIgnoreCase(!lexer->isCaseSensitive());
        recolor();
        createLexerAPI(lexer);
    } else {
        resetNullLexer();
    }
}

SimAbstractLexer *SimScintilla::lexer() const
{
    return d->lexer;
}

bool SimScintilla::isLexerCaseSensitive() const
{
    if (d->lexer == nullptr) {
        return true;
    } else {
        return d->lexer->isCaseSensitive();
    }
}

void SimScintilla::foldAll(bool children)
{
    recolor();

    int maxLine = lineCount();
    bool expanding = true;

    for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) {
        if (ScintillaEdit::send(SimSci::CMD_GetFoldLevel, lineSeek) & SC_FOLDLEVELHEADERFLAG) {
            expanding = !ScintillaEdit::send(SimSci::CMD_GetFoldExpanded, lineSeek);
            break;
        }
    }

    for (int line = 0; line < maxLine; line++) {
        int level = ScintillaEdit::send(SimSci::CMD_GetFoldLevel, line);

        if (!(level & SC_FOLDLEVELHEADERFLAG))
            continue;

        if (children || (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) {
            if (expanding) {
                ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 1);
                foldExpand(line, true, false, 0, level);
                line--;
            } else {
                int lineMaxSubord = ScintillaEdit::send(SimSci::CMD_GetLastChild, line, -1);

                ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 0L);

                if (lineMaxSubord > line)
                    ScintillaEdit::send(SimSci::CMD_HideLines, line + 1, lineMaxSubord);
            }
        }
    }
}

void SimScintilla::foldLine(int line)
{
    ScintillaEdit::send(SimSci::CMD_ToggleFold, line);
}

void SimScintilla::clearFolds()
{
    recolor();

    int maxLine = lineCount();

    for (int line = 0; line < maxLine; line++) {
        int level = ScintillaEdit::send(SimSci::CMD_GetFoldLevel, line);

        if (level & SC_FOLDLEVELHEADERFLAG) {
            ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 1);
            foldExpand(line, true, false, 0, level);
            line--;
        }
    }
}

void SimScintilla::setContractedFolds(const QList<int> &folds)
{
    for (int i = 0; i < folds.count(); ++i) {
        int line = folds[i];
        int last_line = ScintillaEdit::send(SimSci::CMD_GetLastChild, line, -1);

        ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 0L);
        ScintillaEdit::send(SimSci::CMD_HideLines, line + 1, last_line);
    }
}

QList<int> SimScintilla::contractedFolds() const
{
    QList<int> folds;
    int linenr = 0, fold_line;

    while ((fold_line = ScintillaEdit::send(SimSci::CMD_ContractedFoldNext, linenr)) >= 0) {
        folds.append(fold_line);
        linenr = fold_line + 1;
    }

    return folds;
}

void SimScintilla::setMarkerBackgroundColor(int markerNumber, const QColor &col)
{
    if (markerNumber <= MARKER_MAX) {
        int alpha = col.alpha();

        // An opaque background would make the text invisible.
        if (alpha == 255)
            alpha = SC_ALPHA_NOALPHA;

        if (markerNumber < 0) {
            unsigned am = d->allocatedMarkers;

            for (int m = 0; m <= MARKER_MAX; ++m) {
                if (am & 1) {
                    ScintillaEdit::send(SimSci::CMD_MarkerSetBack, m, GetValueFromColor(col));
                    ScintillaEdit::send(SimSci::CMD_MarkerSetAlpha, m, alpha);
                }

                am >>= 1;
            }
        } else if (d->allocatedMarkers & (1 << markerNumber)) {
            ScintillaEdit::send(SimSci::CMD_MarkerSetBack, markerNumber, GetValueFromColor(col));
            ScintillaEdit::send(SimSci::CMD_MarkerSetAlpha, markerNumber, alpha);
        }
    }

}

void SimScintilla::setMarkerForegroundColor(int markerNumber, const QColor &col)
{
    if (markerNumber <= MARKER_MAX) {
        if (markerNumber < 0) {
            unsigned am = d->allocatedMarkers;

            for (int m = 0; m <= MARKER_MAX; ++m) {
                if (am & 1)
                    ScintillaEdit::send(SimSci::CMD_MarkerSetFore, m, GetValueFromColor(col));

                am >>= 1;
            }
        } else if (d->allocatedMarkers & (1 << markerNumber)) {
            ScintillaEdit::send(SimSci::CMD_MarkerSetFore, markerNumber, GetValueFromColor(col));
        }
    }
}

void SimScintilla::setMatchedBraceBackgroundColor(const QColor &col)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetBack, STYLE_BRACELIGHT, GetValueFromColor(col));
}

void SimScintilla::setMatchedBraceForegroundColor(const QColor &col)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetFore, STYLE_BRACELIGHT, GetValueFromColor(col));
}

void SimScintilla::setMatchedBraceIndicator(int indicatorNumber)
{
    ScintillaEdit::send(SimSci::CMD_BraceHighlightIndicator, 1, indicatorNumber);
}

void SimScintilla::resetMatchedBraceIndicator()
{
    ScintillaEdit::send(SimSci::CMD_BraceHighlightIndicator, 0, 0L);
}

void SimScintilla::setUnmatchedBraceBackgroundColor(const QColor &col)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetBack, STYLE_BRACELIGHT, GetValueFromColor(col));
}

void SimScintilla::setUnmatchedBraceForegroundColor(const QColor &col)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetFore, STYLE_BRACELIGHT, GetValueFromColor(col));
}

void SimScintilla::setUnmatchedBraceIndicator(int indicatorNumber)
{
    ScintillaEdit::send(SimSci::CMD_BraceBadlightIndicator, 1, indicatorNumber);
}

void SimScintilla::resetUnmatchedBraceIndicator()
{
    ScintillaEdit::send(SimSci::CMD_BraceBadlightIndicator, 0, 0L);
}

void SimScintilla::clearIndicatorRange(int lineFrom, int indexFrom, int lineTo,int indexTo, int indicatorNumber)
{
    if (indicatorNumber <= INDIC_MAX) {
        int start = positionFromLineIndex(lineFrom, indexFrom);
        int finish = positionFromLineIndex(lineTo, indexTo);

        // We ignore allocatedIndicators to allow any indicators defined
        // elsewhere (e.g. in lexers) to be set.
        if (indicatorNumber < 0) {
            for (int i = 0; i <= INDIC_MAX; ++i) {
                ScintillaEdit::send(SimSci::CMD_SetIndicatorCurrent, i);
                ScintillaEdit::send(SimSci::CMD_IndicatorClearRange, start, finish - start);
            }
        } else {
            ScintillaEdit::send(SimSci::CMD_SetIndicatorCurrent, indicatorNumber);
            ScintillaEdit::send(SimSci::CMD_IndicatorClearRange, start, finish - start);
        }
    }
}

void SimScintilla::fillIndicatorRange(int lineFrom, int indexFrom, int lineTo,int indexTo, int indicatorNumber)
{
    if (indicatorNumber <= INDIC_MAX) {
        int start = positionFromLineIndex(lineFrom, indexFrom);
        int finish = positionFromLineIndex(lineTo, indexTo);

        // We ignore allocatedIndicators to allow any indicators defined
        // elsewhere (e.g. in lexers) to be set.
        if (indicatorNumber < 0) {
            for (int i = 0; i <= INDIC_MAX; ++i) {
                ScintillaEdit::send(SimSci::CMD_SetIndicatorCurrent, i);
                ScintillaEdit::send(SimSci::CMD_IndicatorFillRange, start, finish - start);
            }
        } else {
            ScintillaEdit::send(SimSci::CMD_SetIndicatorCurrent, indicatorNumber);
            ScintillaEdit::send(SimSci::CMD_IndicatorFillRange, start, finish - start);
        }
    }
}


SimCommandProxy* SimScintilla::standardCommands() const
{
    return d->cmdProxy;
}

int SimScintilla::defineMarker(SimSci::MarkerSymbol sym, int markerNumber)
{
    checkMarker(markerNumber);

    if (markerNumber >= 0)
        ScintillaEdit::send(SimSci::CMD_MarkerDefine, markerNumber, static_cast<long>(sym));

    return markerNumber;
}

int SimScintilla::defineMarker(char ch, int markerNumber)
{
    checkMarker(markerNumber);

    if (markerNumber >= 0)
        ScintillaEdit::send(SimSci::CMD_MarkerDefine, markerNumber, static_cast<long>(SC_MARK_CHARACTER) + ch);

    return markerNumber;
}

int SimScintilla::defineMarker(const QPixmap &pm, int markerNumber)
{
    checkMarker(markerNumber);

    if (markerNumber >= 0)
        ScintillaEdit::send(SimSci::CMD_MarkerDefinePixmap, markerNumber, reinterpret_cast<sptr_t>(&pm));

    return markerNumber;
}

int SimScintilla::defineMarker(const QImage &im, int markerNumber)
{
    checkMarker(markerNumber);

    if (markerNumber >= 0) {
        ScintillaEdit::send(SimSci::CMD_RgbImageSetHeight, im.height());
        ScintillaEdit::send(SimSci::CMD_RgbImageSetWidth, im.width());
        ScintillaEdit::send(SimSci::CMD_MarkerDefineRgbaImage, markerNumber, reinterpret_cast<sptr_t>(&im));
    }

    return markerNumber;
}

int SimScintilla::addMarker(int linenr, int markerNumber)
{
    if (markerNumber < 0 || markerNumber > MARKER_MAX || (d->allocatedMarkers & (1 << markerNumber)) == 0)
        return -1;

    return ScintillaEdit::send(SimSci::CMD_MarkerAdd, linenr, markerNumber);
}

unsigned SimScintilla::markersAtLine(int linenr) const
{
    return ScintillaEdit::send(SimSci::CMD_MarkerGet, linenr);
}

void SimScintilla::deleteMarker(int linenr, int markerNumber)
{
    if (markerNumber <= MARKER_MAX) {
        if (markerNumber < 0) {
            unsigned am = d->allocatedMarkers;

            for (int m = 0; m <= MARKER_MAX; ++m) {
                if (am & 1)
                    ScintillaEdit::send(SimSci::CMD_MarkerDelete, linenr, m);

                am >>= 1;
            }
        }
        else if (d->allocatedMarkers & (1 << markerNumber))
            ScintillaEdit::send(SimSci::CMD_MarkerDelete, linenr, markerNumber);
    }
}

void SimScintilla::deleteAllMarker(int markerNumber)
{
    if (markerNumber <= MARKER_MAX)
    {
        if (markerNumber < 0)
            ScintillaEdit::send(SimSci::CMD_MarkerDeleteAll, -1);
        else if (d->allocatedMarkers & (1 << markerNumber))
            ScintillaEdit::send(SimSci::CMD_MarkerDeleteAll, markerNumber);
    }
}

void SimScintilla::deleteMarkerHandle(int mhandle)
{
    ScintillaEdit::send(SimSci::CMD_MarkerDeleteHandle, mhandle);
}

int SimScintilla::markerLine(int mhandle) const
{
    return ScintillaEdit::send(SimSci::CMD_MarkerLineFromHandle, mhandle);
}

int SimScintilla::findNextMarker(int linenr, unsigned mask) const
{
    return ScintillaEdit::send(SimSci::CMD_MarkerNext, linenr, mask);
}

int SimScintilla::findPreviousMarker(int linenr, unsigned mask) const
{
    return ScintillaEdit::send(SimSci::CMD_MarkerPrevious, linenr, mask);
}

void SimScintilla::moveToMatchingBrace()
{
    gotoMatchingBrace(false);
}

void SimScintilla::selectToMatchingBrace()
{
    gotoMatchingBrace(true);
}

void SimScintilla::setBraceMatching(SimSci::BraceMatch bm)
{
    d->braceMode = bm;
}

SimSci::BraceMatch SimScintilla::braceMatching() const
{
    return d->braceMode;
}

void SimScintilla::setBackspaceUnindents(bool unindent)
{
    ScintillaEdit::setBackSpaceUnIndents(unindent);
}

bool SimScintilla::isBackspaceUnindents() const
{
    return ScintillaEdit::backSpaceUnIndents();
}

void SimScintilla::setAutoCompletionCaseSensitivity(bool cs)
{
    ScintillaEdit::autoCSetIgnoreCase(cs);
}

bool SimScintilla::isAutoCompletionCaseSensitivity() const
{
    return ScintillaEdit::autoCIgnoreCase();
}

void SimScintilla::setAutoCompletionReplaceWord(bool replace)
{
    ScintillaEdit::autoCSetDropRestOfWord(replace);
}

bool SimScintilla::isAutoCompletionReplaceWord() const
{
    return ScintillaEdit::autoCDropRestOfWord();
}

void SimScintilla::setAutoCompletionShowSingle(bool single)
{
    ScintillaEdit::autoCSetChooseSingle(single);
}

bool SimScintilla::isAutoCompletionShowSingle() const
{
    return ScintillaEdit::autoCChooseSingle();
}

void SimScintilla::setAutoCompletionSource(SimSci::AutoCompletionSource source)
{
    d->autoCompletionSource = source;
}

SimSci::AutoCompletionSource SimScintilla::autoCompletionSource() const
{
    return d->autoCompletionSource;
}

void SimScintilla::setAutoCompletionThreshold(int thresh)
{
    d->autoCompletionThreshhold = thresh;
}

int SimScintilla::autoCompletionThreshold() const
{
    return d->autoCompletionThreshhold;
}

void SimScintilla::setAutoCompletionUseSingle(SimSci::AutoCompletionUseSingle single)
{
    d->autoCompletionUseSingle = single;
}

SimSci::AutoCompletionUseSingle SimScintilla::autoCompletionUseSingle() const
{
    return d->autoCompletionUseSingle;
}

void SimScintilla::setAutoCompletionFillupsEnabled(bool enabled)
{
    QString fillups;

    if (!enabled)
        fillups = "";
    else if (d->lexer != nullptr)
        fillups = d->lexer->autoCompletionFillups();
    else
        fillups = convertString2ByteArray(d->autoCompletionFillups);

    sends(SimSci::CMD_AutoCSetFillUps, 0, convertString2ByteArray(fillups));
    d->autoCompletionFillupsEnabled = enabled;
}

bool SimScintilla::isAutoCompletionFillupsEnabled() const
{
    return d->autoCompletionFillupsEnabled;
}

void SimScintilla::setAutoCompletionFillups(const QString& fillups)
{
    d->autoCompletionFillups = fillups;
    setAutoCompletionFillupsEnabled(d->autoCompletionFillupsEnabled);
}

void SimScintilla::setAutoCompletionWordSeparators(const QStringList &separators)
{
    d->autoCompletionSeparators = separators;
}

void SimScintilla::setFolding(SimSci::FoldStyle fold, int margin)
{
    d->foldStyle = fold;
    d->foldMargin = margin;

    if (fold == SimSci::FS_None) {
        ScintillaEdit::send(SimSci::CMD_SetMarginWidthN, margin, 0L);
        return;
    }

    int mask = ScintillaEdit::send(SimSci::CMD_GetModEventMask);
    ScintillaEdit::send(SimSci::CMD_SetModEventMask, mask | SC_MOD_CHANGEFOLD);

    ScintillaEdit::send(SimSci::CMD_SetFoldFlags, SC_FOLDFLAG_LINEAFTER_CONTRACTED);

    ScintillaEdit::send(SimSci::CMD_SetMarginTypeN, margin, (long)SC_MARGIN_SYMBOL);
    ScintillaEdit::send(SimSci::CMD_SetMarginMaskN, margin, SC_MASK_FOLDERS);
    ScintillaEdit::send(SimSci::CMD_SetMarginSensitiveN, margin, 1);

    // Set the marker symbols to use.
    switch (fold) {
    case SimSci::FS_Plain:
        setFoldMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
        setFoldMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS);
        setFoldMarker(SC_MARKNUM_FOLDERSUB);
        setFoldMarker(SC_MARKNUM_FOLDERTAIL);
        setFoldMarker(SC_MARKNUM_FOLDEREND);
        setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
        setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);
        break;

    case SimSci::FS_Circled:
        setFoldMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS);
        setFoldMarker(SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS);
        setFoldMarker(SC_MARKNUM_FOLDERSUB);
        setFoldMarker(SC_MARKNUM_FOLDERTAIL);
        setFoldMarker(SC_MARKNUM_FOLDEREND);
        setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
        setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);
        break;

    case SimSci::FS_Boxed:
        setFoldMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
        setFoldMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
        setFoldMarker(SC_MARKNUM_FOLDERSUB);
        setFoldMarker(SC_MARKNUM_FOLDERTAIL);
        setFoldMarker(SC_MARKNUM_FOLDEREND);
        setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
        setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);
        break;

    case SimSci::FS_CircledTree:
        setFoldMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS);
        setFoldMarker(SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS);
        setFoldMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
        setFoldMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);
        setFoldMarker(SC_MARKNUM_FOLDEREND, SC_MARK_CIRCLEPLUSCONNECTED);
        setFoldMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED);
        setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
        break;

    case SimSci::FS_BoxedTree:
        setFoldMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
        setFoldMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
        setFoldMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
        setFoldMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
        setFoldMarker(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
        setFoldMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
        setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
        break;
    }

    ScintillaEdit::send(SimSci::CMD_SetMarginWidthN, margin, d->sDefaultFoldMarginWidth);
}

SimSci::FoldStyle SimScintilla::folding() const
{
    return d->foldStyle;
}

void SimScintilla::setIndicatorDrawUnder(bool under, int indicatorNumber)
{
    ScintillaEdit::indicSetUnder(indicatorNumber, under);
}

bool SimScintilla::isIndicatorDrawUnder(int indicatorNumber) const
{
    return ScintillaEdit::indicUnder(indicatorNumber);
}

void SimScintilla::setIndicatorForegroundColor(const QColor &col, int indicatorNumber)
{
    if (indicatorNumber <= INDIC_MAX) {
        int alpha = col.alpha();

        // We ignore allocatedIndicators to allow any indicators defined
        // elsewhere (e.g. in lexers) to be set.
        if (indicatorNumber < 0) {
            for (int i = 0; i <= INDIC_MAX; ++i) {
                ScintillaEdit::send(SimSci::CMD_IndicSetFore, i, GetValueFromColor(col));
                ScintillaEdit::send(SimSci::CMD_IndicSetAlpha, i, alpha);
            }
        } else {
            ScintillaEdit::send(SimSci::CMD_IndicSetFore, indicatorNumber, GetValueFromColor(col));
            ScintillaEdit::send(SimSci::CMD_IndicSetAlpha, indicatorNumber, alpha);
        }
    }
}

void SimScintilla::setIndicatorOutlineColor(const QColor &col, int indicatorNumber)
{
    if (indicatorNumber <= INDIC_MAX) {
        int alpha = col.alpha();

        // We ignore allocatedIndicators to allow any indicators defined
        // elsewhere (e.g. in lexers) to be set.
        if (indicatorNumber < 0) {
            for (int i = 0; i <= INDIC_MAX; ++i)
                ScintillaEdit::send(SimSci::CMD_IndicSetOutlineAlpha, i, alpha);
        } else {
            ScintillaEdit::send(SimSci::CMD_IndicSetOutlineAlpha, indicatorNumber, alpha);
        }
    }
}

int SimScintilla::defineIndicator(SimSci::IndicatorStyle style, int indicatorNumber)
{
    checkIndicator(indicatorNumber);

    if (indicatorNumber >= 0)
        ScintillaEdit::send(SimSci::CMD_IndicSetStyle, indicatorNumber, static_cast<long>(style));

    return indicatorNumber;
}

QStringList SimScintilla::getApiContext(int pos, int &context_start,int &last_word_start)
{
    enum { Either, Separator, Word };

    QStringList words;
    int good_pos = pos, expecting = Either;

    last_word_start = -1;

    while (pos > 0) {
        if (getSeparator(pos)) {
            if (expecting == Either)
                words.prepend(QString());
            else if (expecting == Word)
                break;

            good_pos = pos;
            expecting = Word;
        } else {
            QString word = getWord(pos);

            if (word.isEmpty() || expecting == Separator)
                break;

            words.prepend(word);
            good_pos = pos;
            expecting = Separator;

            // Return the position of the start of the last word if required.
            if (last_word_start < 0)
                last_word_start = pos;
        }

        // Strip any preceding spaces (mainly around operators).
        char ch;

        while ((ch = getCharacter(pos)) != '\0') {
            // This is the same definition of space that Scintilla uses.
            if (ch != ' ' && (ch < 0x09 || ch > 0x0d)) {
                ++pos;
                break;
            }
        }
    }

    // A valid sequence always starts with a word and so should be expecting a
    // separator.
    if (expecting != Separator)
        words.clear();

    context_start = good_pos;

    return words;
}

void SimScintilla::showAutoCompleteFromAll()
{
    startAutoCompletion(SimSci::ACS_All, false, d->autoCompletionUseSingle != SimSci::ACUS_Never);
}

void SimScintilla::showAutoCompleteFromAPIs()
{
    startAutoCompletion(SimSci::ACS_APIs, false, d->autoCompletionUseSingle != SimSci::ACUS_Never);
}

void SimScintilla::showAutoCompleteFromDocument()
{
    startAutoCompletion(SimSci::ACS_Document, false, d->autoCompletionUseSingle != SimSci::ACUS_Never);
}

void SimScintilla::showCallTip()
{
    SimAPIs *apis = d->lexer->apis();

    if (!apis)
        return;

    int pos, commas = 0;
    bool found = false;
    char ch;

    pos = currentPos();

    // Move backwards through the line looking for the start of the current
    // call tip and working out which argument it is.
    while ((ch = getCharacter(pos)) != '\0') {
        if (ch == ',')
            ++commas;
        else if (ch == ')') {
            int depth = 1;

            // Ignore everything back to the start of the corresponding
            // parenthesis.
            while ((ch = getCharacter(pos)) != '\0') {
                if (ch == ')')
                    ++depth;
                else if (ch == '(' && --depth == 0)
                    break;
            }
        } else if (ch == '(') {
            found = true;
            break;
        }
    }

    // Cancel any existing call tip.
    ScintillaEdit::callTipCancel();

    // Done if there is no new call tip to set.
    if (!found)
        return;

    QStringList context = getApiContext(pos, pos, d->nCallTipsPos);

    if (context.isEmpty())
        return;

    // The last word is complete, not partial.
    context << QString();

    d->calltipsCursor= 0;
    d->calltipsShifts.clear();
#ifdef RESERVE
    d_ptr->calltipsEntries = apis->callTips(context, commas, d_ptr->calltipStyle, d_ptr->calltipsShifts);
#endif
    int nr_entries = d->calltipsEntries.count();

    if (nr_entries == 0)
        return;

    if (d->maxCallTips > 0 && d->maxCallTips < nr_entries) {
        d->calltipsEntries = d->calltipsEntries.mid(0, d->maxCallTips);
        nr_entries = d->maxCallTips;
    }

    int shift;
    QString ct;

    int nr_shifts = d->calltipsShifts.count();

    if (d->maxCallTips < 0 && nr_entries > 1) {
        shift = (nr_shifts > 0 ? d->calltipsShifts.first() : 0);
        ct = d->calltipsEntries[0];
        ct.prepend('\002');
    } else {
        if (nr_shifts > nr_entries)
            nr_shifts = nr_entries;

        // Find the biggest shift.
        shift = 0;

        for (int i = 0; i < nr_shifts; ++i) {
            int sh = d->calltipsShifts[i];

            if (shift < sh)
                shift = sh;
        }

        ct = d->calltipsEntries.join("\n");
    }

    QByteArray ct_ba = ct.toLatin1();
    const char *cts = ct_ba.data();

    sends(SCI_CALLTIPSHOW, getAdjustedCallTipPosition(shift), cts);

    // Done if there is more than one call tip.
    if (nr_entries > 1)
        return;

    // Highlight the current argument.
    const char *astart;

    if (commas == 0)
        astart = strchr(cts, '(');
    else
        for (astart = strchr(cts, ','); astart && --commas > 0; astart = strchr(astart + 1, ','))
            ;

    if (!astart || !*++astart)
        return;

    // The end is at the next comma or unmatched closing parenthesis.
    const char *aend;
    int depth = 0;

    for (aend = astart; *aend; ++aend) {
        char ch = *aend;

        if (ch == ',' && depth == 0)
            break;
        else if (ch == '(')
            ++depth;
        else if (ch == ')') {
            if (depth == 0)
                break;

            --depth;
        }
    }

    if (astart != aend)
        ScintillaEdit::send(SCI_CALLTIPSETHLT, astart - cts, aend - cts);
}

/*!
 * \brief 开始记录宏
 */
void SimScintilla::startRecord()
{
    initMacro();

    d->macro->startRecording();
    ScintillaEdit::startRecord();
}

/*!
 * \brief 停止记录宏
 */
void SimScintilla::stopRecord()
{
    if (d->macro != nullptr) {
        d->macro->stopRecording();
    }
    ScintillaEdit::stopRecord();
}

/*!
 * \brief 从扩展名为mac的文件中加载宏并播放.
 * \return 是否加载成功
 */
bool SimScintilla::loadMacros()
{
    initMacro();

    QString openedMacroFile = QFileDialog::getOpenFileName(this, tr("Load macro file"), ".", "*.mac");
    if (openedMacroFile.isEmpty()) {
        return false;
    }
    return d->macro->load(openedMacroFile);
}

/*!
 * \brief 把已经记录的宏保存到文件中.
 * \return 是否保存成功
 */
bool SimScintilla::saveMacros()
{
    initMacro();

    QString savedMacroFile = QFileDialog::getSaveFileName(this, tr("Save macros to a file "), ".", "*.mac");
    if (savedMacroFile.isEmpty()) {
        return false;
    }
    return d->macro->save(savedMacroFile);
}

/*!
 * \reimp
 */
void SimScintilla::mousePressEvent(QMouseEvent *e)
{
    ScintillaEdit::mousePressEvent(e);

    if (e->buttons() & Qt::LeftButton) {
        notifyPositionChanged();
        d->isMousePressed = true;
    }
}

/*!
 * \reimp
 */
void SimScintilla::mouseMoveEvent(QMouseEvent *e)
{
    ScintillaEdit::mouseMoveEvent(e);
    if (d->isMousePressed) {
        notifyPositionChanged();
    }
}

/*!
 * \reimp
 */
void SimScintilla::mouseReleaseEvent(QMouseEvent *e)
{
    ScintillaEdit::mouseReleaseEvent(e);
    d->isMousePressed = false;
}

/*!
 * \reimp
 */
void SimScintilla::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        d->isCtrlPressed = true;
    }
    ScintillaEdit::keyPressEvent(e);
}

/*!
 * \reimp
 */
void SimScintilla::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        d->isCtrlPressed = false;
    }
    ScintillaEdit::keyReleaseEvent(e);
}

/*!
 * \reimp
 */
void SimScintilla::wheelEvent(QWheelEvent *e)
{
    if (d->isCtrlPressed) {
        if (e->delta() < 0) {
            if (zoom() < MinZoomFactor) {
                return;
            }
        }
    }
    ScintillaEdit::wheelEvent(e);
}

/*!
 * \reimp
 */
bool SimScintilla::event(QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride && !isReadOnly()) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        if (ke->key()) {
            // We want ordinary characters.
            if ((ke->modifiers() == Qt::NoModifier ||
                 ke->modifiers() == Qt::ShiftModifier ||
                 ke->modifiers() == Qt::KeypadModifier) &&
                 ke->key() < Qt::Key_Escape) {
                ke->accept();
                return true;
            }

            // We want any key that is bound.
            SimCommand *cmd = d->cmdProxy->boundTo(ke->key() | ke->modifiers());

            if (cmd)
            {
                ke->accept();
                return true;
            }
        }
    }

    return ScintillaEdit::event(e);
}

/*!
 * \brief 处理缩放的槽函数
 * \param 缩放量
 */
void SimScintilla::onZoom(int z)
{
    Q_UNUSED(z);
    notifyTextWidthChanged();
    notifyMarginWidthChanged();
    if (!d->lineNumberVisible) {
        return;
    }

    adjustLineNumberMarginWidth();
}

/*!
 * \brief 处理语法分析器文本颜色改变的槽函数
 * \param style 风格
 * \param clr 颜色
 */
void SimScintilla::onLexerColorChanged(int style, const QColor& clr)
{
    LOG_VAR(style);
    LOG_VAR(clr);
    if (!clr.isValid()) {
        return;
    }
    ScintillaEdit::styleSetFore(style, GetValueFromColor(clr));
}

/*!
 * \brief 处理语法分析器行尾填充改变的槽函数
 * \param style 风格
 * \param fill 是否填充
 */
void SimScintilla::onLexerEolFillChanged(int style, bool fill)
{
    LOG_VAR(style);
    LOG_VAR(fill);
    ScintillaEdit::styleSetEOLFilled(style, fill);
}

/*!
 * \brief 处理语法分析器字体改变的槽函数
 * \param style 风格
 * \param f 字体
 */
void SimScintilla::onLexerFontChanged(int style, const QFont &f)
{
    LOG_VAR(style);
    LOG_VAR(f);
    changeStyleFont(style, f);

    if (style == d->lexer->braceStyle()) {
        changeStyleFont(STYLE_BRACELIGHT, f);
        changeStyleFont(STYLE_BRACEBAD, f);
    }
}

/*!
 * \brief 处理语法分析器背景颜色改变的槽函数
 * \param style 风格
 * \param clr 颜色
 */
void SimScintilla::onLexerBackgroundColorChanged(int style, const QColor &clr)
{
    LOG_VAR(style);
    LOG_VAR(clr);
    if (!clr.isValid()) {
        return;
    }
    ScintillaEdit::styleSetBack(style, GetValueFromColor(clr));
}

/*!
 * \brief 处理语法分析器属性改变的槽函数
 * \param name 名称
 * \param value 数值
 */
void SimScintilla::onLexerPropertyChanged(const QString& name, const QString& value)
{
    LOG_VAR(name);
    LOG_VAR(value);
    ScintillaEdit::setProperty(name.toStdString().c_str(), value.toStdString().c_str());
}

void SimScintilla::onNotify(SCNotification *pscn)
{
    if (pscn->nmhdr.code == SCN_CHARADDED) {
        handleCharAdded(pscn->ch);
    } else if(pscn->nmhdr.code == SCN_DWELLSTART) {
        onDwellStart(pscn->x, pscn->y);
    } else if(pscn->nmhdr.code == SCN_DWELLEND) {
        onDwellEnd(pscn->x, pscn->y);
    }
}

void SimScintilla::onDwellEnd(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    if (ScintillaEdit::callTipActive()) {
        ScintillaEdit::callTipCancel();
    }
}

void SimScintilla::onDwellStart(int x, int y)
{
#if 0
    int pos = positionFromPoint(x, y);
    int start = wordStartPosition(pos, false);
    int endpos = wordEndPosition(pos, true);
    QString currWord = getTextRange(start,endpos);

    QString strTip = getApiTip(currWord);
    ScintillaEdit::callTipShow(positionFromPoint(x, y), convertString2ByteArray(strTip));
#endif
}

void SimScintilla::onUpdateUi()
{
    if (d->addedChar != QChar(0)) {
        int pos =currentPos();
        int line=lineFromPosition(pos);
        int startpos =wordStartPosition(pos-1,false);
        int linepos=positionFromLine(line);
        if(d->addedChar == '}' && startpos==linepos) {
            int leftPos =braceMatch(pos-1);
            int leftline=lineFromPosition(leftPos);
            int leftindent=lineIndentation(leftline);
            char space[1024];
            memset(space,' ',1024);
            setTargetStart(startpos);
            setTargetEnd(pos-1);
            replaceTarget(leftindent,space);
        } if(d->addedChar == '\n') {
            static int nlineIndent =lineIndentation(line-1);
            // 查找上一行最后一个有效字符（非空白字符）
            static int prevlineStartpos = positionFromLine(line-1);
            int c=' ';
            for(int p=pos-2;p>=prevlineStartpos&&isspace(c);p--)
                c=charAt(p);

            if(c==int('{')) {
                nlineIndent += 4;
            }
            char space[1024];
            memset(space,' ',1024);
            space[nlineIndent]=0;
            replaceSel(space);
        }
    }

    //! 这里有待改进, 性能不是很高效
    notifyTextWidthChanged();
    notifyMarginWidthChanged();
}

void SimScintilla::onMarginClicked(int position, int modifiers, int margin)
{
    Q_UNUSED(modifiers);
    Q_UNUSED(margin);
    int line_number = lineFromPosition(position);
    toggleFold(line_number);
    setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED); //如果折叠就在折叠行的上下各画一条横线
}

void SimScintilla::initStyles()
{
    const static int MARGIN_FOLD_INDEX =2;
    setProperty("fold","1");
    setMarginTypeN(MARGIN_FOLD_INDEX,SC_MARGIN_SYMBOL);
    setMarginMaskN(MARGIN_FOLD_INDEX,SC_MASK_FOLDERS);
    setMarginWidthN(MARGIN_FOLD_INDEX,12);
    setMarginSensitiveN(MARGIN_FOLD_INDEX,true);
    // 折叠标签样式
    markerDefinePixmap(SC_MARKNUM_FOLDER, (const char*)plus_xpm);
    markerDefinePixmap(SC_MARKNUM_FOLDEROPEN, (const char*)minus_xpm);
    markerDefinePixmap(SC_MARKNUM_FOLDEREND, (const char*)plus_xpm);
    markerDefinePixmap(SC_MARKNUM_FOLDEROPENMID, (const char*)minus_xpm);

    markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
    markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);

    // 折叠标签颜色
    markerSetBack(SC_MARKNUM_FOLDERSUB, GetValueFromColor(Qt::black));
    markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, GetValueFromColor(Qt::black));
    markerSetBack(SC_MARKNUM_FOLDERTAIL, GetValueFromColor(Qt::black));

    setFoldMarginColour(true, GetValueFromColor(QColor(199, 199, 199)));
    setFoldMarginHiColour(true, GetValueFromColor(QColor(255,255,255)));
}

QByteArray SimScintilla::convertString2ByteArray(const QString &text)
{
    if (isUtf8())
        return text.toUtf8();

    return text.toLatin1();
}

QString SimScintilla::convertText2String(const char *str)
{
    if (isUtf8())
        return QString::fromUtf8(str);

    return QString::fromLatin1(str);
}

void SimScintilla::changeBackgroundStyle(int style, const QColor &color)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetBack, style, GetValueFromColor(color));
}

void SimScintilla::changeForegroundStyle(int style, const QColor &color)
{
    ScintillaEdit::send(SimSci::CMD_StyleSetBack, style, GetValueFromColor(color));
}

void SimScintilla::changeStyleFont(int style, const QFont &f)
{
    ScintillaEdit::styleSetFont(style, f.family().toLatin1().data());
    ScintillaEdit::styleSetSize(style, f.pointSize());
    ScintillaEdit::styleSetWeight(style, -f.weight());
    ScintillaEdit::styleSetItalic(style, f.italic());
    ScintillaEdit::styleSetUnderline(style, f.underline());
    ScintillaEdit::styleSetBold(style, f.bold());

    // Tie the font settings of the default style to that of style 0 (the style
    // conventionally used for whitespace by lexers).  This is needed so that
    // fold marks, indentations, edge columns etc are set properly.
    if (style == 0)
        changeStyleFont(STYLE_DEFAULT, f);
}

void SimScintilla::foldExpand(int &line, bool doExpand, bool force, int visLevels, int level)
{
    int lineMaxSubord = ScintillaEdit::send(SimSci::CMD_GetLastChild, line, level & SC_FOLDLEVELNUMBERMASK);

    line++;

    while (line <= lineMaxSubord) {
        if (force) {
            if (visLevels > 0)
                ScintillaEdit::send(SimSci::CMD_ShowLines, line, line);
            else
                ScintillaEdit::send(SimSci::CMD_HideLines, line, line);
        } else if (doExpand)
            ScintillaEdit::send(SimSci::CMD_ShowLines, line, line);

        int levelLine = level;

        if (levelLine == -1)
            levelLine = ScintillaEdit::send(SimSci::CMD_GetFoldLevel, line);

        if (levelLine & SC_FOLDLEVELHEADERFLAG) {
            if (force) {
                if (visLevels > 1)
                    ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 1);
                else
                    ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 0L);

                foldExpand(line, doExpand, force, visLevels - 1);
            } else if (doExpand) {
                if (!ScintillaEdit::send(SimSci::CMD_GetFoldExpanded, line))
                    ScintillaEdit::send(SimSci::CMD_SetFoldExpanded, line, 1);

                foldExpand(line, true, force, visLevels - 1);
            } else
                foldExpand(line, false, force, visLevels - 1);
        }
        else
            line++;
    }
}
void SimScintilla::checkMarker(int &markerNumber)
{
    allocateId(markerNumber, d->allocatedMarkers, 0, MARKER_MAX);
}

void SimScintilla::checkIndicator(int &indicatorNumber)
{
    allocateId(indicatorNumber, d->allocatedIndicators, INDIC_CONTAINER, INDIC_MAX);
}

void SimScintilla::allocateId(int &id, unsigned &allocated, int min, int max)
{
    if (id >= 0) {
        // Note that we allow existing identifiers to be explicitly redefined.
        if (id > max)
            id = -1;
    } else {
        unsigned aids = allocated >> min;

        // Find the smallest unallocated identifier.
        for (id = min; id <= max; ++id) {
            if ((aids & 1) == 0)
                break;

            aids >>= 1;
        }
    }

    // Allocate the identifier if it is valid.
    if (id >= 0)
        allocated |= (1 << id);
}

void SimScintilla::gotoMatchingBrace(bool select)
{
    long braceAtCaret;
    long braceOpposite;

    bool isInside = findMatchingBrace(braceAtCaret, braceOpposite,
                                      SimSci::BM_Sloppy);

    if (braceOpposite >= 0) {
        // Convert the character positions into caret positions based on
        // whether the caret position was inside or outside the braces.
        if (isInside) {
            if (braceOpposite > braceAtCaret)
                braceAtCaret++;
            else
                braceOpposite++;
        } else {
            if (braceOpposite > braceAtCaret)
                braceOpposite++;
            else
                braceAtCaret++;
        }

        ensureLineVisible( ScintillaEdit::send(SimSci::CMD_LineFromPosition, braceOpposite));

        if (select)
            ScintillaEdit::send(SimSci::CMD_SetSel, braceAtCaret, braceOpposite);
        else
            ScintillaEdit::send(SimSci::CMD_SetSel, braceOpposite, braceOpposite);
    }
}

bool SimScintilla::findMatchingBrace(long &brace, long &other, SimSci::BraceMatch mode)
{
    bool colonMode = false;
    int brace_style = (d->lexer == nullptr ? -1 : d->lexer->braceStyle());

    brace = -1;
    other = -1;

    long caretPos = ScintillaEdit::send(SimSci::CMD_GetCurrentPos);

    if (caretPos > 0)
        brace = checkBrace(caretPos - 1, brace_style, colonMode);

    bool isInside = false;

    if (brace < 0 && mode == SimSci::BM_Sloppy) {
        brace = checkBrace(caretPos, brace_style, colonMode);

        if (brace >= 0 && !colonMode)
            isInside = true;
    }

    if (brace >= 0) {
        if (colonMode) {
            // Find the end of the Python indented block.
            long lineStart = ScintillaEdit::send(SimSci::CMD_LineFromPosition, brace);
            long lineMaxSubord = ScintillaEdit::send(SimSci::CMD_GetLastChild, lineStart, -1);

            other = ScintillaEdit::send(SimSci::CMD_GetLineEndPosition, lineMaxSubord);
        }
        else
            other = ScintillaEdit::send(SimSci::CMD_BraceMatch, brace);

        if (other > brace)
            isInside = !isInside;
    }

    return isInside;
}

// Check if the character at a position is a brace.
long SimScintilla::checkBrace(long pos, int brace_style, bool &colonMode)
{
    long brace_pos = -1;
    char ch = ScintillaEdit::send(SimSci::CMD_GetCharAt, pos);

    if (ch == ':') {
        // A bit of a hack, we should really use a virtual.
        if (d->lexer != nullptr && qstrcmp(d->lexer->lexerStr().toLatin1(), "python") == 0)
        {
            brace_pos = pos;
            colonMode = true;
        }
    } else if (ch && strchr("[](){}<>", ch)) {
        if (brace_style < 0)
            brace_pos = pos;
        else {
            int style = ScintillaEdit::send(SimSci::CMD_GetStyleAt, pos) & 0x1f;

            if (style == brace_style)
                brace_pos = pos;
        }
    }

    return brace_pos;
}

// Set up a folder marker.
void SimScintilla::setFoldMarker(int marknr, int mark)
{
    ScintillaEdit::send(SimSci::CMD_MarkerDefine, marknr, mark);

    if (mark != SC_MARK_EMPTY) {
        ScintillaEdit::send(SimSci::CMD_MarkerSetFore, marknr, GetValueFromColor(QColor(Qt::white)));
        ScintillaEdit::send(SimSci::CMD_MarkerSetBack, marknr, GetValueFromColor(QColor(Qt::black)));
    }
}

void SimScintilla::startAutoCompletion(SimSci::AutoCompletionSource acs, bool checkThresh, bool choose_single)
{
    int start, ignore;
    QStringList context = getApiContext(currentPos(), start, ignore);

    if (context.isEmpty())
        return;

    // Get the last word's raw data and length.
    QByteArray s = convertString2ByteArray(context.last());
    const char *last_data = s.constData();
    int lastlen = s.size();

    if (checkThresh && lastlen < d->autoCompletionThreshhold )
        return;

    // Generate the string representing the valid words to select from.
    QStringList wlist;

    if ((acs == SimSci::ACS_All || acs == SimSci::ACS_APIs) && d->lexer != nullptr) {
        SimAPIs *apis = d->lexer->apis();

        if (apis != nullptr)
            apis->updateAutoCompletionList(context, wlist);
    }

    if (acs == SimSci::ACS_All || acs == SimSci::ACS_Document) {
        int sflags = SCFIND_WORDSTART;

        if (!ScintillaEdit::send(SimSci::CMD_AutoCGetIgnoreCase))
            sflags |= SCFIND_MATCHCASE;

        ScintillaEdit::send(SimSci::CMD_SetSearchFlags, sflags);

        int pos = 0;
        int dlen = length();
        int caret = currentPos();
        int clen = caret - start;
        char *orig_context = new char[clen + 1];

        TextRange tr;
        tr.chrg.cpMin = start;
        tr.chrg.cpMax = caret;
        tr.lpstrText = orig_context;

        ScintillaEdit::send(SimSci::CMD_GetTextRange, 0, reinterpret_cast<sptr_t>(&tr));

        for (;;) {
            int fstart;

            ScintillaEdit::send(SimSci::CMD_SetTargetStart, pos);
            ScintillaEdit::send(SimSci::CMD_SetTargetEnd, dlen);

            if ((fstart = sends(SimSci::CMD_SearchInTarget, clen, orig_context)) < 0)
                break;

            // Move past the root part.
            pos = fstart + clen;

            // Skip if this is the context we are auto-completing.
            if (pos == caret)
                continue;

            // Get the rest of this word.
            QString w = last_data;

            while (pos < dlen) {
                char ch = ScintillaEdit::send(SimSci::CMD_GetCharAt, pos);

                if (!isWordCharacter(ch))
                    break;

                w += ch;
                ++pos;
            }

            // Add the word if it isn't already there.
            if (!w.isEmpty()) {
                bool keep;

                // If there are APIs then check if the word is already present
                // as an API word (i.e. with a trailing space).
                if (acs == SimSci::ACS_All) {
                    QString api_w = w;
                    api_w.append(' ');

                    keep = !wlist.contains(api_w);
                } else {
                    keep = true;
                }

                if (keep && !wlist.contains(w))
                    wlist.append(w);
            }
        }

        delete []orig_context;
    }

    if (wlist.isEmpty())
        return;

    wlist.sort();

    ScintillaEdit::send(SimSci::CMD_AutoCSetChooseSingle, choose_single);
    ScintillaEdit::send(SimSci::CMD_AutoCSetSeparator, AutoCSeparator);

    QByteArray wlist_s = convertString2ByteArray(wlist.join(QChar(AutoCSeparator)));
    sends(SimSci::CMD_AutoCShow, lastlen, wlist_s.constData());
}

bool SimScintilla::getSeparator(int &pos) const
{
    int opos = pos;

    // Go through each separator.
    for (int i = 0; i < d->autoCompletionSeparators.count(); ++i) {
        const QString &ws = d->autoCompletionSeparators[i];

        // Work backwards.
        uint l;

        for (l = ws.length(); l; --l) {
            char ch = getCharacter(pos);

            if (ch == '\0' || ws.at(l - 1) != ch)
                break;
        }

        if (!l)
            return true;

        // Reset for the next separator.
        pos = opos;
    }

    return false;
}

QString SimScintilla::getWord(int &pos) const
{
    QString word;
    bool numeric = true;
    char ch;

    while ((ch = getCharacter(pos)) != '\0') {
        if (!isWordCharacter(ch)) {
            ++pos;
            break;
        }

        if (ch < '0' || ch > '9')
            numeric = false;

        word.prepend(ch);
    }

    // We don't auto-complete numbers.
    if (numeric)
        word.truncate(0);

    return word;
}

char SimScintilla::getCharacter(int &pos) const
{
    if (pos <= 0)
        return '\0';

    char ch = ScintillaEdit::send(SimSci::CMD_GetCharAt, --pos);

    // Don't go past the end of the previous line.
    if (ch == '\n' || ch == '\r') {
        ++pos;
        return '\0';
    }

    return ch;
}

int SimScintilla::getAdjustedCallTipPosition(int ctshift) const
{
    int ct = d->nCallTipsPos;

    if (ctshift) {
        int ctmin = ScintillaEdit::send(SCI_POSITIONFROMLINE, ScintillaEdit::send(SCI_LINEFROMPOSITION, ct));

        if (ct - ctshift < ctmin)
            ct = ctmin;
    }
    return 0;
}

/*!
 * \brief 建立起Lexer与Scintilla之间的信号槽函数, 便于在Lexer的设置改变时更新文本的风格.
 * \param lexer
 */
void SimScintilla::handleLexerSignals(const SimAbstractLexer *lexer)
{
    if (lexer != nullptr) {
        connect(lexer, SIGNAL(colorChanged(int,QColor)), this, SLOT(onLexerColorChanged(int, QColor)));
        connect(lexer, SIGNAL(backgroundColorChanged(int,QColor)), this, SLOT(onLexerBackgroundColorChanged(int, QColor)));
        connect(lexer, SIGNAL(eolFillChanged(int,bool)), this, SLOT(onLexerEolFillChanged(int, bool)));
        connect(lexer, SIGNAL(fontChanged(int,QFont)), this, SLOT(onLexerFontChanged(int,QFont)));
        connect(lexer, SIGNAL(propertyChanged(QString,QString)), this, SLOT(onLexerPropertyChanged(QString,QString)));
    }
}

void SimScintilla::setLexerStyle(int style)
{
    onLexerColorChanged(style, d->lexer->defaultColor(style));
    onLexerBackgroundColorChanged(style, d->lexer->defaultBackgroundColor(style));
    onLexerEolFillChanged(style, d->lexer->defaultEolFill(style));
    onLexerFontChanged(style, d->lexer->defaultFont(style));
}

void SimScintilla::detachLexer()
{
    if (d->lexer != nullptr) {
        d->lexer->setEditor(nullptr);
        d->lexer->disconnect(this);

        ScintillaEdit::styleClearAll();
        ScintillaEdit::clearDocumentStyle();
        ScintillaEdit::styleResetDefault();

        delete d->lexer;
    }
}

void SimScintilla::notifyPositionChanged()
{
    int line = 0;
    int index = 0;
    lineIndexFromPosition(currentPos(), line, index);
    emit positionChanged(line, index);
    emit positionChanged(currentPos());
}

void SimScintilla::setupConns()
{
    connect(this, SIGNAL(zoom(int)), SLOT(onZoom(int)));
    connect(this, SIGNAL(notify(SCNotification*)), this, SLOT(onNotify(SCNotification*)));
    connect(this, SIGNAL(dwellEnd(int,int)), this, SLOT(onDwellEnd(int,int)));
    connect(this, SIGNAL(dwellStart(int,int)), this, SLOT(onDwellStart(int,int)));
    connect(this, SIGNAL(updateUi()), this, SLOT(onUpdateUi()));
    connect(this, SIGNAL(marginClicked(int,int,int)), this, SLOT(onMarginClicked(int,int,int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SIGNAL(hScrollBarValueChanged(int)));
}

void SimScintilla::resetNullLexer()
{
    ScintillaEdit::setLexer(SimSci::Lang_None);

    setTextColor(d->textColor);
    setBackgroundColor(d->backgroundColor);

    ScintillaEdit::autoCSetFillUps(d->autoCompletionFillups.toStdString().c_str());
    ScintillaEdit::autoCSetIgnoreCase(false);
    d->autoCompletionSeparators.clear();
    d->wordchars = DefaultWordChars;
}

void SimScintilla::adjustLineNumberMarginWidth()
{
    if (!isLineNumberVisible()) {
        return;
    }
#ifdef USELOG
    LOG_VAR(zoomFactor());
#endif
    int maxLine = lineCount();
    LOG_VAR(maxLine);
    qreal textWidth = fontMetrics().width(QString("%1").arg(maxLine));
    qreal marginWidth = 0;

    if (zoomFactor() <= 1) {
        marginWidth = textWidth * 2;
    } else {
        marginWidth = textWidth * zoomFactor() * 0.7;
    }

    LOG_VAR(marginWidth);

    ScintillaEdit::setMarginWidthN(1, marginWidth);
}

bool SimScintilla::isStartChar(char ch) const
{
    QString s = QChar(ch);

    for (int i = 0; i < d->autoCompletionSeparators.size(); ++i)
        if (d->autoCompletionSeparators[i].endsWith(s))
            return true;

    return false;
}

void SimScintilla::maintainIndentation(char ch, long pos)
{
    if (ch != '\r' && ch != '\n')
        return;

    int currline = lineFromPosition(pos);

    // Get the indentation of the preceding non-zero length line.
    int ind = 0;

    for (int line = currline - 1; line >= 0; --line) {
        if (lineEndPosition(line) > positionFromLine(line)) {
            ind = lineIndentation(line);
            break;
        }
    }

    if (ind > 0)
        autoIndentLine(pos, currline, ind);
}

void SimScintilla::autoIndentLine(long pos, int line, int indent)
{
    if (indent < 0)
        return;

    long posbefore = lineEndPosition(line);
    setLineIndentation(line, indent);
    long psoafter = lineIndentPosition(line);
    long newpos = -1;

    if (psoafter > posbefore)
        newpos = pos + (psoafter - posbefore);
    else if (psoafter < posbefore && pos >= psoafter) {
        if (pos >= posbefore)
            newpos = pos + (psoafter - posbefore);
        else
            newpos = psoafter;
    }

    if (newpos >= 0)
        ScintillaEdit::setSel(newpos, newpos);
}

void SimScintilla::autoIndentation(char ch, long pos)
{
    int currline = lineFromPosition(pos);
    int indentWidth = ScintillaEdit::indent() == 0 ? ScintillaEdit::tabWidth() : ScintillaEdit::indent();
    long currlineStart = positionFromLine(currline);

    const char *blockStart = d->lexer->blockStart().toStdString().c_str();
    bool startSingle = (blockStart && qstrlen(blockStart) == 1);

    const char *blockEnd = d->lexer->blockEnd().toStdString().c_str();
    bool endSingle = (blockEnd && qstrlen(blockEnd) == 1);

    if (endSingle && blockEnd[0] == ch) {
        if (!(d->lexer->autoIndentStyle() & SimSci::AIS_Closing) && rangeIsWhitespace(currlineStart, pos - 1))
            autoIndentLine(pos, currline, blockIndent(currline - 1) - indentWidth);
    } else if (startSingle && blockStart[0] == ch) {
        // De-indent if we have already indented because the previous line was
        // a start of block keyword.
        if (!(d->lexer->autoIndentStyle() & SimSci::AIS_Opening) && currline > 0 && d->getIndentState(currline - 1) == SimScintillaPrivate::isKeywordStart && rangeIsWhitespace(currlineStart, pos - 1))
            autoIndentLine(pos, currline, blockIndent(currline - 1) - indentWidth);
    } else if (ch == '\r' || ch == '\n')
        autoIndentLine(pos, currline, blockIndent(currline - 1));
}

bool SimScintilla::rangeIsWhitespace(long spos, long epos)
{
    while (spos < epos) {
        char ch = ScintillaEdit::charAt(spos);

        if (ch != ' ' && ch != '\t')
            return false;

        ++spos;
    }

    return true;
}

int SimScintilla::blockIndent(int line)
{

    if (line < 0 || d->lexer == nullptr)
        return 0;

    // Handle the trvial case.
    if (d->lexer->blockStartKeyword().isEmpty() && d->lexer->blockStart().isEmpty() && d->lexer->blockEnd().isEmpty())
        return ScintillaEdit::lineIndentation(line);

    int lineLimit = line - d->lexer->blockLookback();

    if (lineLimit < 0)
        lineLimit = 0;

    for (int l = line; l >= lineLimit; --l) {
        SimScintillaPrivate::IndentState istate = d->getIndentState(l);

        if (istate != SimScintillaPrivate::isNone) {
            int indentWidth = ScintillaEdit::indent();
            int ind = lineIndentation(l);

            if (istate == SimScintillaPrivate::isBlockStart) {
                if (!(d->lexer->autoIndentStyle() & SimSci::AIS_Opening))
                    ind += indentWidth;
            } else if (istate == SimScintillaPrivate::isBlockEnd) {
                if (d->lexer->autoIndentStyle() & SimSci::AIS_Closing)
                    ind -= indentWidth;

                if (ind < 0)
                    ind = 0;
            } else if (line == l)
                ind += indentWidth;

            return ind;
        }
    }

    return ScintillaEdit::lineIndentation(line);
}

/*!
 * \brief 根据API获取相应提示.
 * \param api
 * \return 提示信息
 */
QString SimScintilla::getApiTip(const QString &api)
{
    return "null";
}

void SimScintilla::handleCharAdded(char ch)
{
    LOG_VAR(ch);
    emit charAdded(ch);
    d->addedChar = ch;

    addEnclosingChar(ch);

    // 如果是选中文本的话就忽略自动完成
    long pos = ScintillaEdit::selectionStart();

    if (pos != ScintillaEdit::selectionEnd() || pos == 0)
        return;

    // If auto-completion is already active then see if this character is a
    // start character.  If it is then create a new list which will be a subset
    // of the current one.  The case where it isn't a start character seems to
    // be handled correctly elsewhere.
    if (ScintillaEdit::autoCActive()) {
        if (isStartChar(ch)) {
            ScintillaEdit::autoCCancel();
            startAutoCompletion(d->autoCompletionSource, false, d->autoCompletionUseSingle == SimSci::ACUS_Always);
        }

        return;
    }

    // Handle call tips.
    if (d->calltipStyle != SimSci::CTS_None && d->lexer != nullptr && strchr("(),", ch) != NULL)
        showCallTip();

    // Handle auto-indentation.
    if (d->autoIndent) {
        if (d->lexer == nullptr || d->lexer->autoIndentStyle() & SimSci::AIS_Maintain)
            maintainIndentation(ch, pos);
        else
            autoIndentation(ch, pos);
    }

    // See if we might want to start auto-completion.
    if (!ScintillaEdit::autoCActive() && d->autoCompletionSource != SimSci::ACS_None) {
        if (isStartChar(ch))
            startAutoCompletion(d->autoCompletionSource, false, d->autoCompletionUseSingle == SimSci::ACUS_Always);
        else if (d->autoCompletionThreshhold >= 1 && isWordCharacter(ch))
            startAutoCompletion(d->autoCompletionSource, true, d->autoCompletionUseSingle == SimSci::ACUS_Always);
    }
}

/*!
 * \brief 根据输入的字符自动添加结束符.
 * \param ch 输入字符
 */
void SimScintilla::addEnclosingChar(char ch)
{
    if (ch == '(') {
        ScintillaEdit::insertText(currentPos(), ")");
    } else if (ch == '{') {
        ScintillaEdit::insertText(currentPos(), "}");
    } else if (ch == '[') {
        ScintillaEdit::insertText(currentPos(), "]");
    }
}

/*!
 * \brief 获取文档中指定开始位置和结束位置之间的字符串.
 * \param start 开始位置
 * \param end 结束为止
 * \return 区间字符串
 */
QString SimScintilla::getTextRange(int start, int end)
{
    int clen = end - start;
    char *textArr = new char[clen + 1];

    TextRange tr;
    tr.chrg.cpMin = start;
    tr.chrg.cpMax = end;
    tr.lpstrText = textArr;

    ScintillaEdit::send(SimSci::CMD_GetTextRange, 0, reinterpret_cast<sptr_t>(&tr));

    QString text(textArr);
    delete []textArr;
    return text;
}

/*!
 * \brief 获取当前位置的单词.
 */
QString SimScintilla::getCurrentWord()
{
    int start = wordStartPosition(currentPos(), false);
    int endpos = wordEndPosition(currentPos(), true);
    QString currWord = getTextRange(start,endpos);
    return currWord;
}

/*!
 * \brief 初始化宏对象
 */
void SimScintilla::initMacro()
{
    if (d->macro == nullptr) {
        d->macro = new SimMacro(this);
    }
}

/*!
 * \brief 发射字符宽度改变的信号
 */
void SimScintilla::notifyTextWidthChanged()
{
    qreal textWidth = ScintillaEdit::textWidth(STYLE_DEFAULT, "A");
    emit textWidthChanged(textWidth);
}

void SimScintilla::notifyMarginWidthChanged()
{
    int marginWidth_ = 0;
    for (int i = 0; i < 4; i++) {
        marginWidth_ += marginWidth(i);
    }
    emit marginWidthChanged(marginWidth_);
}

void SimScintilla::applyStyles()
{
    int styleCount = 1 << ScintillaEdit::styleBits();

    for (int s = 0; s < styleCount; ++s) {
        if (!d->lexer->description(s).isEmpty())
            setLexerStyle(s);
    }
}

void SimScintilla::setLexerKeywords()
{
    for (int k = 0; k <= KEYWORDSET_MAX; ++k) {
        const QString kw = d->lexer->keywords(k);
        ScintillaEdit::setKeyWords(k, kw.toStdString().c_str());
    }
}

void SimScintilla::getLexerProps()
{
    d->autoCompletionSeparators = d->lexer->autoCompletionWordSeparators();
    d->wordchars = d->lexer->wordCharacters();

    if (d->wordchars.isEmpty())
        d->wordchars = DefaultWordChars;
}

void SimScintilla::createLexerAPI(SimAbstractLexer *lexer)
{
    if (lexer == nullptr) {
        return;
    }
    SimAPIs* apis = new SimAPIs(lexer);
    lexer->setAPIs(apis);
    const int MaxKeywordCount = 255;
    for (int i = 0; i < MaxKeywordCount; ++i) {
        QString kws = lexer->keywords(i);
        if (kws.isEmpty()) {
            continue;
        }
        QStringList kwList = kws.split(' ');
        for(const QString& kw : kwList) {
            apis->add(kw);
        }
    }
}

SimScintilla::TextStylePair SimScintilla::styleText(const QList<SimStyledText> &styledText, int styleOffset)
{
    QString text;

    // Build the full text.
    for (int i = 0; i < styledText.count(); ++i) {
        const SimStyledText &st = styledText[i];

        st.apply(this);

        text.append(st.text());
    }

    QByteArray s = convertString2ByteArray(text);

    // There is a style byte for every byte.
    char *sp = new char[s.length()];

    for (int i = 0; i < styledText.count(); ++i)
    {
        const SimStyledText &st = styledText[i];
        QByteArray part = convertString2ByteArray(st.text());
        int partLength = part.size();

        for (int c = 0; c < partLength; ++c)
            *sp++ = (char)(st.style() - styleOffset);
    }

    QByteArray processedStr(sp);
    delete []sp;
    return TextStylePair(s, processedStr);
}

/*!
 * 返回命令代理只针对象, 方便外部对象调用SimCommand
 */
SimCommandProxy* SimScintilla::commandProxy() const
{
    return d->cmdProxy;
}

void SimScintilla::setShortcut(QAction *action, SimSci::Command cmdId) const
{
    SimCommand *cmd = d->cmdProxy->find(cmdId);

    if (cmd && cmd->key())
        action->setShortcut(QKeySequence(cmd->key()));
}
