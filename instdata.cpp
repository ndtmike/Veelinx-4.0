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

  Function: CreateDateTime

  Description:
  ============
  Used with CreateDateTime QString

******************************************************************************/
QDateTime InstData::CreateDateTime( QString rawtest )
{
    QDateTime testtime;
    QString strdata;
    QString format = "mm/dd/yy hh:mm:ss AP";
    QRegExp testdatetime("*Test Date/Time:*");
    testdatetime.setPatternSyntax( QRegExp::Wildcard);

    strdata = rawtest.right( rawtest.indexOf(testdatetime )+15 );
    testtime = QDateTime::fromString( strdata, format );

    return(testtime);
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
    unsigned index;

    return_property.PropAmpGain = AmpGain::Gain_1;
    return_property.PropCalc = Calc::Velocity;
    return_property.PropCaptureRate = CaptureRate::RATE_1000KHZ;
    return_property.PropCycleTime = 5;
    return_property.PropDataSave = true;
    return_property.PropDensity = 200;
    return_property.PropEMethod = EMethod::ArbMu;
    return_property.PropPicSave = true;
    return_property.PropPTravelDistance = 500;
    return_property.PropPTravelVelocity = 500;
    return_property.PropPulseRate = Pulse::PulsePerSeq_3;
    return_property.PropWave = PWave;
    return_property.PropUnits = Units::USC;
    return_property.PropVoltage = Voltage::Low;

//Determine PropCalc Set & PropPTravelDistance/Velocity Metric/USC
    QRegExp calc("*SET P-Distance*");
    calc.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf(calc);
    return_property.PropCalc = index == -1? Calc::Velocity : Calc::Distance;

    QRegExp metric("*feet/second*");
    metric.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf(metric);
    return_property.PropUnits = index == -1? Units::Metric : Units::USC;

//Determine Capture Rate

//Detemine  Gain

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
    return_test.TestTime = CreateDateTime( workingstring );

    index = rawtest.indexOf(testdatetime);  //regular expression
    workingstring = rawtest.at( index );
    return_test.TransitTime = CreateTransitTime( workingstring );

    return_test.TestProp = CreateProperties( rawtest );

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
double InstData::CreateTransitTime( QString rawtest )
{
    double testtransittime = 0.00;
    QString strdata;
    const unsigned numberwidth = 8;
    bool ok = false;

    strdata = rawtest.mid( rawtest.indexOf(':')+1, numberwidth );
    testtransittime = strdata.toDouble( &ok );
    return( testtransittime );
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
