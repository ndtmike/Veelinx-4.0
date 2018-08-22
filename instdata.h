 /* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: Instdata.h
**
** Header file for instdata.cpp - Veelinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2018
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#ifndef INST_DATA_H
#define INST_DATA_H

//#include <math.h>  this was double defined not really sure where the original is defined?
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <functional>

#include <QDateTime>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QVector>

class InstData
{
public:
    enum AmpGain{ Gain_1 = 1, Gain_5 = 5, Gain_10 = 10, Gain_25 = 25, Gain_50 = 50, Gain_100 = 100, Gain_250 = 250, Gain_500 = 500 };
    enum Calc{ Distance, Velocity };
    enum EMethod{ ArbMu = 0, DerivedMu, SimpleE };
    enum Pulse{ PulsePerSeq_1 = 1, PulsePerSeq_3 = 3, PulsePerSeq_10 = 10};
    enum CaptureRate{ RATE_250KHZ = 250, RATE_500KHZ = 500, RATE_1000KHZ = 1000 , RATE_2000KHZ = 2000 };
    enum Units{ USC = 0, Metric};
    enum Voltage{ Low = 0, Hi };
    enum Wave{ PWave = 0, SWave };

    struct Properties{ //material property parameters for Windsor Probe
        AmpGain PropAmpGain;
        Calc PropCalc;
        CaptureRate PropCaptureRate;
        unsigned PropCycleTime;
        bool PropDataSave;
        double PropDensity;
        double PropDistance;
        double PropE;
        EMethod PropEMethod;
        double PropMu;
        bool PropPicSave;
        double PropVelocity;
        bool PropRun;
        Pulse PropPulseRate;
        Wave PropWave;
        Units PropUnits;
        Voltage PropVoltage;
    };

    struct Test{
        QVector<long> ADC;
        double TransitTime;
        Properties TestProp;
        QDateTime TestTime;
        unsigned TestNumber;
    };

    InstData( void );
//    InstData( Test Init_Test ); //constructor!!

    void AddTest( Test test );
    void AddTest( QString rawdata );
    Test GetTest( QVector<Test>::iterator itr_current );
    QStringList GetTest( unsigned gettestnumber, bool* ok );
    QVector<Test>::iterator GetBeginItr();
    QVector<Test>::iterator GetEndItr();

private:

    QVector <Test> TestData;
    static double MMtoInch(void){return(25.4);}
    static double MPAtoPSI(void){return(145.0);}

    AmpGain CreateAmpGain( QStringList rawproperties);
    Properties CreateCalc( QStringList rawdata, Properties rawproperties );
    CaptureRate CreateCaptureRate( QStringList rawproperties );
    QDateTime CreateDateTime( QString rawtest );
    double CreateDensity( QStringList rawdata );
    Properties CreateE( QStringList* rawlist, Properties* rawproperties );
    Properties CreateProperties( QStringList rawproperties );
    Test CreateTest( QStringList rawtest);
    unsigned CreateTestNumber( QString rawtest );
    double CreateTransitTime( QStringList rawtest );

    QStringList GetADC( Test* workingtest );
    QString GetAmpGain( Test* workingtest );
    QStringList GetCalc( Test* workingtest );
    QString GetCaptureRate( Test* workingtest );
    QString GetDateTime( Test* workingtest );
    QString GetDensity( Test* workingtest );
    QString GetE( Test* workingTest );
    QString GetTestNumber( Test* workingtest );
    QString GetTransitTime( Test* workingtest );
};

#endif // INST_DATA_H
