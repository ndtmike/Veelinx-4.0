/****************************************************************************
**
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: mainwindow.cpp
**
** Rebarlinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2018
** Email: mike@ndtjames.com
**
** 6/16/18 Initial Creation
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "comm_def.h"
#include "console.h"
#include "instdata.h"
#include "mainwindow.h"
#include "setting_dialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QtDebug>

/******************************************************************************

  Function: MainWindow()

  Description:
  ============
  Creator for rebarlinx project

  This project is implemented without Designer in order to integrate the
  QwtPlot library, which conflict with the msvc.

******************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    SerialConsole = new Console;
    SerialConsole->setEnabled(false);
    setCentralWidget(SerialConsole);

    Serial = new QSerialPort(this);
    Plot = new QwtPlot;
    SerialTimeOut = new QTimer(this);
    CurrentData = new InstData();

    CreateActions();
    CreateMenus();
    CreateStatusBar();
    SerialCreateActions();

    NewUpload = false;
    Metric =  false;
    MessageReply = false;

//single shot timer connects after everyting is loaded.
    QTimer* init_timer = new QTimer(this); //warning about being used?
    init_timer->singleShot(1, this, SLOT( SerialCheckPort()));
}

MainWindow::~MainWindow()
{
    SerialPortClose();
}
/******************************************************************************

  Function: createActions

  Description:
  ============
  Connects Menu's to Actions

******************************************************************************/
void MainWindow::CreateActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::MenuActOpen);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::MenuActSave);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::MenuActCopy);

    PlotAct = new QAction(tr("&Plot"), this);
    PlotAct->setStatusTip(tr("Plots the Data Uploaded"));
    connect(PlotAct, &QAction::triggered, this, &MainWindow::MenuActPlot);

    TestNumberAct = new QAction(tr("&Test Number"), this);
    TestNumberAct->setStatusTip(tr("Select the Test Number to View"));
    connect( TestNumberAct, &QAction::triggered, this, &MainWindow::MenuActTestNumber );

    RunNoAct = new QAction(tr("No"), this);
    RunNoAct->setStatusTip(tr("Stop V-Meter"));
    RunNoAct->setCheckable ( true );
    RunNoAct->setChecked( true );
    connect( RunNoAct, &QAction::triggered, this, &MainWindow::MenuActRunNo);

    RunYesAct = new QAction(tr("Yes"), this);
    RunYesAct->setStatusTip(tr("Start V-Meter"));
    RunYesAct->setCheckable ( true );
    RunYesAct->setChecked( false );
    connect( RunYesAct, &QAction::triggered, this, &MainWindow::MenuActRunYes);

    RunGrp = new QActionGroup( this );
    RunGrp->addAction( RunNoAct );
    RunGrp->addAction( RunYesAct );

    SaveDataNoAct = new QAction(tr("No"), this);
    SaveDataNoAct->setStatusTip(tr("Do Not Save Data"));
    SaveDataNoAct->setCheckable ( true );
    SaveDataNoAct->setChecked( true );
    connect( SaveDataNoAct, &QAction::triggered, this, &MainWindow::MenuActSaveDataNo);

    SaveDataYesAct = new QAction(tr("Yes"), this);
    SaveDataYesAct->setStatusTip(tr("Save Data"));
    SaveDataYesAct->setCheckable ( true );
    SaveDataYesAct->setChecked( false );
    connect( SaveDataYesAct, &QAction::triggered, this, &MainWindow::MenuActSaveDataYes);

    SaveDataGrp = new QActionGroup( this );
    SaveDataGrp->addAction( SaveDataNoAct );
    SaveDataGrp->addAction( SaveDataYesAct );

    SavePicNoAct = new QAction(tr("No"), this);
    SavePicNoAct->setStatusTip(tr("Do Not Save Picture"));
    SavePicNoAct->setCheckable ( true );
    SavePicNoAct->setChecked( true );
    connect( SavePicNoAct, &QAction::triggered, this, &MainWindow::MenuActPicNo);

    SavePicYesAct = new QAction(tr("Yes"), this);
    SavePicYesAct->setStatusTip(tr("Save Picture"));
    SavePicYesAct->setCheckable ( true );
    SavePicYesAct->setChecked( false );
    connect( SavePicYesAct, &QAction::triggered, this, &MainWindow::MenuActPicYes);

    SavePicGrp = new QActionGroup( this );
    SavePicGrp->addAction( SavePicNoAct );
    SavePicGrp->addAction( SavePicYesAct );

    AmpGain1Act = new QAction(tr("Amplifier Gain 1"), this);
    AmpGain1Act->setStatusTip(tr("Set Amplifier Gain to 1"));
    AmpGain1Act->setCheckable ( true );
    AmpGain1Act->setChecked( false );
    connect( AmpGain1Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain1);

    AmpGain5Act = new QAction(tr("Amplifier Gain 5"), this);
    AmpGain5Act->setStatusTip(tr("Set Amplifier Gain to 5"));
    AmpGain5Act->setCheckable ( true );
    AmpGain5Act->setChecked( false );
    connect( AmpGain5Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain5 );

    AmpGain10Act = new QAction(tr("Amplifier Gain 10"), this);
    AmpGain10Act->setStatusTip(tr("Set Amplifier Gain to 10"));
    AmpGain10Act->setCheckable ( true );
    AmpGain10Act->setChecked( true );
    connect( AmpGain10Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain10 );

    AmpGain25Act = new QAction(tr("Amplifier Gain 25"), this);
    AmpGain25Act->setStatusTip(tr("Set Amplifier Gain to 25"));
    AmpGain25Act->setCheckable ( true );
    AmpGain25Act->setChecked( false );
    connect( AmpGain25Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain25 );

    AmpGain50Act = new QAction(tr("Amplifier Gain 50"), this);
    AmpGain50Act->setStatusTip(tr("Set Amplifier Gain to 50"));
    AmpGain50Act->setCheckable ( true );
    AmpGain50Act->setChecked( false );
    connect( AmpGain50Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain50 );

    AmpGain100Act = new QAction(tr("Amplifier Gain 100"), this);
    AmpGain100Act->setStatusTip(tr("Set Amplifier Gain to 100"));
    AmpGain100Act->setCheckable ( true );
    AmpGain100Act->setChecked( false );
    connect( AmpGain100Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain100 );

    AmpGain250Act = new QAction(tr("Amplifier Gain 250"), this);
    AmpGain250Act->setStatusTip(tr("Set Amplifier Gain to 250"));
    AmpGain250Act->setCheckable ( true );
    AmpGain250Act->setChecked( false );
    connect( AmpGain250Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain250 );

    AmpGain500Act = new QAction(tr("Amplifier Gain 500"), this);
    AmpGain500Act->setStatusTip(tr("Set Amplifier Gain to 500"));
    AmpGain500Act->setCheckable ( true );
    AmpGain500Act->setChecked( false );
    connect( AmpGain500Act, &QAction::triggered, this, &MainWindow::MenuActAmpGain500 );

    AmpGainActGrp = new QActionGroup( this );
    AmpGainActGrp->addAction( AmpGain1Act );
    AmpGainActGrp->addAction( AmpGain5Act );
    AmpGainActGrp->addAction( AmpGain10Act );
    AmpGainActGrp->addAction( AmpGain25Act );
    AmpGainActGrp->addAction( AmpGain50Act );
    AmpGainActGrp->addAction( AmpGain100Act );
    AmpGainActGrp->addAction( AmpGain250Act );
    AmpGainActGrp->addAction( AmpGain500Act );

    CaptureRate250Act = new QAction(tr("250kHz Capture Rate"), this);
    CaptureRate250Act->setStatusTip(tr("Set Capture Rate to 250kHz"));
    CaptureRate250Act->setCheckable ( true );
    CaptureRate250Act->setChecked( false );
    connect( CaptureRate250Act, &QAction::triggered, this, &MainWindow::MenuCaptureRate250 );

    CaptureRate500Act = new QAction(tr("500kHz Capture Rate"), this);
    CaptureRate500Act->setStatusTip(tr("Set Capture Rate to 500kHz"));
    CaptureRate500Act->setCheckable ( true );
    CaptureRate500Act->setChecked( true );
    connect( CaptureRate500Act, &QAction::triggered, this, &MainWindow::MenuCaptureRate500 );

    CaptureRate1000Act = new QAction(tr("1 MHz Capture Rate"), this);
    CaptureRate1000Act->setStatusTip(tr("Set Capture Rate to 1 MHz"));
    CaptureRate1000Act->setCheckable ( true );
    CaptureRate1000Act->setChecked( false );
    connect( CaptureRate1000Act, &QAction::triggered, this, &MainWindow::MenuCaptureRate1000 );

    CaptureRate2000Act = new QAction(tr("2 MHz Capture Rate"), this);
    CaptureRate2000Act->setStatusTip(tr("Set Capture Rate to 2 MHz"));
    CaptureRate2000Act->setCheckable ( true );
    CaptureRate2000Act->setChecked( false );
    connect( CaptureRate2000Act, &QAction::triggered, this, &MainWindow::MenuCaptureRate2000 );

    CaptureRateActGrp = new QActionGroup( this );
    CaptureRateActGrp->addAction( CaptureRate250Act );
    CaptureRateActGrp->addAction( CaptureRate500Act );
    CaptureRateActGrp->addAction( CaptureRate1000Act );
    CaptureRateActGrp->addAction( CaptureRate2000Act );

    PulseRate1Act = new QAction(tr("Pulse Rate 1"), this);
    PulseRate1Act->setStatusTip(tr("Set Pulse Rate to 1"));
    PulseRate1Act->setCheckable ( true );
    PulseRate1Act->setChecked( false );
    connect( PulseRate1Act, &QAction::triggered, this, &MainWindow::MenuActPulse1 );

    PulseRate3Act = new QAction(tr("Pulse Rate 3"), this);
    PulseRate3Act->setStatusTip(tr("Set Pulse Rate to 3"));
    PulseRate3Act->setCheckable ( true );
    PulseRate3Act->setChecked( true );
    connect( PulseRate3Act, &QAction::triggered, this, &MainWindow::MenuActPulse3 );

    PulseRate10Act = new QAction(tr("Pulse Rate 10"), this);
    PulseRate10Act->setStatusTip(tr("Set Pulse Rate to 10"));
    PulseRate10Act->setCheckable ( true );
    PulseRate10Act->setChecked( false );
    connect( PulseRate10Act, &QAction::triggered, this, &MainWindow::MenuActPulse10 );

    PulseRateActGrp = new QActionGroup( this );
    PulseRateActGrp->addAction( PulseRate1Act );
    PulseRateActGrp->addAction( PulseRate3Act );
    PulseRateActGrp->addAction( PulseRate10Act );

    HiVoltAct = new QAction(tr( "Hi Voltage Pulse" ), this);
    HiVoltAct->setStatusTip(tr( "Change to Hi Voltage Pulse" ));
    HiVoltAct->setCheckable ( true );
    HiVoltAct->setChecked( false );
    connect( HiVoltAct, &QAction::triggered, this, &MainWindow::MenuActHiVolt );

    LoVoltAct = new QAction(tr("Low Voltage Pulse"), this);
    LoVoltAct->setStatusTip(tr("Change to Low Voltage Pulse"));
    LoVoltAct->setCheckable ( true );
    LoVoltAct->setChecked( false );
    connect( LoVoltAct, &QAction::triggered, this, &MainWindow::MenuActLoVolt );

    VoltageActGrp = new QActionGroup( this );
    VoltageActGrp->addAction( HiVoltAct );
    VoltageActGrp->addAction( LoVoltAct );

    MeasureDistAct = new QAction(tr( "Distance" ), this);
    MeasureDistAct->setStatusTip(tr( "Change to Measure Distance Between Transducers" ));
    MeasureDistAct->setCheckable ( true );
    MeasureDistAct->setChecked( true );
    connect( MeasureDistAct, &QAction::triggered, this, &MainWindow::MenuActDistance );

    MeasureVelAct = new QAction(tr( "Velocity" ), this);
    MeasureVelAct->setStatusTip(tr( "Change to Measure Velocity Between Transducers" ));
    MeasureVelAct->setCheckable ( true );
    MeasureVelAct->setChecked( false );
    connect( MeasureVelAct, &QAction::triggered, this, &MainWindow::MenuActVelocity );

    MeasureActGrp = new QActionGroup( this );
    MeasureActGrp->addAction( MeasureDistAct );
    MeasureActGrp->addAction( MeasureVelAct );

    CycleTimeAct = new QAction(tr( "Cycle Time" ), this);
    MeasureVelAct->setStatusTip(tr( "Change to Cycle Time Between Tests" ));
    connect( CycleTimeAct, &QAction::triggered, this, &MainWindow::MenuActCycleTime );

    DensityAct = new QAction(tr( "Density" ), this);
    MeasureVelAct->setStatusTip(tr( "Change Material Density" ));
    connect( DensityAct, &QAction::triggered, this, &MainWindow::MenuActDensity );

/*
    USCAct = new QAction(tr( "US Customary" ), this);
    USCAct->setStatusTip(tr( "Change to US Customary Units" ));
    USCAct->setCheckable ( true );
    USCAct->setChecked( true );
    connect( USCAct, &QAction::triggered, this, &MainWindow::MenuActUSC );

    MetricAct = new QAction(tr("Metric"), this);
    MetricAct->setStatusTip(tr("Metric Units"));
    MetricAct->setCheckable ( true );
    MetricAct->setChecked( false );
    connect( MetricAct, &QAction::triggered, this, &MainWindow::MenuActMetric);
*/

/*
    UnitsActGrp = new QActionGroup( this );
    UnitsActGrp->addAction( USCAct );
    UnitsActGrp->addAction( MetricAct );
*/
    aboutAct = new QAction( tr("&About"), this);
    aboutAct->setStatusTip( tr("Show the application's About box") );
    connect( aboutAct, &QAction::triggered, this, &MainWindow:: MenuActAbout );

    aboutQtAct = new QAction( tr("About &Qt"), this);
    aboutQtAct->setStatusTip( tr("Show the Qt library's About box") );
    connect( aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt );

    connect(Serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::SerialPortHandleError);
    connect(Serial, &QSerialPort::readyRead, this, &MainWindow::SerialPortReadData);
    connect(SerialConsole, &Console::getData, this, &MainWindow::SerialPortWriteData);
}

/******************************************************************************

  Function: createMenus

  Description:
  ============
  Creates the Menu items

******************************************************************************/
void MainWindow::CreateMenus()
{
    FileMenu = menuBar()->addMenu(tr("&File"));
    FileMenu->addAction( openAct );
    FileMenu->addAction( saveAct );
    FileMenu->addSeparator();
    FileMenu->addAction( exitAct );

    EditMenu = menuBar()->addMenu( tr( "&Edit" ));
    EditMenu->addAction( copyAct );

    ToolMenu = menuBar()->addMenu( tr( "&Tool" ));

    ToolMenu->addSeparator();
    AmpGainMenu = ToolMenu->addMenu(tr("&Amplifier Gain"));
    AmpGainMenu->addAction( AmpGain1Act );
    AmpGainMenu->addAction( AmpGain5Act );
    AmpGainMenu->addAction( AmpGain10Act );
    AmpGainMenu->addAction( AmpGain25Act );
    AmpGainMenu->addAction( AmpGain50Act );
    AmpGainMenu->addAction( AmpGain100Act );
    AmpGainMenu->addAction( AmpGain250Act );
    AmpGainMenu->addAction( AmpGain500Act );

    CaptureRateMenu = ToolMenu->addMenu(tr("&Capture Rate"));
    CaptureRateMenu->addAction( CaptureRate250Act );
    CaptureRateMenu->addAction( CaptureRate500Act );
    CaptureRateMenu->addAction( CaptureRate1000Act );
    CaptureRateMenu->addAction( CaptureRate2000Act );

    VoltMenu = ToolMenu->addMenu( tr( "&Voltage" ));
    VoltMenu->addAction( HiVoltAct );
    VoltMenu->addAction( LoVoltAct );

    ToolMenu->addAction( DensityAct );

    ToolMenu->addSeparator();

    PulseMenu = ToolMenu->addMenu((tr("&Pulse Rate")));
    PulseMenu->addAction( PulseRate1Act );
    PulseMenu->addAction( PulseRate3Act );
    PulseMenu->addAction( PulseRate10Act );

    ToolMenu->addAction( CycleTimeAct );

    RunMenu = ToolMenu->addMenu( tr( "&Run" ));
    RunMenu->addAction( RunYesAct );
    RunMenu->addAction( RunNoAct );

    SaveDataMenu = ToolMenu->addMenu( tr( "Sa&ve Data" ));
    SaveDataMenu->addAction( SaveDataYesAct );
    SaveDataMenu->addAction( SaveDataNoAct );

    SavePicMenu = ToolMenu->addMenu(tr("Save &Pic"));
    SavePicMenu->addAction( SavePicYesAct );
    SavePicMenu->addAction( SavePicNoAct );

    MeasureMenu = ToolMenu->addMenu( tr("&Measure"));
    MeasureMenu->addAction( MeasureDistAct );
    MeasureMenu->addAction( MeasureVelAct );

    ViewMenu = menuBar()->addMenu( tr( "&View" ));
    ViewMenu->addAction( PlotAct );
    ViewMenu->addSeparator();

    helpMenu = menuBar()->addMenu( tr( "&Help" ));
    helpMenu->addAction( aboutAct );
    helpMenu->addAction( aboutQtAct );
}

/******************************************************************************

  Function: CreateStatuBar

  Description:
  ============
  Creates the Menu items

******************************************************************************/
void MainWindow::CreateStatusBar()
{
    Status = new QLabel(" Not Connected ");
    Status->setAlignment(Qt::AlignHCenter);
    Status->setMinimumSize(Status->sizeHint());
    statusBar()->addWidget(Status);
}

/******************************************************************************

  Function: CreateTestmenu

  Description:
  ============
  Dynamically Creates Menu Items for each test
******************************************************************************/
void MainWindow::CreateTestMenus()
{
    bool ok = false;
    if( CurrentData->NumTests() < 2 ){
        QVector< InstData::Test >::iterator begin = CurrentData->GetBeginItr();
        unsigned displaytest = begin->TestNumber;
        SerialConsole->putData( CurrentData->GetTest( displaytest, &ok));//get current test number?
    }else{
        QVector< InstData::Test >::iterator end = ( CurrentData->GetEndItr()) - 1;
        unsigned displaytest = end->TestNumber;
        SerialConsole->putData( CurrentData->GetTest( displaytest, &ok));

        ViewMenu->addAction( TestNumberAct );
    }

}

/******************************************************************************

  Function: MenuActAbout

  Description:
  ============
  Creates the Menu items

******************************************************************************/
void MainWindow::MenuActAbout()
{
    const QString about_rebarlinx = tr("The <b>Veelinx Sofware</b> is for use with <br>"
                                       "the James Instruments Inc.<br>"
                                       "<a href=\"https://www.ndtjames.com/V-Meter-MK-IV-p/v-c-4.htm\">V-Meter</a><br>"
                                       "USA: +1773.4636565<br>"
                                       "Europe: +31.548.659032<br>"
                                       "Email: <a href=\"mailto:info@ndtjames.com?Subject=Rebarlinx\" target=\"_top\">info@ndtjames.com</a><br>"
                                       "Copyright 2017<br>");
    const QString version =  QString::QString("<br> Software Version <<br> <b>%1.%2.%3</b>").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH);
    QString show = about_rebarlinx + version;
    QMessageBox::about(this, tr("About Rebarlinx Software"), show);
    ShowStatusMessage( "About Rebarlinx" );
}

/******************************************************************************

  Function: MenuActAmpGain1

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain1()
{
    QByteArray ba1;

    ba1.resize( REMOTE_CTRL_MSG_SIZE );
    ba1[0] = REMOTE_CTRL_HEADER; ba1[1] =  MSG_CODE_AMP_GAIN;
    ba1[2] = AMPLIFIER_GAIN_1_SETTING;
    ba1[3] = MSG_CODE_FILL;
    ba1[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba1 );

    ShowStatusMessage( "Change to Amplifier Gain 1" );
}

/******************************************************************************

  Function: MenuActAmpGain5

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain5()
{
    QByteArray ba5;

    ba5.resize( REMOTE_CTRL_MSG_SIZE );
    ba5[0] = REMOTE_CTRL_HEADER; ba5[1] =  MSG_CODE_AMP_GAIN;
    ba5[2] = AMPLIFIER_GAIN_5_SETTING;
    ba5[3] = MSG_CODE_FILL; ba5[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba5 );

    ShowStatusMessage( "Change to Amplifier Gain 5" );
}

/******************************************************************************

  Function: MenuActAmpGain10

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain10()
{
    QByteArray ba10;

    ba10.resize( REMOTE_CTRL_MSG_SIZE );
    ba10[0] = REMOTE_CTRL_HEADER; ba10[1] =  MSG_CODE_AMP_GAIN;
    ba10[2] = AMPLIFIER_GAIN_10_SETTING;
    ba10[3] = MSG_CODE_FILL; ba10[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba10 );

    ShowStatusMessage( "Change to Amplifier Gain 10" );
}


/******************************************************************************

  Function: MenuActAmpGain25

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain25()
{
    QByteArray ba25;

    ba25.resize( REMOTE_CTRL_MSG_SIZE );
    ba25[0] = REMOTE_CTRL_HEADER; ba25[1] =  MSG_CODE_AMP_GAIN;
    ba25[2] = AMPLIFIER_GAIN_25_SETTING;
    ba25[3] = MSG_CODE_FILL;
    ba25[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba25 );

    ShowStatusMessage( "Change to Amplifier Gain 25" );
}

/******************************************************************************

  Function: MenuActAmpGain50

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain50()
{
    QByteArray ba50;

    ba50.resize( REMOTE_CTRL_MSG_SIZE );
    ba50[0] = REMOTE_CTRL_HEADER; ba50[1] =  MSG_CODE_AMP_GAIN;
    ba50[2] = AMPLIFIER_GAIN_50_SETTING;
    ba50[3] = MSG_CODE_FILL; ba50[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba50 );

    ShowStatusMessage( "Change to Amplifier Gain 50" );
}

/******************************************************************************

  Function: MenuActAmpGain100

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain100()
{
    QByteArray ba100;

    ba100.resize( REMOTE_CTRL_MSG_SIZE );
    ba100[0] = REMOTE_CTRL_HEADER; ba100[1] =  MSG_CODE_AMP_GAIN;
    ba100[2] = AMPLIFIER_GAIN_100_SETTING;
    ba100[3] = MSG_CODE_FILL; ba100[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba100 );

    ShowStatusMessage( "Change to Amplifier Gain 100" );
}

/******************************************************************************

  Function: MenuActAmpGain250

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain250()
{
    QByteArray ba250;

    ba250.resize( REMOTE_CTRL_MSG_SIZE );
    ba250[0] = REMOTE_CTRL_HEADER; ba250[1] =  MSG_CODE_AMP_GAIN;
    ba250[2] = AMPLIFIER_GAIN_250_SETTING;
    ba250[3] = MSG_CODE_FILL; ba250[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba250 );

    ShowStatusMessage( "Change to Amplifier Gain 250" );
}

/******************************************************************************

  Function: MenuActAmpGain500

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain500()
{
    QByteArray ba500;

    ba500.resize( REMOTE_CTRL_MSG_SIZE );
    ba500[0] = REMOTE_CTRL_HEADER; ba500[1] =  MSG_CODE_AMP_GAIN;
    ba500[2] = AMPLIFIER_GAIN_500_SETTING;
    ba500[3] = MSG_CODE_FILL; ba500[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( ba500 );

    ShowStatusMessage( "Change to Amplifier Gain 500" );
}

/******************************************************************************

  Function: MenuActCaptureRate250

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate250()
{
    QByteArray bpr250;
    bpr250.resize( REMOTE_CTRL_MSG_SIZE );
    bpr250[0] = REMOTE_CTRL_HEADER; bpr250[1] =  MSG_CODE_PICTURE_RATE;
    bpr250[2] = PICTURE_RATE_250MHZ;
    bpr250[3] = MSG_CODE_FILL; bpr250[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpr250 );
    ShowStatusMessage( "Change to Capture Rate 250" );
}

/******************************************************************************

  Function: MenuActCaptureRate500

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate500()
{
    QByteArray bpr500;
    bpr500.resize( REMOTE_CTRL_MSG_SIZE );
    bpr500[0] = REMOTE_CTRL_HEADER; bpr500[1] =  MSG_CODE_PICTURE_RATE;
    bpr500[2] = PICTURE_RATE_500MHZ;
    bpr500[3] = MSG_CODE_FILL; bpr500[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpr500 );
    ShowStatusMessage( "Change to Capture Rate 500" );
}

/******************************************************************************

  Function: MenuActCaptureRate1000

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate1000()
{
    QByteArray bpr1000;
    bpr1000.resize( REMOTE_CTRL_MSG_SIZE );
    bpr1000[0] = REMOTE_CTRL_HEADER; bpr1000[1] =  MSG_CODE_PICTURE_RATE;
    bpr1000[2] = PICTURE_RATE_1000MHZ;
    bpr1000[3] = MSG_CODE_FILL; bpr1000[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpr1000 );
    ShowStatusMessage( "Change to Capture Rate 1000" );
}

/******************************************************************************

  Function: MenuActCaptureRate1000

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate2000()
{
    QByteArray bpr2000;
    bpr2000.resize( REMOTE_CTRL_MSG_SIZE );
    bpr2000[0] = REMOTE_CTRL_HEADER; bpr2000[1] =  MSG_CODE_PICTURE_RATE;
    bpr2000[2] = PICTURE_RATE_2000MHZ;
    bpr2000[3] = MSG_CODE_FILL; bpr2000[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpr2000 );
    ShowStatusMessage( "Change to Capture Rate 2000" );
}

/******************************************************************************

  Function: MenuActCopy

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActCopy()
{
    SerialConsole->selectAll();
    SerialConsole->copy();
    ShowStatusMessage( "Copy Data" );
}

/******************************************************************************

  Function: MenuActCycleTime

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActCycleTime()
{
    QByteArray b;
    bool ok = false;
    const int value = 3;
    int returnui = value;

    returnui = QInputDialog::getInt( this, tr("Cycle Time"),
                      tr("Requires a Value Between 2 and 10 second"),
                      value ,
                      CYCLE_TIME_MIN,
                      CYCLE_TIME_MAX,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr( "Cycle Time" ),
               ( QString("The Cycle Time is %1 sec").arg( returnui )));

    b.resize( REMOTE_CTRL_MSG_SIZE );
    b[0] = REMOTE_CTRL_HEADER;
    b[1] =  MSG_CODE_PULSE_CYCLE_TIME;
    b[2] = (char) returnui;
    b[3] = MSG_CODE_FILL;
    b[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( b );

    ShowStatusMessage( "Change to Measure Distance" );
}

/******************************************************************************

  Function: MenuActDensity

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActDensity()
{
    QByteArray b;
    bool ok = false;
    const int value = 3;
    const unsigned top_byte_divisor = 0x100; //divide by this to get the top byte
    int returnui = value;

    if( Metric == false){
        returnui = QInputDialog::getInt( this, tr("Cycle Time"),
                      tr("Requires a Value Between 50 and 500 pounds per cubic foot"),
                      value ,
                      MAT_DENSITY_MIN,
                      MAT_DENSITY_MAX,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr( "Cycle Time" ),
               ( QString("The Density is %1 ").arg( returnui )));
    }else{
        returnui = QInputDialog::getInt( this, tr("Cycle Time"),
                      tr("Requires a Value Between 80 and 800 kg per cubic meter"),
                      value ,
                      MAT_DENSITY_MIN_MET,
                      MAT_DENSITY_MAX_MET,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr( "Cycle Time" ),
               ( QString("The Density is %1 ").arg( returnui )));
    }
    b.resize( REMOTE_CTRL_MSG_SIZE );
    b[0] = REMOTE_CTRL_HEADER;
    b[1] =  MSG_CODE_DENSITY;
    b[2] = (char) ( returnui / top_byte_divisor );
    b[3] = (char) ( returnui % top_byte_divisor );
    b[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( b );

    ShowStatusMessage( "Change to Measure Distance" );
}

/******************************************************************************

  Function: MenuActHiVolt

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActHiVolt()
{
    QByteArray bvolthi;
    bvolthi.resize( REMOTE_CTRL_MSG_SIZE );
    bvolthi[0] = REMOTE_CTRL_HEADER; bvolthi[1] =  MSG_CODE_PULSER_VOLTAGE;//??
    bvolthi[2] = MSG_CODE_HIVOLT ;
    bvolthi[3] = MSG_CODE_FILL; bvolthi[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bvolthi );

    ShowStatusMessage( "Change to Hi Voltage" );
}

/******************************************************************************

  Function: MenuActLoVolt

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActLoVolt()
{
    QByteArray bvoltlo;
    bvoltlo.resize( REMOTE_CTRL_MSG_SIZE );
    bvoltlo[0] = REMOTE_CTRL_HEADER; bvoltlo[1] =  MSG_CODE_PULSER_VOLTAGE;//??
    bvoltlo[2] = MSG_CODE_LOVOLT;
    bvoltlo[3] = MSG_CODE_FILL; bvoltlo[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bvoltlo );

    ShowStatusMessage( "Change to Lo Voltage" );
}

/******************************************************************************

  Function: MenuActMeasureDistance

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActDistance()
{
    QByteArray bdistance;
    bool ok = false;
    double velocity = 0.0;
    double scaler = 10.0; //some kind of scale constant
    const unsigned top_byte_divisor = 0x100; //divide by this to get the top byte
    const double dvalue = 3000;
    const double dvaluemetric = 1000;
    int returnui;

    if(Metric == false){
        velocity = QInputDialog::getDouble( this, tr("Velocity"),
                      tr("Requires a Value Between 100 and 40000 feet per second"),
                      dvalue ,
                      MAT_TRAVEL_VEL_MIN,
                      MAT_TRAVEL_VEL_MAX,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr( "Velocity" ),
               ( QString("The Velocity is %1 feet/sec").arg( velocity )));
    }else{
        velocity = QInputDialog::getDouble( this, tr("Velocity"),
                      tr("Requires a Value Between 100 and 40000 feet per second"),
                      dvaluemetric ,
                      MAT_TRAVEL_VEL_MIN_MET / scaler,
                      MAT_TRAVEL_VEL_MAX_MET / scaler,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr("Velocity"),
               ( QString("The Velocity is %1 meters/second").arg( velocity )));
        velocity *= 10.0;
    }


    returnui = (int) velocity;

    bdistance.resize( REMOTE_CTRL_MSG_SIZE );
    bdistance[0] = REMOTE_CTRL_HEADER;
    bdistance[1] =  MSG_CODE_MSMT_MODE;
    bdistance[2] = MSG_CODE_VELOCITY;
    bdistance[3] = MSG_CODE_FILL;
    bdistance[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( bdistance );

    bdistance.resize( REMOTE_CTRL_MSG_SIZE ); //this not working?
    bdistance[0] = REMOTE_CTRL_HEADER;
    bdistance[1] = MSG_CODE_CH_VELOCITY;
    bdistance[2] = (char) ( returnui / top_byte_divisor );
    bdistance[3] = (char) ( returnui % top_byte_divisor );
    bdistance[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( bdistance );

    ShowStatusMessage( "Change to Measure Distance" );
}

/******************************************************************************

  Function: MenuActMetric

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActMetric()
{
    Metric = false;
    ShowStatusMessage( "Change to Metric Units" );
}

/******************************************************************************

  Function: MenuActOpen

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActOpen()
{
    ShowStatusMessage( "Open File" );
    ShowStatusMessage( tr( "Save File" ));
    QString fileName = QFileDialog::getOpenFileName(this, tr("V-Meter Saved File"),
                                QDir::homePath(), tr("All Files (*);;Text Files (*.txt)"));
    QFile file( fileName );
    if (!fileName.isEmpty()){
        QTextStream out( &file );
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "MainWindow",
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
        }

#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        SerialConsole->setPlainText( out.readAll());
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        ShowStatusMessage( fileName );
     }
}

/******************************************************************************

  Function: MenuActPlot

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPlot()
{
    Plot->~QwtPlot();
    Plot = new QwtPlot;
    Plot->setTitle( "V-Meter Signal Plot" );
    Plot->setCanvasBackground( Qt::white );

    Plot->insertLegend( new QwtLegend() );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach( Plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "V-Meter Raw Data" );
    curve->setPen( Qt::blue, 4 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve->setSymbol( symbol );

    QPolygonF points;
    QString rawdata = SerialConsole->toPlainText();
    QStringList rawdatalist = rawdata.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    bool numtest = false;
    double addbufffer = 0;
    double pointscount = 0.0;
    foreach( const QString &str, rawdatalist ){
        addbufffer = str.toDouble(&numtest);
        if( numtest == true ) {
            points << QPointF( pointscount, addbufffer);
            pointscount += 1.0;
        }
    }

    curve->setSamples( points );
    curve->attach( Plot );

    Plot->replot();

    Plot->resize(600, 400);
    Plot->show();

    ShowStatusMessage( "Plot" );
}

/******************************************************************************

  Function: MenuActPicNo

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPicNo()
{
    QByteArray bpsf;
    bpsf.resize( REMOTE_CTRL_MSG_SIZE );
    bpsf[0] = REMOTE_CTRL_HEADER; bpsf[1] =  MSG_CODE_DISPLAY_SIG;//??
    bpsf[2] = MSG_CODE_OFF;
    bpsf[3] = MSG_CODE_FILL; bpsf[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpsf );

    ShowStatusMessage( tr("Save Picture No" ));
}

/******************************************************************************

  Function: MenuActPicYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPicYes()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_DISPLAY_SIG;//??
    bpst[2] = MSG_CODE_ON;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );

    ShowStatusMessage( tr("Save Picture Yes" ));
}

/******************************************************************************

  Function: MenuActPulse1

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPulse1()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_PULSE_FREQ;
    bpst[2] = PULSES_PER_SEQ_1;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );
    ShowStatusMessage( tr( "Change Pulse to 1" ));
}

/******************************************************************************

  Function: MenuActPulse3

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPulse3()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_PULSE_FREQ;
    bpst[2] = PULSES_PER_SEQ_3;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );
    ShowStatusMessage( tr( "Change Pulse to 3" ));
}

/******************************************************************************

  Function: MenuActPulse10

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPulse10()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_PULSE_FREQ;
    bpst[2] = PULSES_PER_SEQ_10;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );
    ShowStatusMessage( tr( "Change Pulse to 10" ));
}

/******************************************************************************

  Function: MenuActRunNo

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActRunNo()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_RUN_A_TEST;//??
    bpst[2] = MSG_CODE_OFF;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );
    ShowStatusMessage( tr( "Run No" ));
}

/******************************************************************************

  Function: MenuActRunYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActRunYes()
{
    QByteArray bpst;
    bpst.resize( REMOTE_CTRL_MSG_SIZE );
    bpst[0] = REMOTE_CTRL_HEADER; bpst[1] =  MSG_CODE_RUN_A_TEST;//??
    bpst[2] = MSG_CODE_ON;
    bpst[3] = MSG_CODE_FILL; bpst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bpst );
    ShowStatusMessage( tr("Run Yes" ));
}

/******************************************************************************

  Function: MenuActSaveDataNo

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActSaveDataNo()
{
    QByteArray bdsf;
    bdsf.resize( REMOTE_CTRL_MSG_SIZE );
    bdsf[0] = REMOTE_CTRL_HEADER; bdsf[1] =  MSG_CODE_SAVE_TEST;
    bdsf[2] = MSG_CODE_OFF;
    bdsf[3] = MSG_CODE_FILL; bdsf[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bdsf );

    ShowStatusMessage( tr( "Save No" ));
}

/******************************************************************************

  Function: MenuActSaveDataYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActSaveDataYes()
{
    QByteArray bdst;
    bdst.resize( REMOTE_CTRL_MSG_SIZE );
    bdst[0] = REMOTE_CTRL_HEADER; bdst[1] =  MSG_CODE_SAVE_TEST;
    bdst[2] = MSG_CODE_ON;
    bdst[3] = MSG_CODE_FILL; bdst[4] = REMOTE_CTRL_FOOTER;

    SerialPortWriteData( bdst );

    ShowStatusMessage( tr( "Save Yes" ));
}

/******************************************************************************

  Function: MenuActSave

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActSave()
{
    ShowStatusMessage( tr( "Save File" ));
    QString fileName = QFileDialog::getSaveFileName(this, tr("V-Meter Saved File"),
                                QDir::homePath(), tr("All Files (*);;Text Files (*.txt)"));
    QFile file( fileName );
    if (!fileName.isEmpty()){
        QTextStream out( &file );
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "MainWindow",
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
        }

#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        out << SerialConsole->toPlainText();
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        ShowStatusMessage( fileName );
     }
}

/******************************************************************************

  Function: MenuActTestNumber

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActTestNumber()
{
    QStringList items;

    for( QVector< InstData::Test >::iterator itr = ( CurrentData->GetBeginItr());
         itr != CurrentData->GetEndItr(); ++itr ){

        items << QString::number( itr->TestNumber, 10 );
    }

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                               tr("Test Number"), items, 0, false, &ok);
    unsigned displaytest;
    if (ok && !item.isEmpty()){
        for( QVector< InstData::Test >::iterator itr = ( CurrentData->GetBeginItr());
            itr != CurrentData->GetEndItr(); ++itr ){
                if( item == QString::number( itr->TestNumber ))
                    displaytest = itr->TestNumber;
            }
    }
    SerialConsole->clear();
    SerialConsole->putData( CurrentData->GetTest( displaytest, &ok));
    SerialConsole->setFocus();
    SerialConsole->moveCursor( QTextCursor::Start );
    ShowStatusMessage( tr( "Select Test Number" ));
}
/******************************************************************************

  Function: MenuActUSC

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActUSC()
{
    Metric = false;
    ShowStatusMessage( "Change to US Customary Units" );
}

/******************************************************************************

  Function: MenuActVelocity

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActVelocity()
{
    ShowStatusMessage( "Transducer Velocity Changed" );

    bool ok = false;
    double distance = 0.0;
    QByteArray bdistance;
    const unsigned top_byte_divisor = 0x100; //divide by this to get the top byte
    const double scaler = 10.0; //some kind of scale constant
    const double dvalue = 3000;
    const double dvaluemetric = 1000;
    unsigned returnui;

    if(Metric == false){
        distance = QInputDialog::getDouble( this, tr("Distance"),
                      tr("Requires a Value Between 0.1 and 600 feet"),
                      dvalue ,
                      MAT_TRAVEL_DIST_MIN,
                      MAT_TRAVEL_DIST_MAX ,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr( "Distance" ),
               ( QString("The Distance is %1 feet").arg( distance )));
    }else{
        distance = QInputDialog::getDouble( this, tr("Distance"),
                      tr("Requires a Value Between 0.003 and 15 meters per second"),
                      dvaluemetric ,
                      MAT_TRAVEL_DIST_MIN_MET * scaler,
                      MAT_TRAVEL_DIST_MAX_MET * scaler,
                      0, &ok );
        if(ok) QMessageBox::information( this, tr("Distance"),
               ( QString("The Distance is %1 meters/second").arg( distance )));
    }

    distance *= 10.0;

    returnui = ( int ) distance;
    bdistance.resize( REMOTE_CTRL_MSG_SIZE );
    bdistance[0] = REMOTE_CTRL_HEADER;
    bdistance[1] =  MSG_CODE_MSMT_MODE;
    bdistance[2] = MSG_CODE_DISTANCE; // codes a reversed?
    bdistance[3] = MSG_CODE_FILL;
    bdistance[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( bdistance );

    bdistance.resize( REMOTE_CTRL_MSG_SIZE ); // this section not working
    bdistance[0] = REMOTE_CTRL_HEADER;
    bdistance[1] = MSG_CODE_CH_DISTANCE;
    bdistance[2] = (char) ( returnui / top_byte_divisor );
    bdistance[3] = (char) ( returnui % top_byte_divisor );
    bdistance[4] = REMOTE_CTRL_FOOTER;
    SerialPortWriteData( bdistance );

    ShowStatusMessage( "Change to Measure Velocity" );
}

/******************************************************************************

  Function: SerialCheckPort()

  Description:
  ============
  Checks that an instrument is connected to the PC

******************************************************************************/
void MainWindow::SerialCheckPort()
{
    QString description;
    QString manufacturer;
    QString portname;

    const QString portmanufacturer = "FTDI";
    const QString noport = tr("No Available Ports") + '\n'
                              + tr("Check instrument is plugged in") + '\n'
                              + tr("or serial port installed properly") + '\n'
                              + tr("then restart Rebarlinx");
    const QString messageTitle = tr("Check Serial Port");
    const QString connected = tr("Connected to ");
    QList <QSerialPortInfo> availablePorts;
    bool r = false;
    availablePorts = QSerialPortInfo::availablePorts();

    if(!availablePorts.isEmpty()){
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            portname = info.portName();
            description = info.description();
            manufacturer = info.manufacturer();
            if(manufacturer == portmanufacturer){
                Serial->setPortName(portname);
                r = true;
            }
        if( r == true ) break;
        }
    }
    if(r == false){
        QMessageBox::information(this,messageTitle,noport);
    }else{
        QMessageBox::information(this ,messageTitle , connected + portname );
        ShowStatusMessage( "Serial Port Found" );
        SerialPortOpen(); // Found the instrument open the port
    }
}

/******************************************************************************

  Function: SerialCreateActions

  Description:
  ============


******************************************************************************/
void MainWindow::SerialCreateActions()
{
    connect(Serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(SerialPortHandleError(QSerialPort::SerialPortError)));
    connect(Serial, SIGNAL(readyRead()), this, SLOT(SerialPortReadData()));
    connect(SerialTimeOut,SIGNAL(timeout()), this, SLOT( SerialDataRecieved() ));

}

/******************************************************************************

  Function: SerialDataRecieved

  Description:
  ============
 - removed confirmation messagebox

******************************************************************************/
void MainWindow::SerialDataRecieved()
{
    QString uploadeddata = SerialConsole->toPlainText();

    SerialTimeOut->stop();
    if( MessageReply == false ){
        CurrentData->AddTest( uploadeddata );
        CreateTestMenus();
    }
#ifndef R_DEBUG
    SerialConsole->clear();
#endif
    SerialConsole->setFocus();
    SerialConsole->moveCursor( QTextCursor::Start );
    NewUpload = false;
}

/******************************************************************************

  Function: SerialPortOpen

  Description:
  ============


******************************************************************************/
void MainWindow::SerialPortOpen()
{
    const int baudrate = 9600;
    Serial->setBaudRate( baudrate );
    Serial->setDataBits(QSerialPort::Data8);
    Serial->setParity(QSerialPort::NoParity);
    Serial->setStopBits(QSerialPort::OneStop);
    Serial->setFlowControl(QSerialPort::NoFlowControl);
    if (Serial->open(QIODevice::ReadWrite)) {
        SerialConsole->setEnabled(true);
        SerialConsole->setLocalEchoEnabled(true);
        ShowStatusMessage(tr("Connected"));
    } else {
        QMessageBox::critical(this, tr("Error"), Serial->errorString());

        ShowStatusMessage(tr("Open error"));
    }
}

/******************************************************************************

  Function: SerialPortClose

  Description:
  ============


******************************************************************************/
void MainWindow::SerialPortClose()
{
    if (Serial->isOpen())
        Serial->close();
    SerialConsole->setEnabled(false);

    ShowStatusMessage(tr("Serial Port Closed"));
}

/******************************************************************************

  Function: SerialPortReadData

  Description:
  ============


******************************************************************************/
void MainWindow::SerialPortReadData()
{
    if( NewUpload == false ){
        SerialConsole->clear();
    }

    NewUpload = true;
    const int timeouttime = 200; //200 mSec after the last character it should stop
    QByteArray data = Serial->readAll();
    SerialConsole->putData( data );
    SerialTimeOut->start( timeouttime );
}

/******************************************************************************

  Function: SerialPortWriteData

  Description:
  ============


******************************************************************************/
void MainWindow::SerialPortWriteData(const QByteArray &data)
{
    MessageReply = true;
    NewUpload = false;
    const int reviewtestnumsec = 5;
    const int sec = 2;

    if( Serial->clear()){
         Serial->write(data);
    }
    QTime dieTime;
    if( data[1] ==  MSG_CODE_REVIEW_TEST_NUM ){
        dieTime = QTime::currentTime().addSecs( reviewtestnumsec ); // wait five second
    }else{
        dieTime = QTime::currentTime().addSecs( sec ); // wait two second
    }
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/******************************************************************************

  Function: SerialPortHandleError

  Description:
  ============


******************************************************************************/
void MainWindow::SerialPortHandleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), Serial->errorString());
        SerialPortClose();
    }
}

/******************************************************************************

  Function: ShowStatusMessage

  Description:
  ============


******************************************************************************/
void MainWindow::ShowStatusMessage(QString message)
{
    Status->setText( message );
}
