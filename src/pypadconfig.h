#ifndef PYPADCONFIG_H
#define PYPADCONFIG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>


#define PYPC_MIN 100

class PYPadConfig : public QWidget
{
    Q_OBJECT
public:
    explicit PYPadConfig(QWidget *parent = 0);

private:
    void initWidgets();
    void initSettings();
    void initMessages();


private:
    QVBoxLayout* MainLayout;
    QGridLayout* ConfigLayout;
    QLabel* WidthLabel;
    QLabel* HeightLabel;
    QSlider* WidthSlider;
    QSlider* HeightSlider;
    QCheckBox* StickCheck;
    QHBoxLayout* BtnLayout;
    QPushButton* OKBtn;


private Q_SLOTS:
    void CheckHandler(int state);
    void SliderHandler(int value);
    void BtnHandler();

Q_SIGNALS:
    void sig_SetStickToFather(bool stick);
    void sig_SetPadSize(const QSize& size);
};

#endif // PYPADCONFIG_H
