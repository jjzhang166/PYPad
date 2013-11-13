#ifndef SIMSCINTILLA_DEFS_H
#define SIMSCINTILLA_DEFS_H

#include <Scintilla.h>
#include <SciLexer.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(OBJECT) if (OBJECT != nullptr) { delete OBJECT; }
#endif

#define SIM_STYLE_MAX 128

namespace SimSci {
    enum Technology {
        Tech_Default = SC_TECHNOLOGY_DEFAULT,
        Tech_DirectWrite = SC_TECHNOLOGY_DIRECTWRITE
    };

    //! 此枚举类型定义了Lexer支持的语言类型.
    enum Language {
        //! 非编程语言
        Lang_None,
        //! C++
        Lang_Cpp,
        //! Javascript
        Lang_Js,
        //! APDL
        Lang_Apdl
    };

    //! 此枚举类型定义了换行的方式
    enum Wrap {
        //! 不换行
        Wrap_None = SC_WRAP_NONE,
        //! 以词换行
        Wrap_Word= SC_WRAP_WORD,
        //! 以字符换行
        Wrap_Char = SC_WRAP_CHAR
    };

    enum WrapIndent {
        WI_Fixed = SC_WRAPINDENT_FIXED,
        WI_Same = SC_WRAPINDENT_SAME,
        WI_Indented = SC_WRAPINDENT_INDENT
    };

    //! 此枚举类型定义了注释风格
    enum AnnotationStyle {
        //! 隐藏注释
        AS_Hidden = ANNOTATION_HIDDEN,
        //! 标准注释
        AS_Standard = ANNOTATION_STANDARD,
        //!
        AS_Boxed = ANNOTATION_BOXED
    };

    enum Edge {
        Edge_None = EDGE_NONE,
        Edge_Line = EDGE_LINE,
        Edge_Background = EDGE_BACKGROUND
    };

    //! 此枚举类型定义了鼠标的形状
    enum Cursor {
        //! 正常
        Cursor_Normal = SC_CURSORNORMAL,
        //! 箭头
        Cursor_Arrrow = SC_CURSORARROW,
        //! 等待状态
        Cursor_Wait = SC_CURSORWAIT,
        //! 反向箭头
        Cursor_ReverseArrow = SC_CURSORREVERSEARROW
    };

    //! 此枚举类型定义了字体加粗的风格
    enum Weight {
        //! 正常
        Weight_Normal = SC_WEIGHT_NORMAL,
        //! 半加粗
        Weight_Semibold = SC_WEIGHT_SEMIBOLD,
        //! 加粗
        Weight_Bold = SC_WEIGHT_BOLD
    };

    enum CaretStyle {
        CS_Invisible = CARETSTYLE_INVISIBLE,
        CS_Line = CARETSTYLE_LINE,
        CS_Block = CARETSTYLE_BLOCK
    };

    enum Caret {
        Caret_Slop = CARET_SLOP,
        Caret_Strict = CARET_STRICT,
        Caret_Jumps = CARET_JUMPS,
        Caret_Even = CARET_EVEN
    };

    enum Cache {
        Cache_None = SC_CACHE_NONE,
        Cache_Caret = SC_CACHE_CARET,
        Cache_Page = SC_CACHE_PAGE,
        Cache_Document = SC_CACHE_DOCUMENT
    };

    //! 此枚举类型定义了空白符的可见风格
    enum WhitespaceVisibility {
        //! 不可见
        WSV_Invisible = SCWS_INVISIBLE,
        //! 总是可见
        WSV_Visible = SCWS_VISIBLEALWAYS,
        //! 缩进后可见
        WSV_AfterIndent = SCWS_VISIBLEAFTERINDENT
    };

    enum MarkerSymbol {
        MS_Circle = SC_MARK_CIRCLE,
        MS_RoundRect = SC_MARK_ROUNDRECT,
        MS_Arrow = SC_MARK_ARROW,
        MS_SmallRect = SC_MARK_SMALLRECT,
        MS_ShortArrow = SC_MARK_SHORTARROW,
        MS_Empty = SC_MARK_EMPTY,
        MS_ArrowDown = SC_MARK_ARROWDOWN,
        MS_Minus = SC_MARK_MINUS,
        MS_Plus = SC_MARK_PLUS,
        MS_VLine = SC_MARK_VLINE,
        MS_LCorner = SC_MARK_LCORNER,
        MS_TCorner = SC_MARK_TCORNER,
        MS_BoxPlus = SC_MARK_BOXPLUS,
        MS_BoxPlusConnected = SC_MARK_BOXPLUSCONNECTED,
        MS_BoxMinus = SC_MARK_BOXMINUS,
        MS_BoxMinusConnected = SC_MARK_BOXMINUSCONNECTED,
        MS_LCornerCurve = SC_MARK_LCORNERCURVE,
        MS_TCornerCurve = SC_MARK_TCORNERCURVE,
        MS_CirclePlus = SC_MARK_CIRCLEPLUS,
        MS_CirclePlusConnected = SC_MARK_CIRCLEPLUSCONNECTED,
        MS_CircleMinus = SC_MARK_CIRCLEMINUS,
        MS_CircleMinusConnected = SC_MARK_CIRCLEMINUSCONNECTED,
        MS_Background = SC_MARK_BACKGROUND,
        MS_DotDotDot = SC_MARK_DOTDOTDOT,
        MS_Arrows = SC_MARK_ARROWS,
        MS_Pixmap = SC_MARK_PIXMAP,
        MS_FullRect = SC_MARK_FULLRECT,
        MS_LeftRect = SC_MARK_LEFTRECT,
        MS_Available = SC_MARK_AVAILABLE,
        MS_Underline = SC_MARK_UNDERLINE,
        MS_RgbImage = SC_MARK_RGBAIMAGE,
        MS_Character = SC_MARK_CHARACTER
    };

    enum EolMode {
        Eol_Windows = SC_EOL_CRLF,
        Eol_Mac = SC_EOL_CR,
        Eol_Unix = SC_EOL_LF
    };

    enum SelectionMode {
        SM_Stream = SC_SEL_STREAM,
        SM_Rectangle = SC_SEL_RECTANGLE,
        SM_Lines = SC_SEL_LINES,
        SM_Thin = SC_SEL_THIN
    };

    enum UpdateMode {
        UM_Content = SC_UPDATE_CONTENT,
        UM_Selection = SC_UPDATE_SELECTION,
        UM_VScroll = SC_UPDATE_V_SCROLL,
        UM_HScroll = SC_UPDATE_H_SCROLL
    };

    enum MarginOption {
        MO_None = SC_MARGINOPTION_NONE,
        MO_SubLineSelect = SC_MARGINOPTION_SUBLINESELECT
    };

    enum IndicatorStyle {
        IS_Plain = INDIC_PLAIN,
        IS_Squiggle = INDIC_SQUIGGLE,
        IS_Tt = INDIC_TT,
        IS_Diagonal = INDIC_DIAGONAL,
        IS_Strike = INDIC_STRIKE,
        IS_Hidden = INDIC_HIDDEN,
        IS_Box = INDIC_BOX,
        IS_RoundBox = INDIC_ROUNDBOX,
        IS_StraightBox = INDIC_STRAIGHTBOX,
        IS_Dash = INDIC_DASH,
        IS_Dots = INDIC_DOTS,
        IS_Max = INDIC_MAX
    };

    enum FoldStyle {
        //! Folding is disabled.
        FS_None,

        //! Plain folding style using plus and minus symbols.
        FS_Plain,

        //! Circled folding style using circled plus and minus symbols.
        FS_Circled,

        //! Boxed folding style using boxed plus and minus symbols.
        FS_Boxed,

        //! Circled tree style using a flattened tree with circled plus and
        //! minus symbols and rounded corners.
        FS_CircledTree,

        //! Boxed tree style using a flattened tree with boxed plus and minus
        //! symbols and right-angled corners.
        FS_BoxedTree
    };

    //! This enum defines the different margin types.
    enum MarginType {
        //! The margin contains symbols, including those used for folding.
        MT_Symbol= SC_MARGIN_SYMBOL,

        //! The margin contains symbols and uses the default foreground color
        //! as its background color.
        MT_DefaultForegroundColor = SC_MARGIN_FORE,

        //! The margin contains symbols and uses the default background color
        //! as its background color.
        MT_DefaultBackgroundColor = SC_MARGIN_BACK,

        //! The margin contains line numbers.
        MT_Number = SC_MARGIN_NUMBER,

        //! The margin contains styled text.
        MT_Text = SC_MARGIN_TEXT,

        //! The margin contains right justified styled text.
        MT_TextRightJustified = SC_MARGIN_RTEXT
    };

    //! This enum defines the different brace matching modes.  The character
    //! pairs {}, [] and () are treated as braces.  The Python lexer will also
    //! match a : with the end of the corresponding indented block.
    enum BraceMatch {
        //! Brace matching is disabled.
        BM_None,

        //! Brace matching is enabled for a brace immediately before the
        //! current position.
        BM_Strict,

        //! Brace matching is enabled for a brace immediately before or after
        //! the current position.
        BM_Sloppy
    };

    //! This enum defines the behavior if an auto-completion list contains a
    //! single entry.
    enum AutoCompletionUseSingle {
        //! The single entry is not used automatically and the auto-completion
        //! list is displayed.
        ACUS_Never,

        //! The single entry is used automatically when auto-completion is
        //! explicitly requested (using autoCompleteFromAPIs() or
        //! autoCompleteFromDocument()) but not when auto-completion is
        //! triggered as the user types.
        ACUS_Explicit,

        //! The single entry is used automatically and the auto-completion list
        //! is not displayed.
        ACUS_Always
    };

    //! This enum defines the different sources for auto-completion lists.
    enum AutoCompletionSource {
        //! No sources are used, ie. automatic auto-completion is disabled.
        ACS_None,

        //! The source is all available sources.
        ACS_All,

        //! The source is the current document.
        ACS_Document,

        //! The source is any installed APIs.
        ACS_APIs
    };

    //! This enum defines the different line wrap visual flags.
    enum WrapVisualFlag {
        //! No wrap flag is displayed.
        WVF_None,

        //! A wrap flag is displayed by the text.
        WVF_ByText,

        //! A wrap flag is displayed by the border.
        WVF_ByBorder,

        //! A wrap flag is displayed in the line number margin.
        WVF_InMargin
    };

    enum Style
    {
        Style_Default = 32,
        Style_LineNumber = 33,
        Style_BraceLight = 34,
        Style_BraceBad = 35,
        Style_ControlChar = 36,
        Style_IndentGuide = 37,
        Style_CallTip = 38,
        Style_LastPredefined = 39,
        Style_Max = 255
    };

    //! This enum defines the different auto-indentation styles.
    enum AutoIndentStyle {

        AIS_None = 0x00,

        //! A line is automatically indented to match the previous line.
        AIS_Maintain = 0x01,

        //! If the language supported by the current lexer has a specific start
        //! of block character (e.g. { in C++), then a line that begins with
        //! that character is indented as well as the lines that make up the
        //! block.  It may be logically ored with AiClosing.
        AIS_Opening = 0x02,

        //! If the language supported by the current lexer has a specific end
        //! of block character (e.g. } in C++), then a line that begins with
        //! that character is indented as well as the lines that make up the
        //! block.  It may be logically ored with AiOpening.
        AIS_Closing = 0x04
    };

    //! This enum defines the different indentation guide views.
    //!
    //! \sa SCI_GETINDENTATIONGUIDES, SCI_SETINDENTATIONGUIDES
    enum IndentationGuideView
    {
        //! No indentation guides are shown.
        IGV_None = SC_IV_NONE,

        //! Indentation guides are shown inside real indentation white space.
        IGV_Real = SC_IV_REAL,

        //! Indentation guides are shown beyond the actual indentation up to
        //! the level of the next non-empty line.  If the previous non-empty
        //! line was a fold header then indentation guides are shown for one
        //! more level of indent than that line.  This setting is good for
        //! Python.
        IGV_LookForward = SC_IV_LOOKFORWARD,

        //! Indentation guides are shown beyond the actual indentation up to
        //! the level of the next non-empty line or previous non-empty line
        //! whichever is the greater.  This setting is good for most languages.
        IGV_LookBoth = SC_IV_LOOKBOTH
    };

//    ! 此枚举类型定义了不同的按键修饰符
//    enum Modifier
//    {
//        //! No modifier key.
//        SCMOD_NORM = 0,

//        //! Shift key.
//        SCMOD_SHIFT = 1,

//        //! Control key (the Command key on OS/X, the Ctrl key on other
//        //! platforms).
//        SCMOD_CTRL = 2,

//        //! Alt key.
//        SCMOD_ALT = 4,

//        //! This is the same as SCMOD_META on all platforms.
//        SCMOD_SUPER = 8,

//        //! Meta key (the Ctrl key on OS/X, the Windows key on other
//        //! platforms).
//        SCMOD_META = 16
//    };

    enum Command {
        CMD_Start = SCI_START,
        CMD_OptionalStart = SCI_OPTIONAL_START,
        CMD_LexerStart = SCI_LEXER_START,
        CMD_AddText = SCI_ADDTEXT,
        CMD_AddStyledText = SCI_ADDSTYLEDTEXT,
        CMD_InsertText = SCI_INSERTTEXT,
        CMD_ClearAll = SCI_CLEARALL,
        CMD_ClearDocumentStyle = SCI_CLEARDOCUMENTSTYLE,
        CMD_GetLength = SCI_GETLENGTH,
        CMD_GetCharAt = SCI_GETCHARAT,
        CMD_GetCurrentPos = SCI_GETCURRENTPOS,
        CMD_GetAnchor = SCI_GETANCHOR,
        CMD_GetStyleAt = SCI_GETSTYLEAT,
        CMD_Redo = SCI_REDO,
        CMD_SetUndoCollection = SCI_SETUNDOCOLLECTION,
        CMD_SelectAll = SCI_SELECTALL,
        CMD_SetSavePoint = SCI_SETSAVEPOINT,
        CMD_GetStyledText = SCI_GETSTYLEDTEXT,
        CMD_CanRedo = SCI_CANREDO,
        CMD_MarkerLineFromHandle = SCI_MARKERLINEFROMHANDLE,
        CMD_MarkerDeleteHandle = SCI_MARKERDELETEHANDLE,
        CMD_GetUndoCollection = SCI_GETUNDOCOLLECTION,
        CMD_GetViewWS = SCI_GETVIEWWS,
        CMD_SetViewWS = SCI_SETVIEWWS,
        CMD_PositionFromPoint = SCI_POSITIONFROMPOINT,
        CMD_PositionFromPointClose = SCI_POSITIONFROMPOINTCLOSE,
        CMD_GotoLine = SCI_GOTOLINE,
        CMD_GotoPos = SCI_GOTOPOS,
        CMD_SetAnchor = SCI_SETANCHOR,
        CMD_GetCurLine = SCI_GETCURLINE,
        CMD_GetEndStyled = SCI_GETENDSTYLED,
        CMD_ConvertEols = SCI_CONVERTEOLS,
        CMD_GetEolMode = SCI_GETEOLMODE,
        CMD_SetEolMode = SCI_SETEOLMODE,
        CMD_StartStyling = SCI_STARTSTYLING,
        CMD_SetStyling = SCI_SETSTYLING,
        CMD_GetBufferedDraw = SCI_GETBUFFEREDDRAW,
        CMD_SetBufferedDraw = SCI_SETBUFFEREDDRAW,
        CMD_SetTabWidth = SCI_SETTABWIDTH,
        CMD_GetTabWidth = SCI_GETTABWIDTH,
        CMD_SetCodePage = SCI_SETCODEPAGE,
        CMD_MarkerDefine = SCI_MARKERDEFINE,
        CMD_MarkerSetFore = SCI_MARKERSETFORE,
        CMD_MarkerSetBack = SCI_MARKERSETBACK,
        CMD_MarkerAdd = SCI_MARKERADD,
        CMD_MarkerDelete = SCI_MARKERDELETE,
        CMD_MarkerDeleteAll = SCI_MARKERDELETEALL,
        CMD_MarkerGet = SCI_MARKERGET,
        CMD_MarkerNext = SCI_MARKERNEXT,
        CMD_MarkerPrevious = SCI_MARKERPREVIOUS,
        CMD_MarkerDefinePixmap = SCI_MARKERDEFINEPIXMAP,
        CMD_SetMarginTypeN = SCI_SETMARGINTYPEN,
        CMD_GetMarginTypeN = SCI_GETMARGINTYPEN,
        CMD_SetMarginWidthN = SCI_SETMARGINWIDTHN,
        CMD_GetMarginWidthN = SCI_GETMARGINWIDTHN,
        CMD_SetMarginMaskN = SCI_SETMARGINMASKN,
        CMD_GetMarginMaskN = SCI_GETMARGINMASKN,
        CMD_SetMarginSensitiveN = SCI_SETMARGINSENSITIVEN,
        CMD_GetMarginSensitiveN = SCI_GETMARGINSENSITIVEN,
        CMD_SetMarginCursorN = SCI_SETMARGINCURSORN,
        CMD_GetMarginCursorN = SCI_GETMARGINCURSORN,
        CMD_StyleClearAll = SCI_STYLECLEARALL,
        CMD_StyleSetFore = SCI_STYLESETFORE,
        CMD_StyleSetBack = SCI_STYLESETBACK,
        CMD_StyleSetBold = SCI_STYLESETBOLD,
        CMD_StyleSetItalic = SCI_STYLESETITALIC,
        CMD_StyleSetSize = SCI_STYLESETSIZE,
        CMD_StyleSetFont = SCI_STYLESETFONT,
        CMD_StyleSetEolFilled = SCI_STYLESETEOLFILLED,
        CMD_StyleResetDefault = SCI_STYLERESETDEFAULT,
        CMD_StyleSetUnderline = SCI_STYLESETUNDERLINE,
        CMD_StyleSetCase = SCI_STYLESETCASE,
        CMD_StyleSetSizeFractional = SCI_STYLESETSIZEFRACTIONAL,
        CMD_StyleGetSizeFractional = SCI_STYLEGETSIZEFRACTIONAL,
        CMD_StyleSetWeight = SCI_STYLESETWEIGHT,
        CMD_StyleGetWeight = SCI_STYLEGETWEIGHT,
        CMD_StyleSetCharacterSet = SCI_STYLESETCHARACTERSET,
        CMD_SetSelFore = SCI_SETSELFORE,
        CMD_SetSelBack = SCI_SETSELBACK,
        CMD_SetCaretFore = SCI_SETCARETFORE,
        CMD_AssignCMDKey = SCI_ASSIGNCMDKEY,
        CMD_ClearCMDKey = SCI_CLEARCMDKEY,
        CMD_ClearAllCMDKeys = SCI_CLEARALLCMDKEYS,
        CMD_SetStylingEx = SCI_SETSTYLINGEX,
        CMD_StyleSetVisible = SCI_STYLESETVISIBLE,
        CMD_GetCaretPeriod = SCI_GETCARETPERIOD,
        CMD_SetCaretPeriod = SCI_SETCARETPERIOD,
        CMD_SetWordChars = SCI_SETWORDCHARS,
        CMD_BeginUndoAction = SCI_BEGINUNDOACTION,
        CMD_EndUndoAction = SCI_ENDUNDOACTION,
        CMD_IndicSetStyle = SCI_INDICSETSTYLE,
        CMD_IndicGetStyle = SCI_INDICGETSTYLE,
        CMD_IndicSetFore = SCI_INDICSETFORE,
        CMD_IndicGetFore = SCI_INDICGETFORE,
        CMD_SetWhitespaceFore = SCI_SETWHITESPACEFORE,
        CMD_SetWhitespaceBack = SCI_SETWHITESPACEBACK,
        CMD_SetWhitespaceSize = SCI_SETWHITESPACESIZE,
        CMD_GetWhitespaceSize = SCI_GETWHITESPACESIZE,
        CMD_SetStyleBits = SCI_SETSTYLEBITS,
        CMD_GetStyleBits = SCI_GETSTYLEBITS,
        CMD_SetLineState = SCI_SETLINESTATE,
        CMD_GetLineState = SCI_GETLINESTATE,
        CMD_GetMaxLineState = SCI_GETMAXLINESTATE,
        CMD_GetCaretLineVisible = SCI_GETCARETLINEVISIBLE,
        CMD_SetCaretLineVisible = SCI_SETCARETLINEVISIBLE,
        CMD_GetCaretLineBack = SCI_GETCARETLINEBACK,
        CMD_SetCaretLineBack = SCI_SETCARETLINEBACK,
        CMD_StyleSetChangeable = SCI_STYLESETCHANGEABLE,
        CMD_AutoCShow = SCI_AUTOCSHOW,
        CMD_AutoCCancel = SCI_AUTOCCANCEL,
        CMD_AutoCActive = SCI_AUTOCACTIVE,
        CMD_AutoCPosStart = SCI_AUTOCPOSSTART,
        CMD_AutoCComplete = SCI_AUTOCCOMPLETE,
        CMD_AutoCStops = SCI_AUTOCSTOPS,
        CMD_AutoCSetSeparator = SCI_AUTOCSETSEPARATOR,
        CMD_AutoCGetSeparator = SCI_AUTOCGETSEPARATOR,
        CMD_AutoCSelect = SCI_AUTOCSELECT,
        CMD_AutoCSetCancelAtStart = SCI_AUTOCSETCANCELATSTART,
        CMD_AutoCGetCancelAtStart = SCI_AUTOCGETCANCELATSTART,
        CMD_AutoCSetFillUps = SCI_AUTOCSETFILLUPS,
        CMD_AutoCSetChooseSingle = SCI_AUTOCSETCHOOSESINGLE,
        CMD_AutoCGetChooseSingle = SCI_AUTOCGETCHOOSESINGLE,
        CMD_AutoCSetIgnoreCase = SCI_AUTOCSETIGNORECASE,
        CMD_AutoCGetIgnoreCase = SCI_AUTOCGETIGNORECASE,
        CMD_UserListShow = SCI_USERLISTSHOW,
        CMD_AutoCSetAutoHide = SCI_AUTOCSETAUTOHIDE,
        CMD_AutoCGetAutoHide = SCI_AUTOCGETAUTOHIDE,
        CMD_AutoCSetDropRestOfWord = SCI_AUTOCSETDROPRESTOFWORD,
        CMD_AutoCGetDropRestOfWord = SCI_AUTOCGETDROPRESTOFWORD,
        CMD_SetIndent = SCI_SETINDENT,
        CMD_GetIndent = SCI_GETINDENT,
        CMD_SetUseTabs = SCI_SETUSETABS,
        CMD_GetUseTabs = SCI_GETUSETABS,
        CMD_SetLineIndentation = SCI_SETLINEINDENTATION,
        CMD_GetLineIndentation = SCI_GETLINEINDENTATION,
        CMD_GetLineIndentPosition = SCI_GETLINEINDENTPOSITION,
        CMD_GetColumn = SCI_GETCOLUMN,
        CMD_SetHScrollBar = SCI_SETHSCROLLBAR,
        CMD_GetHScrollBar = SCI_GETHSCROLLBAR,
        CMD_SetIndentationGuides = SCI_SETINDENTATIONGUIDES,
        CMD_GetIndentationGuides = SCI_GETINDENTATIONGUIDES,
        CMD_SetHighlightGuide = SCI_SETHIGHLIGHTGUIDE,
        CMD_GetHighlightGuide = SCI_GETHIGHLIGHTGUIDE,
        CMD_GetLineEndPosition = SCI_GETLINEENDPOSITION,
        CMD_GetCodePage = SCI_GETCODEPAGE,
        CMD_GetCaretFore = SCI_GETCARETFORE,
        CMD_GetReadOnly = SCI_GETREADONLY,
        CMD_SetCurrentPos = SCI_SETCURRENTPOS,
        CMD_SetSelectionStart = SCI_SETSELECTIONSTART,
        CMD_GetSelectionStart = SCI_GETSELECTIONSTART,
        CMD_SetSelectionEnd = SCI_SETSELECTIONEND,
        CMD_GetSelectionEnd = SCI_GETSELECTIONEND,
        CMD_SetPrintMagnification = SCI_SETPRINTMAGNIFICATION,
        CMD_GetPrintMagnification = SCI_GETPRINTMAGNIFICATION,
        CMD_SetPrintColourMode = SCI_SETPRINTCOLOURMODE,
        CMD_GetPrintColourMode = SCI_GETPRINTCOLOURMODE,
        CMD_FindText = SCI_FINDTEXT,
        CMD_FormatRange = SCI_FORMATRANGE,
        CMD_GetFirstVisibleLine = SCI_GETFIRSTVISIBLELINE,
        CMD_GetLine = SCI_GETLINE,
        CMD_GetLineCount = SCI_GETLINECOUNT,
        CMD_SetMarginLeft = SCI_SETMARGINLEFT,
        CMD_GetMarginLeft = SCI_GETMARGINLEFT,
        CMD_SetMarginRight = SCI_SETMARGINRIGHT,
        CMD_GetMarginRight = SCI_GETMARGINRIGHT,
        CMD_SetModify = SCI_GETMODIFY,
        CMD_SetSel = SCI_SETSEL,
        CMD_GetSelText = SCI_GETSELTEXT,
        CMD_GetTextRange = SCI_GETTEXTRANGE,
        CMD_HideSelection = SCI_HIDESELECTION,
        CMD_PointXFromPosition = SCI_POINTXFROMPOSITION,
        CMD_PointYFromPosition = SCI_POINTYFROMPOSITION,
        CMD_LineFromPosition = SCI_LINEFROMPOSITION,
        CMD_PositionFromLine = SCI_POSITIONFROMLINE,
        CMD_LineScroll = SCI_LINESCROLL,
        CMD_ScrollCaret = SCI_SCROLLCARET,
        CMD_ReplaceSel = SCI_REPLACESEL,
        CMD_SetReadOnly = SCI_SETREADONLY,
        CMD_Null = SCI_NULL,
        CMD_CanPaste = SCI_CANPASTE,
        CMD_CanUndo = SCI_CANUNDO,
        CMD_EmptyUndoBuffer = SCI_EMPTYUNDOBUFFER,
        CMD_Undo = SCI_UNDO,
        CMD_Cut = SCI_CUT,
        CMD_Copy = SCI_COPY,
        CMD_Paste = SCI_PASTE,
        CMD_Clear = SCI_CLEAR,
        CMD_SetText = SCI_SETTEXT,
        CMD_GetText = SCI_GETTEXT,
        CMD_GetTextLength = SCI_GETTEXTLENGTH,
        CMD_GetDirectionFunction = SCI_GETDIRECTFUNCTION,
        CMD_GetDirectPointer = SCI_GETDIRECTPOINTER,
        CMD_SetOvertype = SCI_SETOVERTYPE,
        CMD_GetOvertype = SCI_GETOVERTYPE,
        CMD_SetCaretWidth = SCI_SETCARETWIDTH,
        CMD_GetCaretWidth = SCI_GETCARETWIDTH,
        CMD_SetTargetStart = SCI_SETTARGETSTART,
        CMD_GetTargetStart = SCI_GETTARGETSTART,
        CMD_SetTargetEnd = SCI_SETTARGETEND,
        CMD_GetTargetEnd = SCI_GETTARGETEND,
        CMD_ReplaceTarget = SCI_REPLACETARGET,
        CMD_ReplaceTargetRe = SCI_REPLACETARGETRE,
        CMD_SearchInTarget = SCI_SEARCHINTARGET,
        CMD_SetSearchFlags = SCI_SETSEARCHFLAGS,
        CMD_GetSearchFlags = SCI_GETSEARCHFLAGS,
        CMD_CallTipShow = SCI_CALLTIPSHOW,
        CMD_CallTipCancel = SCI_CALLTIPCANCEL,
        CMD_CallTipActive = SCI_CALLTIPACTIVE,
        CMD_CallTipPosStart = SCI_CALLTIPPOSSTART,
        CMD_CallTipSEtHlt = SCI_CALLTIPSETHLT,
        CMD_CallTipSetBack = SCI_CALLTIPSETBACK,
        CMD_CallTipSetFore = SCI_CALLTIPSETFORE,
        CMD_CallTipSetForeHlt = SCI_CALLTIPSETFOREHLT,
        CMD_AutoCSetMaxWidth = SCI_AUTOCSETMAXWIDTH,
        CMD_AutoCGetMaxWidth = SCI_AUTOCGETMAXWIDTH,
        CMD_AutoCSetMaxHeight = SCI_AUTOCSETMAXHEIGHT,
        CMD_AutoCGetMaxHeight = SCI_AUTOCGETMAXHEIGHT,
        CMD_CallTipUseStyle = SCI_CALLTIPUSESTYLE,
        CMD_CallTipSetPosition = SCI_CALLTIPSETPOSITION,
        CMD_VisibleFromDocLine = SCI_VISIBLEFROMDOCLINE,
        CMD_DocLineFromVisible = SCI_DOCLINEFROMVISIBLE,
        CMD_SetFoldLevel = SCI_SETFOLDLEVEL,
        CMD_GetFoldLevel = SCI_GETFOLDLEVEL,
        CMD_GetLastChild = SCI_GETLASTCHILD,
        CMD_GetFoldParent = SCI_GETFOLDPARENT,
        CMD_ShowLines = SCI_SHOWLINES,
        CMD_HideLines = SCI_HIDELINES,
        CMD_GetLineVisible = SCI_GETLINEVISIBLE,
        CMD_SetFoldExpanded = SCI_SETFOLDEXPANDED,
        CMD_GetFoldExpanded = SCI_GETFOLDEXPANDED,
        CMD_ToggleFold = SCI_TOGGLEFOLD,
        CMD_EnsureVisible = SCI_ENSUREVISIBLE,
        CMD_SetFoldFlags = SCI_SETFOLDFLAGS,
        CMD_EnsureVisibleEnforcePolicy = SCI_ENSUREVISIBLEENFORCEPOLICY,
        CMD_WrapCount = SCI_WRAPCOUNT,
        CMD_GetAllLinesVisible = SCI_GETALLLINESVISIBLE,
        CMD_SetTabIndents = SCI_SETTABINDENTS,
        CMD_GetTabIndents = SCI_GETTABINDENTS,
        CMD_SetBackspaceUnindents = SCI_SETBACKSPACEUNINDENTS,
        CMD_GetBackspaceUnindents = SCI_GETBACKSPACEUNINDENTS,
        CMD_SetMouseDwellTime = SCI_SETMOUSEDWELLTIME,
        CMD_GetMouseDwellTime = SCI_GETMOUSEDWELLTIME,
        CMD_WordStartPosition = SCI_WORDSTARTPOSITION,
        CMD_WordEndPosition = SCI_WORDENDPOSITION,
        CMD_SetWrapMode = SCI_SETWRAPMODE,
        CMD_GetWrapMode = SCI_GETWRAPMODE,
        CMD_SetLayoutCache = SCI_SETLAYOUTCACHE,
        CMD_GetLayoutCache = SCI_GETLAYOUTCACHE,
        CMD_SetScrollWidth = SCI_SETSCROLLWIDTH,
        CMD_GetScrollWidth = SCI_GETSCROLLWIDTH,
        CMD_TextWidth = SCI_TEXTWIDTH,
        CMD_SetEndAtLastLine = SCI_SETENDATLASTLINE,
        CMD_GetEndAtLastLine = SCI_GETENDATLASTLINE,
        CMD_TextHeight = SCI_TEXTHEIGHT,
        CMD_SetVScrollBar = SCI_SETVSCROLLBAR,
        CMD_GetVScrollBar = SCI_GETVSCROLLBAR,
        CMD_AppendText = SCI_APPENDTEXT,
        CMD_GetTwoPhaseDraw = SCI_GETTWOPHASEDRAW,
        CMD_SetTwoPhaseDraw = SCI_SETTWOPHASEDRAW,
        CMD_AutoCGetTyleSeparator = SCI_AUTOCGETTYPESEPARATOR,
        CMD_AutoCSetTyleSeparator = SCI_AUTOCSETTYPESEPARATOR,
        CMD_TargetFromSelection = SCI_TARGETFROMSELECTION,
        CMD_LinesJoin = SCI_LINESJOIN,
        CMD_LinesSplit = SCI_LINESSPLIT,
        CMD_SetFoldMarginColour = SCI_SETFOLDMARGINCOLOUR,
        CMD_SetFoldMarginHiColour = SCI_SETFOLDMARGINHICOLOUR,
        CMD_MarkerSetBackSelected = SCI_MARKERSETBACKSELECTED,
        CMD_MarkerEnableHighlight = SCI_MARKERENABLEHIGHLIGHT,
        CMD_LineDown = SCI_LINEDOWN,
        CMD_LineDownExtend = SCI_LINEDOWNEXTEND,
        CMD_LineUp = SCI_LINEUP,
        CMD_LineUpExtend = SCI_LINEUPEXTEND,
        CMD_CharLeft = SCI_CHARLEFT,
        CMD_CharLeftExtend = SCI_CHARLEFTEXTEND,
        CMD_CharRight = SCI_CHARRIGHT,
        CMD_CharRightExtend = SCI_CHARRIGHTEXTEND,
        CMD_WordLeft = SCI_WORDLEFT,
        CMD_WordLeftExtend = SCI_WORDLEFTEXTEND,
        CMD_WordRight = SCI_WORDRIGHT,
        CMD_WordRightExtend = SCI_WORDRIGHTEXTEND,
        CMD_Home = SCI_HOME,
        CMD_HomeExtend = SCI_HOMEEXTEND,
        CMD_LineEnd = SCI_LINEEND,
        CMD_LineEndExtend = SCI_LINEENDEXTEND,
        CMD_DocumentStart = SCI_DOCUMENTSTART,
        CMD_DocumentStartExtend = SCI_DOCUMENTSTARTEXTEND,
        CMD_DocumentEnd = SCI_DOCUMENTEND,
        CMD_DocumentEndExtend = SCI_DOCUMENTENDEXTEND,
        CMD_PageUp = SCI_PAGEUP,
        CMD_PageUpExtend = SCI_PAGEUPEXTEND,
        CMD_PageDown = SCI_PAGEDOWN,
        CMD_PageDownExtend = SCI_PAGEDOWNEXTEND,
        CMD_EditToggleOvertype = SCI_EDITTOGGLEOVERTYPE,
        CMD_Cancel = SCI_CANCEL,
        CMD_DeleteBack = SCI_DELETEBACK,
        CMD_Tab = SCI_TAB,
        CMD_BackTab = SCI_BACKTAB,
        CMD_NewLine = SCI_NEWLINE,
        CMD_FormFeed = SCI_FORMFEED,
        CMD_VCHome = SCI_VCHOME,
        CMD_VCHomeExtend = SCI_VCHOMEEXTEND,
        CMD_ZoomIn = SCI_ZOOMIN,
        CMD_ZoomOut = SCI_ZOOMOUT,
        CMD_DelWordLeft = SCI_DELWORDLEFT,
        CMD_DelWordRight = SCI_DELWORDRIGHT,
        CMD_LineCut = SCI_LINECUT,
        CMD_LineDelete = SCI_LINEDELETE,
        CMD_LineTranspose = SCI_LINETRANSPOSE,
        CMD_LowerCase = SCI_LOWERCASE,
        CMD_UpperCase = SCI_UPPERCASE,
        CMD_LineScrollDown = SCI_LINESCROLLDOWN,
        CMD_LineScrollUp = SCI_LINESCROLLUP,
        CMD_DeleteBackNotLine = SCI_DELETEBACKNOTLINE,
        CMD_HomeDisplay = SCI_HOMEDISPLAY,
        CMD_HomeDisplayExtend = SCI_HOMEDISPLAYEXTEND,
        CMD_LineEndDisplay = SCI_LINEENDDISPLAY,
        CMD_LineEndDisplayExtend = SCI_LINEENDDISPLAYEXTEND,
        CMD_MoveCaretInsideView = SCI_MOVECARETINSIDEVIEW,
        CMD_LineLength = SCI_LINELENGTH,
        CMD_BraceHighlight = SCI_BRACEHIGHLIGHT,
        CMD_BraceBadlight = SCI_BRACEBADLIGHT,
        CMD_BraceMatch = SCI_BRACEMATCH,
        CMD_GetViewEol = SCI_GETVIEWEOL,
        CMD_SetViewEol = SCI_SETVIEWEOL,
        CMD_GetDocPointer = SCI_GETDOCPOINTER,
        CMD_SetDocPointer = SCI_SETDOCPOINTER,
        CMD_SetModEventMask = SCI_SETMODEVENTMASK,
        CMD_GetEdgeColumn = SCI_GETEDGECOLUMN,
        CMD_SetEdgeColumn = SCI_SETEDGECOLUMN,
        CMD_GetEdgeMode = SCI_GETEDGEMODE,
        CMD_SetEdgeMode = SCI_SETEDGEMODE,
        CMD_GetEdgeColour = SCI_GETEDGECOLOUR,
        CMD_SetEdgeColour = SCI_SETEDGECOLOUR,
        CMD_SearchAnchor = SCI_SEARCHANCHOR,
        CMD_SearchNext = SCI_SEARCHNEXT,
        CMD_SearchPrev = SCI_SEARCHPREV,
        CMD_LinesOnScreen = SCI_LINESONSCREEN,
        CMD_UsePopup = SCI_USEPOPUP,
        CMD_SelectionIsRectangle = SCI_SELECTIONISRECTANGLE,
        CMD_SetZoom = SCI_SETZOOM,
        CMD_GetZoom = SCI_GETZOOM,
        CMD_CreateDocument = SCI_CREATEDOCUMENT,
        CMD_AddRefDocument = SCI_ADDREFDOCUMENT,
        CMD_ReleaseDocument = SCI_RELEASEDOCUMENT,
        CMD_GetModEventMask = SCI_GETMODEVENTMASK,
        CMD_SetFocus = SCI_SETFOCUS,
        CMD_GetFocus = SCI_GETFOCUS,
        CMD_SetStatus = SCI_SETSTATUS,
        CMD_GetStatus = SCI_GETSTATUS,
        CMD_SetMouseDownCaptures = SCI_SETMOUSEDOWNCAPTURES,
        CMD_GetMouseDownCaptures = SCI_GETMOUSEDOWNCAPTURES,
        CMD_SetCursor = SCI_SETCURSOR,
        CMD_GetCursor = SCI_GETCURSOR,
        CMD_SetControlCharSymbol = SCI_SETCONTROLCHARSYMBOL,
        CMD_GetControlCharSymbol = SCI_GETCONTROLCHARSYMBOL,
        CMD_WordPartLeft = SCI_WORDPARTLEFT,
        CMD_WordPartLeftExtend = SCI_WORDPARTLEFTEXTEND,
        CMD_WordPartRight = SCI_WORDPARTRIGHT,
        CMD_WordPartRightExtend = SCI_WORDPARTRIGHTEXTEND,
        CMD_SetVisiblePolicy = SCI_SETVISIBLEPOLICY,
        CMD_DelLineLeft = SCI_DELLINELEFT,
        CMD_DelLineRight = SCI_DELLINERIGHT,
        CMD_SetXOffset = SCI_SETXOFFSET,
        CMD_GetXOffset = SCI_GETXOFFSET,
        CMD_ChooseCaretX = SCI_CHOOSECARETX,
        CMD_GrabFocus = SCI_GRABFOCUS,
        CMD_SetXCaretPolicy = SCI_SETXCARETPOLICY,
        CMD_SetYCaretPolicy = SCI_SETYCARETPOLICY,
        CMD_LineDuplicate = SCI_LINEDUPLICATE,
        CMD_RegisterImage = SCI_REGISTERIMAGE,
        CMD_SetPrintWrapMode = SCI_SETPRINTWRAPMODE,
        CMD_GetPrintWrapMode = SCI_GETPRINTWRAPMODE,
        CMD_ClearRegisteredImages = SCI_CLEARREGISTEREDIMAGES,
        CMD_StyleSetHotspot = SCI_STYLESETHOTSPOT,
        CMD_SetHotspotActiveFore = SCI_SETHOTSPOTACTIVEFORE,
        CMD_SetHotspotActiveBack = SCI_SETHOTSPOTACTIVEBACK,
        CMD_SetHotspotActiveUnderline = SCI_SETHOTSPOTACTIVEUNDERLINE,
        CMD_ParaDown = SCI_PARADOWN,
        CMD_ParaDownExtend = SCI_PARADOWNEXTEND,
        CMD_ParaUp = SCI_PARAUP,
        CMD_ParaUpExtend = SCI_PARAUPEXTEND,
        CMD_PositionBefore = SCI_POSITIONBEFORE,
        CMD_PositionAfter = SCI_POSITIONAFTER,
        CMD_CopyRange = SCI_COPYRANGE,
        CMD_CopyText = SCI_COPYTEXT,
        CMD_SetSelectionMode = SCI_SETSELECTIONMODE,
        CMD_GetSelectionMode = SCI_GETSELECTIONMODE,
        CMD_GetLineSelStartPostion = SCI_GETLINESELSTARTPOSITION,
        CMD_GetLineSelEndPosition = SCI_GETLINESELENDPOSITION,
        CMD_LineDownRectExtend = SCI_LINEDOWNRECTEXTEND,
        CMD_LineUpRectExtend = SCI_LINEUPRECTEXTEND,
        CMD_CharLeftRectExtend = SCI_CHARLEFTRECTEXTEND,
        CMD_CharRightRectExtend = SCI_CHARRIGHTRECTEXTEND,
        CMD_HomeRectExtend = SCI_HOMERECTEXTEND,
        CMD_VCHomeRectExtend = SCI_VCHOMERECTEXTEND,
        CMD_LineEndRectExtend = SCI_LINEENDRECTEXTEND,
        CMD_PageUpRectExtend = SCI_PAGEUPRECTEXTEND,
        CMD_PageDownRectExtend = SCI_PAGEDOWNRECTEXTEND,
        CMD_StutteredPageUp = SCI_STUTTEREDPAGEUP,
        CMD_StutteredPageUpExtend = SCI_STUTTEREDPAGEUPEXTEND,
        CMD_StutteredPageDown = SCI_STUTTEREDPAGEDOWN,
        CMD_StutteredPageDownExtend = SCI_STUTTEREDPAGEDOWNEXTEND,
        CMD_WordLeftEnd = SCI_WORDLEFTEND,
        CMD_WordLeftEndExtend = SCI_WORDLEFTENDEXTEND,
        CMD_WordRightEnd = SCI_WORDRIGHTEND,
        CMD_WordRightEndExtend = SCI_WORDRIGHTENDEXTEND,
        CMD_SetWhitespaceChars = SCI_SETWHITESPACECHARS,
        CMD_SetCharsDefault = SCI_SETCHARSDEFAULT,
        CMD_AutoCGetCurrent = SCI_AUTOCGETCURRENT,
        CMD_Allocate = SCI_ALLOCATE,
        CMD_HomeWrap = SCI_HOMEWRAP,
        CMD_HomeWrapExtend = SCI_HOMEWRAPEXTEND,
        CMD_LineEndWrap = SCI_LINEENDWRAP,
        CMD_LineEndWrapExtend = SCI_LINEENDWRAPEXTEND,
        CMD_VCHomeWrap = SCI_VCHOMEWRAP,
        CMD_VCHomeWrapExtend = SCI_VCHOMEWRAPEXTEND,
        CMD_LineCopy = SCI_LINECOPY,
        CMD_FindColumn = SCI_FINDCOLUMN,
        CMD_GetCaretSticky = SCI_GETCARETSTICKY,
        CMD_SetCaretSticky = SCI_SETCARETSTICKY,
        CMD_ToggleCaretSticky = SCI_TOGGLECARETSTICKY,
        CMD_SetWrapVisualFlags = SCI_SETWRAPVISUALFLAGS,
        CMD_GetWrapVisualFlags = SCI_GETWRAPVISUALFLAGS,
        CMD_SetWrapVisualFlagsLocation = SCI_SETWRAPVISUALFLAGSLOCATION,
        CMD_GetWrapVisualFlagsLocation = SCI_GETWRAPVISUALFLAGSLOCATION,
        CMD_SetWrapStartIndent = SCI_SETWRAPSTARTINDENT,
        CMD_GetWrapStartIndent = SCI_GETWRAPSTARTINDENT,
        CMD_MarkerAddSet = SCI_MARKERADDSET,
        CMD_SetPasteConvertEndings = SCI_SETPASTECONVERTENDINGS,
        CMD_GetPasteConvertEndings = SCI_GETPASTECONVERTENDINGS,
        CMD_SelectionDuplicate = SCI_SELECTIONDUPLICATE,
        CMD_SetCaretLineBackAlpha = SCI_SETCARETLINEBACKALPHA,
        CMD_GetCaretLineBackAlpha = SCI_GETCARETLINEBACKALPHA,
        CMD_SetWrapIndentMode = SCI_SETWRAPINDENTMODE,
        CMD_GetWrapIndentMode = SCI_GETWRAPINDENTMODE,
        CMD_MarkerSetAlpha = SCI_MARKERSETALPHA,
        CMD_GetSelAlpha = SCI_GETSELALPHA,
        CMD_SetSelAlpha = SCI_SETSELALPHA,
        CMD_GetSelEolFilled = SCI_GETSELEOLFILLED,
        CMD_SetSelEolFilled = SCI_SETSELEOLFILLED,
        CMD_StyleGetFore = SCI_STYLEGETFORE,
        CMD_StyleGetBack = SCI_STYLEGETBACK,
        CMD_StyleGetBold = SCI_STYLEGETBOLD,
        CMD_StyleGetItalic = SCI_STYLEGETITALIC,
        CMD_StyleGetSize = SCI_STYLEGETSIZE,
        CMD_StyleGetFont = SCI_STYLEGETFONT,
        CMD_StyleGetEolFilled = SCI_STYLEGETEOLFILLED,
        CMD_StyleGetUnderline = SCI_STYLEGETUNDERLINE,
        CMD_StyleGetCase = SCI_STYLEGETCASE,
        CMD_StyleGetCharacterSet = SCI_STYLEGETCHARACTERSET,
        CMD_StyleGetVisible = SCI_STYLEGETVISIBLE,
        CMD_StyleGetChangeable = SCI_STYLEGETCHANGEABLE,
        CMD_StyleGetHotspot = SCI_STYLEGETHOTSPOT,
        CMD_GetHotspotActiveFore = SCI_GETHOTSPOTACTIVEFORE,
        CMD_GetHotspotActiveBack = SCI_GETHOTSPOTACTIVEBACK,
        CMD_GetHotspotActiveUnderline = SCI_GETHOTSPOTACTIVEUNDERLINE,
        CMD_GetHotspotSingleLine = SCI_GETHOTSPOTSINGLELINE,
        CMD_BraceHighlightIndicator = SCI_BRACEHIGHLIGHTINDICATOR,
        CMD_BraceBadlightIndicator = SCI_BRACEBADLIGHTINDICATOR,
        CMD_SetIndicatorCurrent = SCI_SETINDICATORCURRENT,
        CMD_GetIndicatorCurrent = SCI_GETINDICATORCURRENT,
        CMD_SetIndicatorValue = SCI_SETINDICATORVALUE,
        CMD_GetIndicatorValue = SCI_GETINDICATORVALUE,
        CMD_IndicatorFillRange = SCI_INDICATORFILLRANGE,
        CMD_IndicatorClearRange = SCI_INDICATORCLEARRANGE,
        CMD_IndicatorAllOnFor = SCI_INDICATORALLONFOR,
        CMD_IndicatorValueAt = SCI_INDICATORVALUEAT,
        CMD_IndicatorStart = SCI_INDICATORSTART,
        CMD_IndicatorEnd = SCI_INDICATOREND,
        CMD_IndicSetUnder = SCI_INDICSETUNDER,
        CMD_IndicGetUnder = SCI_INDICGETUNDER,
        CMD_SetCaretStyle = SCI_SETCARETSTYLE,
        CMD_GetCaretStyle = SCI_GETCARETSTYLE,
        CMD_SetPositionCache = SCI_SETPOSITIONCACHE,
        CMD_GetPositionCache = SCI_GETPOSITIONCACHE,
        CMD_SetScrollWidthTracking = SCI_SETSCROLLWIDTHTRACKING,
        CMD_GetScrollWidthTracking = SCI_GETSCROLLWIDTHTRACKING,
        CMD_DelWordRightEnd = SCI_DELWORDRIGHTEND,
        CMD_CopyAllowLine = SCI_COPYALLOWLINE,
        CMD_GetCharacterPointer = SCI_GETCHARACTERPOINTER,
        CMD_SetKeysUnicode = SCI_SETKEYSUNICODE,
        CMD_GetKeysUnicode = SCI_GETKEYSUNICODE,
        CMD_IndicSetAlpha = SCI_INDICSETALPHA,
        CMD_IndicGetAlpha = SCI_INDICGETALPHA,
        CMD_SetExtraAscent = SCI_SETEXTRAASCENT,
        CMD_GetExtraAscent = SCI_GETEXTRAASCENT,
        CMD_SetExtraDescent = SCI_SETEXTRADESCENT,
        CMD_GetExtraDescent = SCI_GETEXTRADESCENT,
        CMD_MarkerSymbolDefined = SCI_MARKERSYMBOLDEFINED,
        CMD_MarginSetText = SCI_MARGINSETTEXT,
        CMD_MarginGetText = SCI_MARGINGETTEXT,
        CMD_MarginSetStyle = SCI_MARGINSETSTYLE,
        CMD_MarginGetStyle = SCI_MARGINGETSTYLE,
        CMD_MarginSetStyles = SCI_MARGINSETSTYLES,
        CMD_MarginGetStyles = SCI_MARGINGETSTYLES,
        CMD_MarginTextClearAll = SCI_MARGINTEXTCLEARALL,
        CMD_MarginSetStyleOffset = SCI_MARGINSETSTYLEOFFSET,
        CMD_MarginGetStyleOffset = SCI_MARGINGETSTYLEOFFSET,
        CMD_SetMarginOptions = SCI_SETMARGINOPTIONS,
        CMD_AnnotationSetText = SCI_ANNOTATIONSETTEXT,
        CMD_AnnotationGetText = SCI_ANNOTATIONGETTEXT,
        CMD_AnnotationSetStyle = SCI_ANNOTATIONSETSTYLE,
        CMD_AnnotationGetStyle = SCI_ANNOTATIONGETSTYLE,
        CMD_AnnotationSetStyles = SCI_ANNOTATIONSETSTYLES,
        CMD_AnnotationGetStyles = SCI_ANNOTATIONGETSTYLES,
        CMD_AnnotationGetLines = SCI_ANNOTATIONGETLINES,
        CMD_AnnotationClearAll = SCI_ANNOTATIONCLEARALL,
        CMD_AnnotationSetVisible = SCI_ANNOTATIONSETVISIBLE,
        CMD_AnnotationGetVisible = SCI_ANNOTATIONGETVISIBLE,
        CMD_AnnotationSetStyleOffset = SCI_ANNOTATIONSETSTYLEOFFSET,
        CMD_AnnotationGetStyleOffset = SCI_ANNOTATIONGETSTYLEOFFSET,
        CMD_SetEmptySelection = SCI_SETEMPTYSELECTION,
        CMD_GetMarginOptions = SCI_GETMARGINOPTIONS,
        CMD_IndicSetOutlineAlpha = SCI_INDICSETOUTLINEALPHA,
        CMD_IndicGetOutlineAlpha = SCI_INDICGETOUTLINEALPHA,
        CMD_AddUndoAction = SCI_ADDUNDOACTION,
        CMD_CharPositionFromPoint = SCI_CHARPOSITIONFROMPOINT,
        CMD_CharPositionFromPointClose = SCI_CHARPOSITIONFROMPOINTCLOSE,
        CMD_SetMultipleSelection = SCI_SETMULTIPLESELECTION,
        CMD_GetMultipleSelection = SCI_GETMULTIPLESELECTION,
        CMD_SetAdditionalSelectionTyping = SCI_SETADDITIONALSELECTIONTYPING,
        CMD_GetAdditionalSelectionTyping = SCI_GETADDITIONALSELECTIONTYPING,
        CMD_SetAdditionalCaretsBlink = SCI_SETADDITIONALCARETSBLINK,
        CMD_GetAdditionalCaretsBlink = SCI_GETADDITIONALCARETSBLINK,
        CMD_SetAdditionalCaretsVisible = SCI_SETADDITIONALCARETSVISIBLE,
        CMD_GetAdditionalCaretsVisible = SCI_GETADDITIONALCARETSVISIBLE,
        CMD_AutoCGetCurrentText = SCI_AUTOCGETCURRENTTEXT,
        CMD_GetSelections = SCI_GETSELECTIONS,
        CMD_ClearSelections = SCI_CLEARSELECTIONS,
        CMD_SetSelection = SCI_SETSELECTION,
        CMD_AddSelection = SCI_ADDSELECTION,
        CMD_SetMainSelection = SCI_SETMAINSELECTION,
        CMD_GetMainSelection = SCI_GETMAINSELECTION,
        CMD_SetSelectionNCaret = SCI_SETSELECTIONNCARET,
        CMD_GetSelectionNCaret = SCI_GETSELECTIONNCARET,
        CMD_SetSelectionNAnchor = SCI_SETSELECTIONNANCHOR,
        CMD_GetSelectionNAnchor = SCI_GETSELECTIONNANCHOR,
        CMD_SetSelectionNCaretVirtualSpace = SCI_SETSELECTIONNCARETVIRTUALSPACE,
        CMD_GetSelectionNCaretVirtualSpace = SCI_GETSELECTIONNCARETVIRTUALSPACE,
        CMD_SetSelectionNAnchorVirtualSpace = SCI_SETSELECTIONNANCHORVIRTUALSPACE,
        CMD_GetSelectionNAnchorVirtualSpace = SCI_GETSELECTIONNANCHORVIRTUALSPACE,
        CMD_SetSelectionNStart = SCI_SETSELECTIONNSTART,
        CMD_GetSelectionNStart = SCI_GETSELECTIONNSTART,
        CMD_SetSelectionNEnd = SCI_SETSELECTIONNEND,
        CMD_GetSelectionNEnd = SCI_GETSELECTIONNEND,
        CMD_SetRectangularSelectionCaret = SCI_SETRECTANGULARSELECTIONCARET,
        CMD_GetRectangularSelectionCaret = SCI_GETRECTANGULARSELECTIONCARET,
        CMD_SetRectangularSelectionAnchor = SCI_SETRECTANGULARSELECTIONANCHOR,
        CMD_GetRectangularSelectionAnchor = SCI_GETRECTANGULARSELECTIONANCHOR,
        CMD_SetRectangularSelectionCaretVirtualSpace = SCI_SETRECTANGULARSELECTIONCARETVIRTUALSPACE,
        CMD_GetRectangularSelectionCaretVirtualSpace = SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE,
        CMD_SetRectangularSelectionAnchorVirtualSpace = SCI_SETRECTANGULARSELECTIONANCHORVIRTUALSPACE,
        CMD_GetRectangularSelectionAnchorVirtualSpace = SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE,
        CMD_SetVirtualSpaceOptions = SCI_SETVIRTUALSPACEOPTIONS,
        CMD_GetvirtualSpaceOptions = SCI_GETVIRTUALSPACEOPTIONS,
        CMD_SetRectangularSelectionModifier = SCI_SETRECTANGULARSELECTIONMODIFIER,
        CMD_GetRectangularSelectionModifier = SCI_GETRECTANGULARSELECTIONMODIFIER,
        CMD_SetAdditionalSelFore = SCI_SETADDITIONALSELFORE,
        CMD_SetAdditionalSelBack = SCI_SETADDITIONALSELBACK,
        CMD_SetAdditionalSelAlpha = SCI_SETADDITIONALSELALPHA,
        CMD_GetAdditionalSelAlpha = SCI_GETADDITIONALSELALPHA,
        CMD_SetAdditionalCaretFore = SCI_SETADDITIONALCARETFORE,
        CMD_GetAdditionalCaretFore = SCI_GETADDITIONALCARETFORE,
        CMD_RotateSelection = SCI_ROTATESELECTION,
        CMD_SwapMainAnchorCaret = SCI_SWAPMAINANCHORCARET,
        CMD_SetFontQuality = SCI_SETFONTQUALITY,
        CMD_GetFontQuality = SCI_GETFONTQUALITY,
        CMD_SetFirstVisibleLine = SCI_SETFIRSTVISIBLELINE,
        CMD_SetMultiPaste = SCI_SETMULTIPASTE,
        CMD_GetMultiPaste = SCI_GETMULTIPASTE,
        CMD_GetTag = SCI_GETTAG,
        CMD_ChangeLexerState = SCI_CHANGELEXERSTATE,
        CMD_ContractedFoldNext = SCI_CONTRACTEDFOLDNEXT,
        CMD_VerticalCentreCaret = SCI_VERTICALCENTRECARET,
        CMD_MoveSelectionLinesUp = SCI_MOVESELECTEDLINESUP,
        CMD_MoveSelectionLinesDown = SCI_MOVESELECTEDLINESDOWN,
        CMD_SetIdentifier = SCI_SETIDENTIFIER,
        CMD_GetIdentifier = SCI_GETIDENTIFIER,
        CMD_RgbImageSetWidth = SCI_RGBAIMAGESETWIDTH,
        CMD_RgbImageSetHeight = SCI_RGBAIMAGESETHEIGHT,
        CMD_MarkerDefineRgbaImage = SCI_MARKERDEFINERGBAIMAGE,
        CMD_RegisterRgbaImage = SCI_REGISTERRGBAIMAGE,
        CMD_ScrollToStart = SCI_SCROLLTOSTART,
        CMD_ScrollToEnd = SCI_SCROLLTOEND,
        CMD_SetTechnology = SCI_SETTECHNOLOGY,
        CMD_GetTechnology = SCI_GETTECHNOLOGY,
        CMD_CreateLoader = SCI_CREATELOADER,
        CMD_CountCharacters = SCI_COUNTCHARACTERS,
        CMD_AutoCSetCaseInsensitiveBehaviour = SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR,
        CMD_AutoCGetCaseInsensitiveBehaviour = SCI_AUTOCGETCASEINSENSITIVEBEHAVIOUR,
        CMD_FindIndicatorShow = SCI_FINDINDICATORSHOW,
        CMD_FindIndicatorFlash = SCI_FINDINDICATORFLASH,
        CMD_FindIndicatorHide = SCI_FINDINDICATORHIDE,
        CMD_GetRangePointer = SCI_GETRANGEPOINTER,
        CMD_GetGapPosition = SCI_GETGAPPOSITION,
        CMD_DeleteRange = SCI_DELETERANGE,
        CMD_GetWordChars = SCI_GETWORDCHARS,
        CMD_GetWhitespaceChars = SCI_GETWHITESPACECHARS,
        CMD_SetPunctuationChars = SCI_SETPUNCTUATIONCHARS,
        CMD_GetPunctuationChars = SCI_GETPUNCTUATIONCHARS,
        CMD_GetSelectionEmpty = 2650,
        CMD_RgbaImageSetScale = 2651,
        CMD_VCHomeDisplay = 2652,
        CMD_VCHomeDisplayExtend = 2653,
        CMD_StartRecord = SCI_STARTRECORD,
        CMD_StopRecord = SCI_STOPRECORD,
        CMD_SetLexer = SCI_SETLEXER,
        CMD_GetLexer = SCI_GETLEXER,
        CMD_Colourise = SCI_COLOURISE,
        CMD_SetProperty = SCI_SETPROPERTY,
        CMD_SetKeywords = SCI_SETKEYWORDS,
        CMD_SetLexerLanguage = SCI_SETLEXERLANGUAGE,
        CMD_LoadLexerLibrary = SCI_LOADLEXERLIBRARY,
        CMD_GetProperty = SCI_GETPROPERTY,
        CMD_GetPropertyExpanded = SCI_GETPROPERTYEXPANDED,
        CMD_GetPropertyInt = SCI_GETPROPERTYINT,
        CMD_GetStyleBitsNeeded = SCI_GETSTYLEBITSNEEDED,
        CMD_GetLexerLanguage = SCI_GETLEXERLANGUAGE,
        CMD_PrivateLexerCall = SCI_PRIVATELEXERCALL,
        CMD_PropertyNames = SCI_PROPERTYNAMES,
        CMD_PropertyType = SCI_PROPERTYTYPE,
        CMD_DescribeProperty = SCI_DESCRIBEPROPERTY,
        CMD_DescribeKeywordSets = SCI_DESCRIBEKEYWORDSETS,

        //! Delete the current character.
        CMD_Delete = SCI_CLEAR,

        //! Delete the word to the left.
        CMD_DeleteWordLeft = SCI_DELWORDLEFT,

        //! Delete the word to the right.
        CMD_DeleteWordRight = SCI_DELWORDRIGHT,

        //! Delete right to the end of the next word.
        CMD_DeleteWordRightEnd = SCI_DELWORDRIGHTEND,

        //! Delete the line to the left.
        CMD_DeleteLineLeft = SCI_DELLINELEFT,

        //! Delete the line to the right.
        CMD_DeleteLineRight = SCI_DELLINERIGHT,

        //! Move the selected lines up one line.
        CMD_MoveSelectedLinesUp = SCI_MOVESELECTEDLINESUP,

        //! Move the selected lines down one line.
        CMD_MoveSelectedLinesDown = SCI_MOVESELECTEDLINESDOWN,

        //! Convert the selection to lower case.
        CMD_SelectionLowerCase = SCI_LOWERCASE,

        //! Convert the selection to upper case.
        CMD_SelectionUpperCase = SCI_UPPERCASE,

        //! Cut the selection to the clipboard.
        CMD_SelectionCut = SCI_CUT,

        //! Copy the selection to the clipboard.
        CMD_SelectionCopy = SCI_COPY,

        CMD_Newline = SCI_NEWLINE,

        //! Insert a formfeed.
        CMD_Formfeed = SCI_FORMFEED,

        //! De-indent one level.
        CMD_Backtab = SCI_BACKTAB
    };

    //! 此枚举类型定义了文字的大小写风格
    enum TextCase {
        //! 原始风格
        TC_Original,
        //! 大写
        TC_Upper,
        //! 小写
        TC_Lower
    };

    //! 此枚举类型定义了调用提示的位置
    enum CallTipsPosition {
        //! 在文字之下
        CTP_BelowText,
        //! 在文字之上
        CTP_AboveText
    };

    //! 此枚举类型定义了调用提示的风格
    enum CallTipsStyle {
        //! 不显示
        CTS_None,
        //! 无上下文
        CTS_NoContext,
        //! 无自动完成上下文
        CTS_NoAutoCompletionContext,
        //! 上下文
        CTS_Context
    };

}
#endif // SIMSCINTILLA_DEFS_H
