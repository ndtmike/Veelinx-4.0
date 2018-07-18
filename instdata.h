 /* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: Inst_Data.h
**
** Header file for parser.cpp - Veelinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2017
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#ifndef INST_DATA_H
#define INST_DATA_H

//#include <math.h>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <functional>

#include <QObject>

/* ---------------------------------------------------------------------------
** Holds the data for each test as well as each tests properties in a vector.
** The class does not 'code/decode' the data to strings that is assumed to happen
** in the QT interface. Thus allowing ease in translation.
** -------------------------------------------------------------------------*/

class InstData//:QObject
{
//    Q_OBJECT
public:
    enum AmpGain{ Gain_1 = 0, Gain_5, Gain_10, Gain_25, Gain_50, Gain_100, Gain_250, Gain_500 };
    enum EMethod{ ArbMu = 0, DerivedMu, SimpleE };
    enum Calc{ Distance, Velocity };
    enum Pulse{ PulsePerSeq_1 = 1, PulsePerSeq_3 = 3, PulsePerSeq_10 = 10};
    enum Rate{ RATE_250KHZ = 0, RATE_500KHZ, RATE_1000KHZ, RATE_2000KHZ };
    enum Units{ Imperial = 0, Metric};
    enum Voltage{ Low = 0, Hi };
    enum Wave{ PWave = 0, SWave };

    struct Properties{ //material property parameters for Windsor Probe
        AmpGain PropAmpGain;
        Calc PropCalc;
        Rate PropCaptureRate;
        unsigned PropCycleTime;
        bool PropDataSave;
        unsigned PropDensity;
        EMethod PropEMethod;
        Calc PropMeaseMode;
        unsigned PropPTravelDistance;
        unsigned PropPTravelVelocity;
        bool PropRun;
        bool PropPicSave;
        Pulse PropPulseRate;
        Wave PropWave;
        Units PropUnits;
        Voltage PropVoltage;
    };

    struct Test{
        std::vector<long> ADC;
        double TransitTime;
        Properties TestProp;
        tm TestTime;
        unsigned TestNumber;
    };

    InstData( void );
//    InstData( Test Init_Test ); //constructor!!

    void AddTest( Test test );
    void AddTest( QByteArray array );
    Test GetTest( std::vector<Test>::iterator itr_current );
    std::vector<Test>::iterator GetBeginItr();
    std::vector<Test>::iterator GetEndItr();

private:

    std::vector <Test> TestData;
    static double MMtoInch(void){return(25.4);}
    static double MPAtoPSI(void){return(145.0);}

};

#endif // INST_DATA_H
