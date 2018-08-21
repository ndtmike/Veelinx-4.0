/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name:  instdata.cpp - Veelinx software
** Divides the input stream up into data packets
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2018
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "instdata.h"

/******************************************************************************

  Function: InstData

  Description:
  ============
  Constructor

******************************************************************************/

InstData::InstData()
{

}

/******************************************************************************

  Function: AddTest

  Description:
  ============
  Add with Test

******************************************************************************/
void InstData::AddTest(InstData::Test test)
{
    TestData.push_back( test );
}

/******************************************************************************

  Function: AddTest

  Description:
  ============
  Add with QString

******************************************************************************/
void InstData::AddTest(QString rawdata)
{
    QStringList rawdatalist = rawdata.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    const QString const_james = "James Instruments V-METER MK IV";
    const QString const_end = "End";
    QStringList working;

    QStringList::const_iterator constIterator;
    for( constIterator = rawdatalist.constBegin(); constIterator != rawdatalist.constEnd(); ++constIterator )
    {
        if( (*constIterator) == const_james )
        {
            for(;(*constIterator) != const_end && constIterator != rawdatalist.constEnd(); ++constIterator)
            {
                working << (*constIterator);
            }
            CreateTest( working );
        }
        working.clear();
    }
}

/******************************************************************************

  Function: CreateAmpGain

  Description:
  ============
  Used with CreateDateTime QString

******************************************************************************/
InstData::AmpGain InstData::CreateAmpGain( QStringList rawproperties )
{
    const QString gain_1 = "* 1 *";
    const QString gain_5 = "* 5 *";
    const QString gain_10 = "* 10 *";
    const QString gain_25 = "* 25 *";
    const QString gain_50 = "* 50 *";
    const QString gain_100 = "* 100 *";
    const QString gain_250 = "* 250 *";
    const QString gain_500 = "* 500 *";

    InstData::AmpGain return_property = AmpGain::Gain_50;

    QRegExp regexp( gain_500 );
    regexp.setPatternSyntax( QRegExp::Wildcard);

    int index = rawproperties.indexOf( regexp );

    if( index != -1 ){ //determine the rate
        return_property = AmpGain::Gain_500;
    }else{
        regexp.setPattern( gain_250 );
        index = rawproperties.indexOf( regexp );
        if( index != -1 ){
            return_property = AmpGain::Gain_250;
        }else{
            regexp.setPattern( gain_100 );
            index = rawproperties.indexOf( regexp );
            if( index != -1 ){
                return_property = AmpGain::Gain_100;
            }else{
                regexp.setPattern( gain_50 );
                index = rawproperties.indexOf( regexp );
                if( index != -1 ){
                    return_property = AmpGain::Gain_50;
                }else{
                    regexp.setPattern( gain_25 );
                    index = rawproperties.indexOf( regexp );
                    if( index != -1 ){
                        return_property = AmpGain::Gain_25;
                    }else{
                        regexp.setPattern( gain_10 );
                        index = rawproperties.indexOf( regexp );
                        if( index != -1 ){
                            return_property = AmpGain::Gain_10;
                        }else{
                            regexp.setPattern( gain_5 );
                            index = rawproperties.indexOf( regexp );
                            if( index != -1 ){
                                return_property = AmpGain::Gain_5;
                            }else{
                                regexp.setPattern( gain_1 );
                                index = rawproperties.indexOf( regexp );
                                if( index != -1 ){
                                    return_property = AmpGain::Gain_1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return( return_property );
}

/******************************************************************************

  Function: CreateCalc

  Description:
  ============


******************************************************************************/
InstData::Properties InstData::CreateCalc( QStringList rawdata, InstData::Properties rawproperties )
{
//find the right QString
    QRegExp qrset( "*SET*" );
    qrset.setPatternSyntax( QRegExp::Wildcard );
    QRegExp qrmeasured( "*Measured*" );
    qrmeasured.setPatternSyntax( QRegExp::Wildcard );

    int setindex = rawdata.indexOf( qrset );
    int measuredindex = rawdata.indexOf( qrmeasured );

//check if velocity or distance calculated
    QRegExp qrcalctest("*SET P-DISTANCE:*");
    qrcalctest.setPatternSyntax( QRegExp::Wildcard );
    int calctestindex = rawdata.indexOf( qrcalctest );
    rawproperties.PropCalc = calctestindex == -1 ? Calc::Velocity : Calc::Distance ;

//fill the distance velocity doubles

    QString setstring = rawdata.at( setindex );
    QString measuredstring = rawdata.at ( measuredindex );

    int numstart = setstring.indexOf(":");
    setstring = setstring.right( numstart - 2 );
    for(int i = 0; i < setstring.size() ; ++i ){
        if( setstring.at(i).isLetter() == true ){
            setstring = setstring.left( i );
            break;
        }
    }

    numstart = measuredstring.indexOf(":");
    measuredstring = measuredstring.right( numstart - 2 );
    for(int i = 0; i < measuredstring.size() ; ++i ){
        if( measuredstring.at(i).isLetter() == true ){
            measuredstring = measuredstring.left( i );
            break;
        }
    }

    bool ok = false;

    if( rawproperties.PropCalc == Calc::Velocity ){
        rawproperties.PropDistance = setstring.toDouble( &ok );
        rawproperties.PropVelocity = measuredstring.toDouble( &ok );
    }else{
        rawproperties.PropVelocity = setstring.toDouble( &ok );
        rawproperties.PropDistance = measuredstring.toDouble( &ok );
    }

    return( rawproperties );
}

/******************************************************************************

  Function: CreateCaptureRate

  Description:
  ============
  Used with CreateDateTime QString

******************************************************************************/
InstData::CaptureRate InstData::CreateCaptureRate( QStringList rawproperties )
{
    const QString rate_250khz = "*250 kHz*";
    const QString rate_500khz = "*500 kHz*";
    const QString rate_1000khz = "*1.0 mHz*";
    const QString rate_2000khz = "*2.0 mHz*";

    InstData::CaptureRate return_property = CaptureRate::RATE_500KHZ;

    QRegExp capturerate(rate_2000khz);
    capturerate.setPatternSyntax( QRegExp::Wildcard);

    int index = rawproperties.indexOf( capturerate );

    if( index != -1 ){ //determine the rate
        return_property = CaptureRate::RATE_2000KHZ;
    }else{
        capturerate.setPattern( rate_1000khz );
        index = rawproperties.indexOf( capturerate );
        if( index != -1 ){
            return_property = CaptureRate::RATE_1000KHZ;
        }else{
            capturerate.setPattern( rate_500khz );
            index = rawproperties.indexOf( capturerate );
            if( index != -1 ){
                return_property = CaptureRate::RATE_500KHZ;
            }else{
                capturerate.setPattern( rate_250khz );
                index = rawproperties.indexOf( capturerate );
                if( index != -1 ){
                    return_property = CaptureRate::RATE_250KHZ;
                }
            }
        }
    }

    return( return_property );
}

/******************************************************************************

  Function: CreateDateTime

  Description:
  ============
  Used with CreateDateTime QString

******************************************************************************/
QDateTime InstData::CreateDateTime( QString rawtest )
{
    QDateTime testtime;
    QString strdata;
    QString format = "MM/dd/yy hh:mm:ss AP";
    const int headerlength = 19;
    const int convertonehundred = 100;

    int pos = rawtest.indexOf( "Test Date/Time:" );
    int rawtestlength = rawtest.length();
    int right = rawtestlength - pos - headerlength;

    strdata = rawtest.right( right );
    testtime = QDateTime::fromString( strdata, format );
    testtime = testtime.addYears( convertonehundred ); //convert to 2000's

    return( testtime );
}

/******************************************************************************

  Function: CreateProperties

  Description:
  ============
  Used with CreateTest QString

******************************************************************************/
InstData::Properties InstData::CreateProperties( QStringList rawproperties )
{
    Properties return_property;
    int index;
    QString workingstring;

    return_property.PropAmpGain = AmpGain::Gain_1;
    return_property.PropCalc = Calc::Velocity;
    return_property.PropCaptureRate = CaptureRate::RATE_1000KHZ;
    return_property.PropCycleTime = 5;
    return_property.PropDataSave = true;
    return_property.PropDensity = 200;
    return_property.PropEMethod = EMethod::ArbMu;
    return_property.PropPicSave = true;
    return_property.PropDistance = 500;
    return_property.PropVelocity = 500;
    return_property.PropPulseRate = Pulse::PulsePerSeq_3;
    return_property.PropWave = PWave;
    return_property.PropUnits = Units::USC;
    return_property.PropVoltage = Voltage::Low;

//Determine PropCalc Set & PropPTravelDistance/Velocity Metric/USC
    QRegExp calc("SET P-DISTANCE*");
    calc.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf( calc );
    return_property.PropCalc = index == -1? Calc::Velocity : Calc::Distance;

//Determine Metric
    QRegExp metric("*feet/second*");
    metric.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf( metric );
    return_property.PropUnits = index == -1? Units::Metric : Units::USC;

//Determine Capture Rate
    return_property.PropCaptureRate = CreateCaptureRate( rawproperties );

//Detemine  Gain
    return_property.PropAmpGain = CreateAmpGain( rawproperties );

//Determine Density

//Wave Type
    QRegExp wavetype("*P-*");
    wavetype.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf(wavetype);
    return_property.PropWave = index == -1? Wave::SWave : Wave::PWave;
//EMethod

//Mu?


    return( return_property );
}

/******************************************************************************

  Function: CreateTest

  Description:
  ============
  Used with AddTest QString

******************************************************************************/
InstData::Test InstData::CreateTest(QStringList rawtest)
{
    Test return_test;
    QStringList::const_iterator rawtestiterator;
    QString workingstring;
    int index = -1;
    QRegExp test("*Test:*");
    test.setPatternSyntax( QRegExp::Wildcard);
    QRegExp testdatetime("*Test Date/Time:*");
    testdatetime.setPatternSyntax( QRegExp::Wildcard);
    QRegExp transittime("*Transit Time:*");
    transittime.setPatternSyntax( QRegExp::Wildcard);

    index = rawtest.indexOf(test);  //regular expression
    workingstring = rawtest.at( index );
    return_test.TestNumber = CreateTestNumber( workingstring );

    index = rawtest.indexOf(testdatetime);  //regular expression
    workingstring = rawtest.at( index );
    return_test.TestTime = CreateDateTime( workingstring ); //working 8/20/18

    return_test.TestProp = CreateProperties( rawtest );

    return_test.TransitTime = CreateTransitTime( rawtest );

    return_test.TestProp = CreateCalc( rawtest, return_test.TestProp );

    return( return_test );
}

/******************************************************************************

  Function: CreateTestNumber

  Description:
  ============
  Used with AddTest QString

******************************************************************************/
unsigned InstData::CreateTestNumber( QString rawtest )
{
    QString strdata;
    const unsigned numberwidth = 8;
    unsigned testnumber;
    bool ok = false;

    strdata = rawtest.mid( rawtest.indexOf(':')+1, numberwidth );
    testnumber = strdata.toInt( &ok );
    return( testnumber );
}

/******************************************************************************

  Function: CreateTransitTime

  Description:
  ============
  Used with AddTest QString

******************************************************************************/
double InstData::CreateTransitTime( QStringList rawtest )
{
    double testtransittime = 0.00;
    const double converttomicro = 0.000001;
    QRegExp regexp( "*Transit Time:*" );
    regexp.setPatternSyntax( QRegExp::Wildcard);
    int index = rawtest.indexOf( regexp );

    QString strdata = rawtest.at( index );
    const int numberwidth = 8;
    const int regexpwidth = 13;
    bool ok = false;

    QString returndata = strdata.mid( regexpwidth, numberwidth );
    testtransittime = returndata.toDouble( &ok );

    return( testtransittime * converttomicro );
}

/******************************************************************************

  Function: GetBeginItr
  Description:
  ============
  Access Iterator

******************************************************************************/
std::vector<InstData::Test>::iterator InstData::GetBeginItr()
{
    return( TestData.begin() );
}

/******************************************************************************

  Function: GetEndItr
  Description:
  ============
  Access Iterator

******************************************************************************/
std::vector<InstData::Test>::iterator InstData::GetEndItr()
{
    return( TestData.end() );
}


/******************************************************************************

  Function: GetTest

  Description:
  ============
  Access Function

******************************************************************************/
InstData::Test InstData::GetTest(std::vector<Test>::iterator itr_current)
{
    Test returntest = *itr_current;
    return(returntest);
}
