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
        unsigned PropDensity;
        EMethod PropEMethod;
        bool PropPicSave;
        double PropDistance;
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
    Test GetTest( std::vector<Test>::iterator itr_current );
    std::vector<Test>::iterator GetBeginItr();
    std::vector<Test>::iterator GetEndItr();

private:

    std::vector <Test> TestData;
    static double MMtoInch(void){return(25.4);}
    static double MPAtoPSI(void){return(145.0);}

    AmpGain CreateAmpGain( QStringList rawproperties);
    Properties CreateCalc( QStringList rawdata, Properties rawproperties );
    CaptureRate CreateCaptureRate( QStringList rawproperties );
    QDateTime CreateDateTime( QString rawtest );
    Properties CreateProperties( QStringList rawproperties );
    Test CreateTest( QStringList rawtest);
    unsigned CreateTestNumber( QString rawtest );
    double CreateTransitTime( QStringList rawtest );

};

#endif // INST_DATA_H
