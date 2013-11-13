#ifndef SIMMACRO_H
#define SIMMACRO_H

#include <QObject>
#include <Scintilla.h>

class SimMacroPrivate;
class SimScintilla;
class SimMacro : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SimMacro)
public:
    explicit SimMacro(SimScintilla* parent);
    ~SimMacro();

    void startRecording();
    void stopRecording();
    void play();

    bool load(const QString& openfile);
    bool save(const QString& savedfile);

private Q_SLOTS:
    void onMacroRecord(int message, uptr_t wParam, sptr_t lParam);
private:
    Q_DISABLE_COPY(SimMacro)
    SimMacroPrivate* d;
};

#endif // SIMMACRO_H
