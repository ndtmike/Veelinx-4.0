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

#include "mainwindow.h"
#include "console.h"
#include "setting_dialog.h"
#include "instdata.h"

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

    HiVoltAct = new QAction(tr( "Hi Voltage Pulse" ), this);
    HiVoltAct->setStatusTip(tr( "Change to Hi Voltage Pulse" ));
    HiVoltAct->setCheckable ( true );
    HiVoltAct->setChecked( true );
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
    connect( MeasureDistAct, &QAction::triggered, this, &MainWindow::MenuActMeasureDistance );

    MeasureVelAct = new QAction(tr( "Velocity" ), this);
    MeasureVelAct->setStatusTip(tr( "Change to Measure Velocity Between Transducers" ));
    MeasureVelAct->setCheckable ( true );
    MeasureVelAct->setChecked( false );
    connect( MeasureVelAct, &QAction::triggered, this, &MainWindow::MenuActMeasureVelocity );

    MeasureActGrp = new QActionGroup( this );
    VoltageActGrp->addAction( MeasureDistAct );
    VoltageActGrp->addAction( MeasureVelAct );

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

    DistanceAct = new QAction(tr("&Distance"), this);
    saveAct->setStatusTip(tr("Set the Distance Between Transducers"));
    connect(DistanceAct, &QAction::triggered, this, &MainWindow::MenuActDistance);

    VelocityAct = new QAction(tr("&Velocity"), this);
    saveAct->setStatusTip(tr("Set the Velocity Between Transducers"));
    connect(VelocityAct, &QAction::triggered, this, &MainWindow::MenuActVelocity);

    UnitsActGrp = new QActionGroup( this );
    UnitsActGrp->addAction( USCAct );
    UnitsActGrp->addAction( MetricAct );

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
    RunMenu = ToolMenu->addMenu( tr( "&Run" ));
    RunMenu->addAction( RunYesAct );
    RunMenu->addAction( RunNoAct );
    SaveDataMenu = ToolMenu->addMenu( tr( "Sa&ve Data" ));
    SaveDataMenu->addAction( SaveDataYesAct );
    SaveDataMenu->addAction( SaveDataNoAct );
    SavePicMenu = ToolMenu->addMenu(tr("Save &Pic"));
    SavePicMenu->addAction( SavePicYesAct );
    SavePicMenu->addAction( SavePicNoAct );

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

    MeasureMenu = ToolMenu->addMenu( tr("&Measure"));
    MeasureMenu->addAction( MeasureDistAct );
    MeasureMenu->addAction( MeasureVelAct );

    VoltMenu = ToolMenu->addMenu( tr( "&Voltage" ));
    VoltMenu->addAction( HiVoltAct );
    VoltMenu->addAction( LoVoltAct );

    ToolMenu->addSeparator();
    ToolMenu->addAction( DistanceAct );
    ToolMenu->addAction( VelocityAct );

    ToolMenu->addSeparator();
    UnitsMenu = ToolMenu->addMenu( tr( "&Units" ));
    UnitsMenu->addAction( USCAct );
    UnitsMenu->addAction( MetricAct );

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
    const QString about_rebarlinx = tr("The <b>Rebarlinx Sofware</b> is for use with <br>"
                                       "the James Instruments Inc.<br>"
                                       "<a href=\"https://www.ndtjames.com/Rebarscope-Complete-System-p/r-c-4.htm\">Rebarscope</a><br>"
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
    ShowStatusMessage( "Change to Amplifier Gain 1" );
}

/******************************************************************************

  Function: MenuActAmpGain5

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain5()
{
    ShowStatusMessage( "Change to Amplifier Gain 5" );
}

/******************************************************************************

  Function: MenuActAmpGain10

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain10()
{
    ShowStatusMessage( "Change to Amplifier Gain 10" );
}


/******************************************************************************

  Function: MenuActAmpGain25

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain25()
{
    ShowStatusMessage( "Change to Amplifier Gain 25" );
}

/******************************************************************************

  Function: MenuActAmpGain50

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain50()
{
    ShowStatusMessage( "Change to Amplifier Gain 50" );
}

/******************************************************************************

  Function: MenuActAmpGain100

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain100()
{
    ShowStatusMessage( "Change to Amplifier Gain 100" );
}

/******************************************************************************

  Function: MenuActAmpGain250

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain250()
{
    ShowStatusMessage( "Change to Amplifier Gain 250" );
}

/******************************************************************************

  Function: MenuActAmpGain500

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActAmpGain500()
{
    ShowStatusMessage( "Change to Amplifier Gain 500" );
}

/******************************************************************************

  Function: MenuActCaptureRate250

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate250()
{
    ShowStatusMessage( "Change to Capture Rate 250" );
}

/******************************************************************************

  Function: MenuActCaptureRate500

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate500()
{
    ShowStatusMessage( "Change to Capture Rate 500" );
}

/******************************************************************************

  Function: MenuActCaptureRate1000

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate1000()
{
    ShowStatusMessage( "Change to Capture Rate 1000" );
}

/******************************************************************************

  Function: MenuActCaptureRate1000

  Description:
  ============

******************************************************************************/
void MainWindow::MenuCaptureRate2000()
{
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

  Function: MenuActDistance

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActDistance()
{
    bool ok = false;

    ShowStatusMessage( "Transducer Distance Changed" );

    double distance = QInputDialog::getDouble( this, tr("Distance"), tr("Requires a value between 0.1 and 600 feet "), 1 , 0.1, 600, 1, &ok );

    if(ok) QMessageBox::information( this, tr("Distance"), ( QString("The Distance is %1 feet").arg(distance)));

}
/******************************************************************************

  Function: MenuActHiVolt

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActHiVolt()
{
    ShowStatusMessage( "Change to Hi Voltage" );
}

/******************************************************************************

  Function: MenuActLoVolt

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActLoVolt()
{
    ShowStatusMessage( "Change to Lo Voltage" );
}

/******************************************************************************

  Function: MenuActMeasureDistance

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActMeasureDistance()
{
    ShowStatusMessage( "Change to Measure Distance" );
}

/******************************************************************************

  Function: MenuActMeasureDistance

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActMeasureVelocity()
{
    ShowStatusMessage( "Change to Measure Velocity" );
}

/******************************************************************************

  Function: MenuActMetric

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActMetric()
{
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
    ShowStatusMessage( tr("Save Picture No" ));
}

/******************************************************************************

  Function: MenuActPicYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActPicYes()
{
    ShowStatusMessage( tr("Save Picture Yes" ));
}

/******************************************************************************

  Function: MenuActRunNo

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActRunNo()
{
    ShowStatusMessage( tr( "Run No" ));
}

/******************************************************************************

  Function: MenuActRunYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActRunYes()
{
    ShowStatusMessage( tr("Run Yes" ));
}

/******************************************************************************

  Function: MenuActSaveNo

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActSaveDataNo()
{
    ShowStatusMessage( tr( "Save No" ));
}

/******************************************************************************

  Function: MenuActSaveYes

  Description:
  ============


******************************************************************************/
void MainWindow::MenuActSaveDataYes()
{
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

  Function: MenuActMetric

  Description:
  ============

******************************************************************************/
void MainWindow::MenuActUSC()
{
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

    double distance = QInputDialog::getDouble( this, tr("Velocity"),
                      tr("Requires a Value Between 100 and 40000 feet per second"),
                      3000 , 100, 40000, 0, &ok );

    if(ok)
        QMessageBox::information( this, tr("Distance"),
                      ( QString("The Velocity is %1 feet").arg(distance)));

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
    connect(SerialTimeOut,SIGNAL(timeout()), this,SLOT(SerialDataRecieved()));

}

/******************************************************************************

  Function: SerialCreateActions

  Description:
  ============


******************************************************************************/
void MainWindow::SerialDataRecieved()
{
    QString uploadeddata = SerialConsole->toPlainText();

    QMessageBox::information(this, tr("Serial Port"), tr("End of Upload"));
    SerialTimeOut->stop();
    CurrentData->AddTest( uploadeddata );
    SerialConsole->clear();

    CreateTestMenus();

    uploadeddata.clear();
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
    const int timeouttime = 400; //200 mSec after the last character it should stop
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
    Serial->write(data);
}

/******************************************************************************

  Function: handleError

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
