#ifndef SIMLEXERAPDL_H
#define SIMLEXERAPDL_H

#include "../simabstractlexer.h"
#include "SciLexer.h"

class SimDbManager;
class SimScintilla;
class SimLexerFactory;

class SimLexerApdl : public SimAbstractLexer
{
    Q_OBJECT

public:

    //! 如下枚举类型定义了为APDL语法分析器所用的不同风格
    enum Tag {

        //! 默认
        Default = SCE_APDL_DEFAULT,
        //! 注释
        Comment = SCE_APDL_COMMENT,
        //! 注释块
        CommentBlock = SCE_APDL_COMMENTBLOCK,
        //! 数字
        Number = SCE_APDL_NUMBER,
        //! 字符串
        String = SCE_APDL_STRING,
        //! 操作符
        Operator = SCE_APDL_OPERATOR,
        //! 字
        Word = SCE_APDL_WORD,
        //! 处理器
        Processor = SCE_APDL_PROCESSOR,
        //! 命令
        Command = SCE_APDL_COMMAND,
        //! 斜杠开头的命令
        SlashCommand = SCE_APDL_SLASHCOMMAND,
        //! 星号开头的命令
        StarCommand = SCE_APDL_STARCOMMAND,
        //! 参数
        Argument = SCE_APDL_ARGUMENT,
        //! 函数
        Function = SCE_APDL_FUNCTION
    };

    int lang() const { return SCLEX_APDL; }
    QString lexerStr() const { return "apdl"; }
    QString description(int style) const;
    QString keywords(int set) const;
    QColor defaultColor(int style) const;
    bool defaultEolFill(int style) const;
    QFont defaultFont(int style) const;
    QColor defaultBackgroundColor(int style) const;

protected:
    friend class SimLexerFactory;
    SimLexerApdl(SimScintilla* parent = 0);

private:
    SimDbManager* m_dbMgr;
};

#endif // SIMLEXERAPDL_H
