#include "simdocument.h"
#include "simscintilladefs.h"
#include "simscintilla.h"

class SimDocumentPrivate {
public:
    explicit SimDocumentPrivate(): doc(nullptr), displayCount(0),attachCount(0),
                                   modified(false), scintilla(nullptr) {}

    void*  doc;
    int displayCount;
    int attachCount;
    bool modified;
    SimScintilla* scintilla;
};

SimDocument::SimDocument(QObject *parent, void *pdoc): ScintillaDocument(parent, pdoc)
{
    d = new SimDocumentPrivate();
}

SimDocument::~SimDocument()
{
    SAFE_RELEASE(d)
}

SimDocument::SimDocument(const SimDocument &other)
{
    attach(other);
}

SimDocument& SimDocument::operator =(const SimDocument& other)
{
    if (d != other.d) {
        detach();
        attach(other);
    }
    return *this;
}

void SimDocument::attach(const SimDocument& other)
{
    ++other.d->attachCount;
    d = other.d;
}

void SimDocument::detach()
{
    if (d == nullptr) {
        return;
    }

    if (--d->attachCount == 0) {
        if (d->doc && d->displayCount == 0) {
            d->scintilla->releaseDocument(reinterpret_cast<sptr_t>(d->doc));
        }
        delete d;
    }

    d = nullptr;
}

void SimDocument::display(SimScintilla* scintilla, const SimDocument* other)
{
    void *ndoc = (other ? other->d->doc : nullptr);

    SimSci::EolMode eolMode= scintilla->eolMode();
    scintilla->setDocPointer(reinterpret_cast<sptr_t>(ndoc));
    ndoc = reinterpret_cast<void *>(scintilla->docPointer());
    scintilla->setEolMode(eolMode);

    d->doc = ndoc;
    ++d->displayCount;
    d->scintilla = scintilla;
}

void SimDocument::undisplay(SimScintilla* scintilla)
{
    if (--d->attachCount == 0)
        delete d;
    else if (--d->displayCount == 0) {
        if (d->doc != nullptr) {
            scintilla->addRefDocument(reinterpret_cast<sptr_t>(d->doc));
        }
    }

    d = nullptr;
}

void SimDocument::setModified(bool modify)
{
    d->modified = modify;
    if (modify) {
        ScintillaDocument::set_save_point();
    }
}

bool SimDocument::isModified() const
{
    return d->modified;
}
