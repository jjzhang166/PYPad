#ifndef SIMDOCUMENT_H
#define SIMDOCUMENT_H

#include "simscintilla_global.h"

#include <ScintillaDocument.h>

class SimScintilla;
class SimDocumentPrivate;
class SIMSCINTILLA_EXPORT SimDocument : public ScintillaDocument
{
    Q_DECLARE_PRIVATE(SimDocument)
    Q_OBJECT
public:
    explicit SimDocument(QObject* parent = nullptr, void* pdoc = nullptr);
    virtual ~SimDocument();

    SimDocument(const SimDocument& other);
    SimDocument& operator=(const SimDocument& other);

private:
    friend class SimScintilla;

    void attach(const SimDocument& other);
    void detach();

    void display(SimScintilla* scintilla, const SimDocument* other);
    void undisplay(SimScintilla* scintilla);

    bool isModified() const;
    void setModified(bool modify);

    SimDocumentPrivate* d;
};

#endif // SIMDOCUMENT_H
