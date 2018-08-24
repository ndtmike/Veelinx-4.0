/****************************************************************************
**
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: mainwindow.h
**
** mainwindow.cpp Rebarlinx software
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtWidgets>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QDialog;
class QLabel;
class QMenu;
class QSerialPort;
class QSerialPortInfo;
QT_END_NAMESPACE

class Console;
class Setting_Dialog;
class InstData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void MenuActAbout();
    void MenuActAmpGain1();
    void MenuActAmpGain5();
    void MenuActAmpGain10();
    void MenuActAmpGain25();
    void MenuActAmpGain50();
    void MenuActAmpGain100();
    void MenuActAmpGain250();
    void MenuActAmpGain500();
    void MenuCaptureRate250();
    void MenuCaptureRate500();
    void MenuCaptureRate1000();
    void MenuCaptureRate2000();
    void MenuActDistance();
    void MenuActCopy();
    void MenuActHiVolt();
    void MenuActLoVolt();
    void MenuActMeasureDistance();
    void MenuActMeasureVelocity();
    void MenuActMetric();
    void MenuActNewFile();
    void MenuActOpen();
    void MenuActPlot();
    void MenuActPicNo();
    void MenuActPicYes();
    void MenuActRunNo();
    void MenuActRunYes();
    void MenuActSaveDataNo();
    void MenuActSaveDataYes();
    void MenuActSave();
//    void MenuActSettings();
    void MenuActUSC();
    void MenuActVelocity();

    void SerialCheckPort();
    void SerialDataRecieved();
    void SerialPortClose();
    void SerialPortHandleError(QSerialPort::SerialPortError error);
    void SerialPortOpen();
    void SerialPortReadData();
    void SerialPortWriteData(const QByteArray &data);

private:
    QByteArray Data;
    bool NewUpload;

    struct SerialSettings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };

    void CreateActions();
    void CreateMenus();
    void CreateStatusBar();
    void SerialCreateActions();

    SerialSettings CurrentSerialSettings;

    void ShowStatusMessage( QString message );

    QMenu *AmpGainMenu;
    QMenu *CaptureRateMenu;
    QMenu *MeasureMenu;
    QMenu *PulseRateMenu;
    QMenu *FileMenu;
    QMenu *EditMenu;
    QMenu *helpMenu;
    QMenu *RunMenu;
    QMenu *SaveDataMenu;
    QMenu *SavePicMenu;
    QMenu *ToolMenu;
    QMenu *UnitsMenu;
    QMenu *VoltMenu;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *copyAct;
    QAction *exitAct;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;

    QAction *AmpGain1Act;
    QAction *AmpGain5Act;
    QAction *AmpGain10Act;
    QAction *AmpGain25Act;
    QAction *AmpGain50Act;
    QAction *AmpGain100Act;
    QAction *AmpGain250Act;
    QAction *AmpGain500Act;
    QAction *CaptureRate250Act;
    QAction *CaptureRate500Act;
    QAction *CaptureRate1000Act;
    QAction *CaptureRate2000Act;
    QAction *DistanceAct;
    QAction *HiVoltAct;
    QAction *MeasureDistAct;
    QAction *MeasureVelAct;
    QAction *MetricAct;
    QAction *PlotAct;
    QAction *PulseRate1Act;
    QAction *PulseRate3Act;
    QAction *PulseRate10Act;
    QAction *RunNoAct;
    QAction *RunYesAct;
    QAction *SaveDataNoAct;
    QAction *SaveDataYesAct;
    QAction *SavePicNoAct;
    QAction *SavePicYesAct;
    QAction *SettingsAct;
    QAction *LoVoltAct;
    QAction *USCAct;
    QAction *VelocityAct;

    QActionGroup *AmpGainActGrp;
    QActionGroup *CaptureRateActGrp;
    QActionGroup *DensityGrp;
    QActionGroup *MeasureActGrp;
    QActionGroup *PulseRateActGrp;
    QActionGroup *RunGrp;
    QActionGroup *SaveDataGrp;
    QActionGroup *SavePicGrp;
    QActionGroup *VoltageActGrp;
    QActionGroup *UnitsActGrp;

    InstData *CurrentData;
    QwtPlot *Plot;
    QSerialPort *Serial;
    QTimer *SerialTimeOut;
    Console *SerialConsole;
    QLabel *Status;
};

#endif // MAINWINDOW_H
