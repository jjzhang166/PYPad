#include "pypadconfig.h"

PYPadConfig::PYPadConfig(QWidget *parent) :
    QWidget(parent)
{
    initWidgets();
    initSettings();
    initMessages();
}

/*!
  private utility functions
*/
void PYPadConfig::initWidgets()
{
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry();
    int MaxWidth = desktopRect.width();
    int MaxHeight = desktopRect.height();

    //
    MainLayout = new QVBoxLayout;

    //
    ConfigLayout = new QGridLayout();
    WidthLabel = new QLabel(tr("Pad Width:"),this);
    HeightLabel = new QLabel(tr("Pad Height"),this);
    WidthSlider =new QSlider(Qt::Horizontal,this);
    WidthSlider->setRange(PYPC_MIN,MaxWidth);
    HeightSlider = new QSlider(Qt::Horizontal,this);
    HeightSlider->setRange(PYPC_MIN,MaxHeight);
    ConfigLayout->addWidget(WidthLabel,0,0);
    ConfigLayout->addWidget(WidthSlider,0,1);
    ConfigLayout->addWidget(HeightLabel,1,0);
    ConfigLayout->addWidget(HeightSlider,1,1);

    //
    StickCheck  =new QCheckBox(tr("Stick to father"),this);

    //
    BtnLayout = new QHBoxLayout;
    OKBtn = new QPushButton(tr("OK"),this);
    BtnLayout->addStretch();
    BtnLayout->addWidget(OKBtn);

    //setup mainLayout
    MainLayout->addLayout(ConfigLayout);
    MainLayout->addWidget(StickCheck);
    MainLayout->addLayout(BtnLayout);
    setLayout(MainLayout);

}

void PYPadConfig::initSettings()
{
    setWindowTitle(tr("Config PYPad"));
    setWindowFlags(Qt::Window);
    StickCheck->setCheckState(Qt::Checked);

    WidthSlider->setValue(400);
    HeightSlider->setValue(500);

    setWindowModality(Qt::ApplicationModal);
}

void PYPadConfig::initMessages()
{
    connect(WidthSlider,SIGNAL(valueChanged(int)),this,SLOT(SliderHandler(int)));
    connect(HeightSlider,SIGNAL(valueChanged(int)),this,SLOT(SliderHandler(int)));

    connect(StickCheck,SIGNAL(stateChanged(int)),this,SLOT(CheckHandler(int)));

    connect(OKBtn,SIGNAL(clicked()),this,SLOT(BtnHandler()));
}

/*!
  private slot functions
*/
void PYPadConfig::CheckHandler(int state)
{
    if(state == Qt::Checked)
    {
        emit sig_SetStickToFather(true);
    }else{
        emit sig_SetStickToFather(false);
    }
}

void PYPadConfig::SliderHandler(int value)
{
    int Width = WidthSlider->value();
    int Height = HeightSlider->value();
    emit sig_SetPadSize(QSize(Width,Height));
}

void PYPadConfig::BtnHandler()
{
    close();
}
