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
//    QVector<QStringList> individualtestdatavector;

    QStringList::const_iterator constIterator;
    for( constIterator = rawdatalist.constBegin(); constIterator != rawdatalist.constEnd(); ++constIterator )
    {
        if( (*constIterator) == const_james )
        {
            for(;(*constIterator) != const_end; ++constIterator)
            {
                working << (*constIterator);
            }
//        individualtestdatavector.append( working );
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
    const QString const_end = "Young's";

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

/*    QStringList::const_iterator constIterator;
    for( constIterator = rawdatalist.constBegin(); constIterator != rawdatalist.constEnd(); ++constIterator )
    {
        if( (constIterator-> ) == const_james )
        {
            for(;(*constIterator) != const_end; ++constIterator)
            {
                working << (*constIterator);
            }
//        individualtestdatavector.append( working );
            CreateTest( working );
        }
        working.clear();
    }
*/
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

    workingstring = rawtest.at( rawtest.indexOf("Test:"));
    return_test.TestNumber = CreateTestNumber( workingstring );

    workingstring = rawtest.at( rawtest.indexOf("Test Date/Time:"));
    return_test.TestTime = CreateDateTime( workingstring );

    workingstring = rawtest.at( rawtest.indexOf("Transit Time:" ));
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

}

/******************************************************************************

  Function: CreateTransitTime

  Description:
  ============
  Used with AddTest QString

******************************************************************************/
double InstData::CreateTransitTime( QString rawtest )
{

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
