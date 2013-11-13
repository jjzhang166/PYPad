#ifndef SIMABSTRACTTEXTSTYLE_H
#define SIMABSTRACTTEXTSTYLE_H

#include "simscintilladefs.h"
#include "simscintilla_global.h"

#include <QObject>

class SimScintilla;
class SimAbstractTextStylePrivate;
class SIMSCINTILLA_EXPORT SimAbstractTextStyle
{
public:

    SimAbstractTextStyle(int style = -1);
    SimAbstractTextStyle(int style, const QString &description, const QColor &color,
                         const QColor &bgColor, const QFont &font, bool eolFill = false);
    virtual ~SimAbstractTextStyle();

    void apply(const SimScintilla *sci) const;
    int style() const;
    void initFrom(const SimAbstractTextStyle& other);

    void setDescription(const QString &description);
    QString description() const;

    void setColor(const QColor &color);
    QColor color() const;

    void setBackgroundColor(const QColor &bgColor);
    QColor backgroundColor() const;

    void setFont(const QFont &font);
    QFont font() const;

    void setEolFill(bool fill);
    bool isEolFill() const;

    void setTextCase(SimSci::TextCase textCase);
    SimSci::TextCase textCase() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setChangeable(bool changeable);
    bool isChangeable() const;

    void setHotspot(bool hotspot);
    bool isHotspot() const;

private:
    SimAbstractTextStylePrivate* d;
    Q_DISABLE_COPY(SimAbstractTextStyle)
};

#endif // SIMABSTRACTTEXTSTYLE_H
