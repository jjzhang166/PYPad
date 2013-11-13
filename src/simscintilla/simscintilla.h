#ifndef SIMSCINTILLA_H
#define SIMSCINTILLA_H

#include "simscintilla_global.h"
#include "simscintilladefs.h"
#include "simstyledtext.h"
#include "simabstractlexer.h"

#include <ScintillaEdit.h>
#include <QMouseEvent>

class SimCommandProxy;
class SimDocument;
class SimScintillaPrivate;
/*!
 * \class SimScintilla
 * \brief
 * \ingroup SimScintilla
 */
class SIMSCINTILLA_EXPORT SimScintilla : public ScintillaEdit
{
    Q_DECLARE_PRIVATE(SimScintilla)
    Q_OBJECT

public:
    explicit SimScintilla(QWidget *parent = 0);
    ~SimScintilla();

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void setTabWidth(int tabWidth);
    int tabWidth() const;

    void setText(const QString& text);
    void append(const QString& text);
    QString text();
    QString textFromLine(int line);
    void insert(const QString& text);
    void insertAt(int line, int index, const QString& text);
    void replace(const QString& replaceStr);
    void clear();

    void zoomIn();
    void zoomOut();
    void zoomFit();
    void setZoomFactor(int zoomFactor);
    int zoomFactor() const;

    void setWrapVisualFlags(SimSci::WrapVisualFlag endFlag, SimSci::WrapVisualFlag startFlag = SimSci::WVF_None, int indent = 0);

    bool findFirst(const QString &expr, bool re, bool cs, bool wo,
                   bool wrap, bool forward = true, int line = -1,
                   int index = -1,bool show = true, bool posix = false);
    bool findFirstInSelection(const QString &expr, bool re, bool cs,bool wo,
                              bool forward = true, bool show = true,bool posix = false);
    bool findNext();

    void setWhitespaceVisibility(SimSci::WhitespaceVisibility visibility);
    SimSci::WhitespaceVisibility whitespaceVisibility() const;

    void setEolVisible(bool visible);
    bool isEolVisible() const;

    void setEolMode(SimSci::EolMode eol);
    SimSci::EolMode eolMode() const;
    void convertEols(SimSci::EolMode eol);

    void setSelectionToEol(bool fill);
    bool isSelectionToEol() const;

    void setLanguage(SimSci::Language lang);
    SimSci::Language language() const;

    void setWrapMode(SimSci::Wrap wrap);
    SimSci::Wrap wrapMode() const;

    void setWrapIndentMode(SimSci::WrapIndent wi);
    SimSci::WrapIndent wrapIndentMode() const;

    void setCursor(SimSci::Cursor cursor);
    SimSci::Cursor cursor() const;

    void setCacheMode(SimSci::Cache cache);
    SimSci::Cache cacheMode() const;

    void setCaretStyle(SimSci::CaretStyle style);
    SimSci::CaretStyle caretStyle() const;

    void setAnnotationVisible(bool visible);
    bool isAnnotationVisible() const;

    void setAnnotationStyle(int line, SimSci::AnnotationStyle style);
    SimSci::AnnotationStyle annotationStyle(int line) const;

    void setAnnotationText(int line, const QString& text);
    QString annotationText(int line) const;

    void setAnnotationText(int line, const QString& text, SimSci::AnnotationStyle style);
    void setAnnotationText(int line, const QString& text, const SimAbstractTextStyle& style);
    void setAnnotationText(int line, const SimStyledText& text);
    void setAnnotationText(int line, const QList<SimStyledText> &text);
    void clearAnnotations(int line = -1);

    void setTechnology(SimSci::Technology tech);
    SimSci::Technology technology() const;

    void setEdgeMode(SimSci::Edge edge);
    SimSci::Edge edgeMode() const;

    void setEdgeColumn(int column);
    int edgeColumn() const;

    void setEdgeColor(const QColor& clr);
    QColor edgeColor() const;

    void setEnableMultiSelection(bool enable);
    bool isMultiSelectionEnabled() const;

    void setLineNumberVisible(bool visible);
    bool isLineNumberVisible() const;

    void setAutoIndent(bool autoIndent);
    bool isAutoIndent() const;

    void setOverwriteMode(bool overwrite);
    bool isOverwriteMode() const;

    bool isRedoEnabled();
    bool isUndoEnalbed();
    bool isModified();
    int lineCount() const;
    int lineAt(const QPoint& point);
    int lineLength(int line);
    int length() const;
    QString selectedText();
    int textHeight(int line);

    void indent(int line);
    void unindent(int line);

    void setSelectionBackgroundColor(const QColor& color);
    QColor selectionBackgroundColor() const;
    void resetSelectionBackgroundColor();

    void setSelectionForegroundColor(const QColor& color);
    QColor selectionForegroundColor() const;
    void resetSelectionForegroundColor();

    void setBackgroundColor(const QColor& clr);
    QColor backgroundColor() const;

    void setTextColor(const QColor& color);
    QColor textColor() const;

    void setModified(bool m);
    bool isModified() const;

    void setUtf8(bool u);
    bool isUtf8() const;

    void setFont(const QFont &font);
    QFont font() const;

    void removeSelectedText();
    void replaceSeletectedText(const QString& text);

    void setCursorPosition(int line, int index);
    void cursorPosition(int& line, int& index);
    void lineIndexFromPosition(int pos, int& line, int& index);
    int positionFromLineIndex(int line, int index);

    bool isFirstLineVisible() const;
    bool hasSelectedText();

    void setLineIndentation(int line, int indentSize);
    int lineIndentation(int line);

    void setIndentationGuides(bool enable);
    bool isIndentationGuides() const;

    void setIndentationGuidesBackgroundColor(const QColor& color);
    QColor indentationGuidesBackgroundColor() const;

    void setIndentationGuidesForegroundColor(const QColor& color);
    QColor indentationGuidesForegroundColor() const;

    void setIndentationWidth(int indentWidth);
    int indentationWidth() const;

    void setIndentationUseTabs(bool use);
    bool isIndentationUseTabs() const;

    void setFirstVisibleLine(int line);
    int firstVisibleLine() const;

    bool isRedoAvailable();
    bool isUndoAvailable();

    void setDocument(const SimDocument& doc);
    SimDocument document() const;

    void setWhitespaceBackgroundColor(const QColor& color);
    QColor whitespaceBackgroundColor() const;

    void setWhitespaceForegroundColor(const QColor& color);
    QColor whitespaceForegroundColor() const;

    void setTabIndents(bool tabIndents);
    bool isTabIndents();

    QString wordAtLineIndex(int line, int index);
    QString wordAtPoint(const QPoint& pot);
    QString wordAtPosition(int pos);

    void setExtraAscent(int extraAscent);
    int extraAscent() const;

    void setExtraDescent(int extraDescent);
    int extraDescent() const;

    void setCallTipsBackgroundColor(const QColor& color);
    QColor callTipsBackgroundColor() const;

    void setCallTipsForegroundColor(const QColor& color);
    QColor callTipsForegroundColor() const;

    void setCallTipsHighlightColor(const QColor& color);
    QColor callTipsHighlightColor() const;

    void setCallTipsPosition(SimSci::CallTipsPosition position);
    SimSci::CallTipsPosition callTipsPosition() const;

    void setCallTipsStyle(SimSci::CallTipsStyle style);
    SimSci::CallTipsStyle callTipsStyle() const;

    void setCallTipsVisible(int num);
    int callTipsVisible() const;

    bool isCallTipsActive();

    void setCaretForegroundColor(const QColor &col);
    void setCaretLineBackgroundColor(const QColor &col);
    void setCaretLineVisible(bool enable);
    void setCaretWidth(int width);

    void setSelection(int lineFrom, int indexFrom, int lineTo, int indexTo);
    void selection(int& lineFrom, int& indexFrom, int& lineTo,int& indexTo) const;

    void ensureCursorVisible();
    void ensureLineVisible(int line);

    void setMarginsBackgroundColor(const QColor &col);
    QColor marginsBackgroundColor() const;

    void setMarginsFont(const QFont &f);
    QFont marginsFont() const;

    void setMarginsForegroundColor(const QColor &col);
    QColor marginsForegroundColor() const;

    void setMarginLineNumbers(int margin, bool lnrs);
    bool isMarginLineNumbers(int margin);

    void setMarginMarkerMask(int margin, int mask);
    int marginMarkerMask(int margin);

    void setMarginSensitivity(int margin, bool sens);
    bool marginSensitivity(int margin);

    void setMarginWidth(int margin, int width);
    void setMarginWidth(int margin, const QString &s);
    int marginWidth(int margin);

    void setMarginType(int margin, SimSci::MarginType type);
    SimSci::MarginType marginType(int margin);

    void setMarginOptions(int options);
    int marginOptions() const;

    void setMarginText(int line, const QString &text, int style);
    void setMarginText(int line, const QString &text, const SimAbstractTextStyle &style);
    void setMarginText(int line, const SimStyledText &text);
    void setMarginText(int line, const QList<SimStyledText> &text);
    QString marginText(int line);

    void clearMarginText(int line = -1);

    void read(QIODevice* io);
    void write(QIODevice* io);

    void registerImage(int id, const QPixmap &pm);
    void registerImage(int id, const QImage &im);
    void clearRegisteredImages();

    void setFoldMarginColors(const QColor &fore, const QColor &back);
    void resetFoldMarginColors();

    void recolor(int start = 0, int end = -1);

    bool isUserListActive() const;
    void showUserList(int id, const QStringList &list);
    void cancelUserList();

    bool isWordCharacter(char ch) const;
    QString wordCharacters() const;

    void setLexer(SimAbstractLexer* lexer);
    SimAbstractLexer *lexer() const;
    bool isLexerCaseSensitive() const;

    void foldAll(bool children = false);
    void foldLine(int line);
    void clearFolds();

    void setContractedFolds(const QList<int> &folds);
    QList<int> contractedFolds() const;

    void setMarkerBackgroundColor(int markerNumber, const QColor &col);
    void setMarkerForegroundColor(int markerNumber, const QColor &col);
    void setMatchedBraceBackgroundColor(const QColor &col);
    void setMatchedBraceForegroundColor(const QColor &col);
    void setMatchedBraceIndicator(int indicatorNumber);
    void resetMatchedBraceIndicator();
    void setUnmatchedBraceBackgroundColor(const QColor &col);
    void setUnmatchedBraceForegroundColor(const QColor &col);
    void setUnmatchedBraceIndicator(int indicatorNumber);
    void resetUnmatchedBraceIndicator();

    void clearIndicatorRange(int lineFrom, int indexFrom, int lineTo,int indexTo, int indicatorNumber);
    void fillIndicatorRange(int lineFrom, int indexFrom, int lineTo,int indexTo, int indicatorNumber);

    SimCommandProxy* standardCommands() const;

    int defineMarker(SimSci::MarkerSymbol sym, int markerNumber = -1);
    int defineMarker(char ch, int markerNumber = -1);
    int defineMarker(const QPixmap &pm, int markerNumber = -1);
    int defineMarker(const QImage &im, int markerNumber = -1);
    int addMarker(int linenr, int markerNumber);
    unsigned markersAtLine(int linenr) const;
    void deleteMarker(int linenr, int markerNumber = -1);
    void deleteAllMarker(int markerNumber = -1);
    void deleteMarkerHandle(int mhandle);
    int markerLine(int mhandle) const;
    int findNextMarker(int linenr, unsigned mask) const;
    int findPreviousMarker(int linenr, unsigned mask) const;

    void moveToMatchingBrace();
    void selectToMatchingBrace();

    void setBraceMatching(SimSci::BraceMatch bm);
    SimSci::BraceMatch braceMatching() const;

    void setBackspaceUnindents(bool unindent);
    bool isBackspaceUnindents() const;

    void setAutoCompletionCaseSensitivity(bool cs);
    bool isAutoCompletionCaseSensitivity() const;

    void setAutoCompletionReplaceWord(bool replace);
    bool isAutoCompletionReplaceWord() const;

    void setAutoCompletionShowSingle(bool single);
    bool isAutoCompletionShowSingle() const;

    void setAutoCompletionSource(SimSci::AutoCompletionSource source);
    SimSci::AutoCompletionSource autoCompletionSource() const;

    void setAutoCompletionThreshold(int thresh);
    int autoCompletionThreshold() const;

    void setAutoCompletionUseSingle(SimSci::AutoCompletionUseSingle single);
    SimSci::AutoCompletionUseSingle autoCompletionUseSingle() const;

    void setAutoCompletionFillupsEnabled(bool enabled);
    bool isAutoCompletionFillupsEnabled() const;

    void setAutoCompletionFillups(const QString& fillups);
    void setAutoCompletionWordSeparators(const QStringList &separators);

    void setFolding(SimSci::FoldStyle fold, int margin = 2);
    SimSci::FoldStyle folding() const;

    void setIndicatorDrawUnder(bool under, int indicatorNumber = -1);
    bool isIndicatorDrawUnder(int indicatorNumber) const;

    void setIndicatorForegroundColor(const QColor &col, int indicatorNumber = -1);
    void setIndicatorOutlineColor(const QColor &col, int indicatorNumber = -1);
    int defineIndicator(SimSci::IndicatorStyle style, int indicatorNumber = -1);

    QStringList getApiContext(int pos, int &context_start,int &last_word_start);
    void showAutoCompleteFromAll();
    void showAutoCompleteFromAPIs();
    void showAutoCompleteFromDocument();
    void showCallTip();

    void startRecord();
    void stopRecord();

    bool loadMacros();
    bool saveMacros();

    SimCommandProxy* commandProxy() const;
    void setShortcut(QAction *action, SimSci::Command cmdId) const;

signals:
    void positionChanged(int pos);
    void positionChanged(int line, int index);
    void textWidthChanged(qreal width);
    void marginWidthChanged(int width);
    void hScrollBarValueChanged(int value);

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual bool event(QEvent *e);

private Q_SLOTS:
    void onZoom(int z);

    void onLexerColorChanged(int style, const QColor& clr);
    void onLexerEolFillChanged(int style, bool fill);
    void onLexerFontChanged(int style, const QFont& f);
    void onLexerBackgroundColorChanged(int style, const QColor& clr);
    void onLexerPropertyChanged(const QString& name, const QString& value);

    void onNotify(SCNotification *pscn);
    void onDwellEnd(int x, int y);
    void onDwellStart(int x, int y);
    void onUpdateUi();
    void onMarginClicked(int position, int modifiers, int margin);

private:

    void initStyles();
    QByteArray convertString2ByteArray(const QString& text);
    QString convertText2String(const char* str);
    void changeBackgroundStyle(int style, const QColor &color);
    void changeForegroundStyle(int style, const QColor &color);
    void changeStyleFont(int style, const QFont& font);
    void foldExpand(int &line, bool doExpand, bool force = false, int visLevels = 0, int level = -1);
    void checkMarker(int &markerNumber);
    void checkIndicator(int &indicatorNumber);
    static void allocateId(int &id, unsigned &allocated, int min, int max);
    void gotoMatchingBrace(bool select);
    bool findMatchingBrace(long &brace, long &other, SimSci::BraceMatch mode);
    long checkBrace(long pos, int brace_style, bool &colonMode);
    void setFoldMarker(int marknr, int mark = SC_MARK_EMPTY);
    void startAutoCompletion(SimSci::AutoCompletionSource acs, bool checkThresh, bool choose_single);
    bool getSeparator(int &pos) const;
    QString getWord(int &pos) const;
    char getCharacter(int &pos) const;
    int getAdjustedCallTipPosition(int ctshift) const;
    void handleLexerSignals(const SimAbstractLexer* lexer);
    void setLexerStyle(int style);
    void detachLexer();
    void notifyPositionChanged();
    void setupConns();
    void resetNullLexer();
    void adjustLineNumberMarginWidth();
    bool isStartChar(char ch) const;
    void maintainIndentation(char ch, long pos);
    void autoIndentLine(long pos, int line, int indent);
    void autoIndentation(char ch, long pos);
    bool rangeIsWhitespace(long spos, long epos);
    int blockIndent(int line);
    QString getApiTip(const QString& api);
    void handleCharAdded(char c);
    void addEnclosingChar(char ch);
    QString getTextRange(int start, int end);
    QString getCurrentWord();
    void initMacro();
    void notifyTextWidthChanged();
    void notifyMarginWidthChanged();
    void applyStyles();
    void setLexerKeywords();

    typedef QPair<QByteArray, QByteArray> TextStylePair;
    TextStylePair styleText(const QList<SimStyledText>& styledtext, int styleOffset);

    SimScintillaPrivate* d;
    Q_DISABLE_COPY(SimScintilla)
};

#endif // SIMSCINTILLA_H
