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
#include <QtDebug>

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
void InstData::AddTest( QString rawdata )
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
            TestData.append( CreateTest( working ));
        }
        working.clear();
    }
    rawdata.clear();
    rawdatalist.clear();
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
    }

    regexp.setPattern( gain_250 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
            return_property = AmpGain::Gain_250;
    }

    regexp.setPattern( gain_100 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
                return_property = AmpGain::Gain_100;
    }

    regexp.setPattern( gain_50 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
        return_property = AmpGain::Gain_50;
    }

    regexp.setPattern( gain_25 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
        return_property = AmpGain::Gain_25;
    }

    regexp.setPattern( gain_10 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
        return_property = AmpGain::Gain_10;
    }

    regexp.setPattern( gain_5 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
        return_property = AmpGain::Gain_5;
    }

    regexp.setPattern( gain_1 );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
        return_property = AmpGain::Gain_1;
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

    int numstart = setstring.indexOf(":")+1;
    int setstringright = setstring.size() - numstart;
    setstring = setstring.right( setstringright );
    for(int i = 0; i < setstring.size() ; ++i ){
        if( setstring.at(i).isLetter() == true ){
            setstring = setstring.left( i );
            break;
        }
    }

    numstart = measuredstring.indexOf(":")+1;
    int measurestringright = measuredstring.size() - numstart;
    measuredstring = measuredstring.right( measurestringright );
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
    }

    capturerate.setPattern( rate_1000khz );
    index = rawproperties.indexOf( capturerate );
    if( index != -1 ){
        return_property = CaptureRate::RATE_1000KHZ;
    }

    capturerate.setPattern( rate_500khz );
    index = rawproperties.indexOf( capturerate );
    if( index != -1 ){
        return_property = CaptureRate::RATE_500KHZ;
    }

    capturerate.setPattern( rate_250khz );
    index = rawproperties.indexOf( capturerate );
    if( index != -1 ){
        return_property = CaptureRate::RATE_250KHZ;
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

  Function: CreateDensity

  Description:
  ============


******************************************************************************/
double InstData::CreateDensity( QStringList rawdata )
{
    double result = 0.0;
    QRegExp qrdensity( "*Density:*" ); //find the right QString
    const QString density( "Density:");
    qrdensity.setPatternSyntax( QRegExp::Wildcard );
    int densityindex = rawdata.indexOf( qrdensity );

    QString densitystring = rawdata.at( densityindex );//fill the distance velocity doubles
    int numstart = densitystring.indexOf( density );
    int rightsize = densitystring.size() - numstart - density.size();
    densitystring = densitystring.right( rightsize );
    for(int i = 0; i < densitystring.size() ; ++i ){
        if( densitystring.at(i).isLetter() == true ){
            densitystring = densitystring.left( i );
            break;
        }
    }

    bool ok = false;
    result = densitystring.toDouble( &ok );

    return( result );
}

/******************************************************************************

  Function: CreateE

  Description:
  ============


******************************************************************************/
InstData::Properties InstData::CreateE( QStringList* rawdata, Properties* rawproperties )
{
    QRegExp qre( "*Young's modulus:*" ); //find the right QString for E!!
    const QString e( "Young's modulus:");
    qre.setPatternSyntax( QRegExp::Wildcard );
    int eindex = rawdata->indexOf( qre );

    QString estring = rawdata->at( eindex );//fill the distance velocity doubles
    int numstart = estring.indexOf( e );
    int rightsize = estring.size() - numstart - e.size();
    estring = estring.right( rightsize );
    for(int i = 0; i < estring.size() ; ++i ){
        if( estring.at(i).isLetter() == true ){
            estring = estring.left( i );
            break;
        }
    }

    bool ok = false;
    rawproperties->PropE = estring.toDouble( &ok );

    QRegExp qrep( "*Young's modulus:*" ); //find the right QString for E!!
    const QString p( "Poisson's ratio:");
    qrep.setPatternSyntax( QRegExp::Wildcard );
    int pindex = rawdata->indexOf( qrep );

    QString pstring = rawdata->at( pindex );//fill the distance velocity doubles
    QString etypestring( "SIMPLE");
    numstart = pstring.indexOf( p );
    rightsize = pstring.size() - numstart - p.size();
    QString pvalstring = pstring.right( rightsize );
    for(int i = 0; i < pvalstring.size() ; ++i ){
        if( pvalstring.at(i).isLetter() == true ){
            pstring = pvalstring.left( i );
            etypestring = pvalstring.right(pvalstring.size() - i );
            break;
        }
    }
    ok = false;
    rawproperties->PropMu = pstring.toDouble( &ok );

    rawproperties->PropEMethod = InstData::SimpleE;
    if( etypestring == "ARBITRARY" )
        rawproperties->PropEMethod = InstData::ArbMu;
    if( etypestring == "DERIVED" )
        rawproperties->PropEMethod = InstData::DerivedMu;

    return( *rawproperties );
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
    return_property.PropDensity = CreateDensity( rawproperties );

//Determine Wave Type
    return_property.PropWave = CreateWaveType( rawproperties );

//Wave Type
    QRegExp wavetype("*P-*");
    wavetype.setPatternSyntax( QRegExp::Wildcard);
    index = rawproperties.indexOf(wavetype);
    return_property.PropWave = index == -1? Wave::SWave : Wave::PWave;

//EMethod & Mu
    return_property = CreateE( &rawproperties, &return_property );

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
    index = rawtest.indexOf(test);
    workingstring = rawtest.at( index );
    return_test.TestNumber = CreateTestNumber( workingstring );//test Number

    QRegExp testdatetime("*Test Date/Time:*");
    testdatetime.setPatternSyntax( QRegExp::Wildcard);
    index = rawtest.indexOf(testdatetime);  //regular expression
    workingstring = rawtest.at( index );
    return_test.TestTime = CreateDateTime( workingstring ); //date and time

    return_test.TestProp = CreateProperties( rawtest ); //returns properties in struct

    QRegExp transittime("*Transit Time:*");
    transittime.setPatternSyntax( QRegExp::Wildcard);
    return_test.TransitTime = CreateTransitTime( rawtest );// returns transit time

    return_test.TestProp = CreateCalc( rawtest, return_test.TestProp ); //returns values for velocity and distance

    bool numtest = false;
    long addbufffer = 0;
    for( rawtestiterator = rawtest.begin(); rawtestiterator != rawtest.end(); ++rawtestiterator ){
        addbufffer = (*rawtestiterator).toInt(&numtest);
        if( numtest == true ) {return_test.ADC.append(addbufffer);}
    }

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

  Function: CreateWaveType

  Description:
  ============

******************************************************************************/
InstData::Wave InstData::CreateWaveType( QStringList rawproperties )
{
    const QString pwave = "WAVE TYPE: 'P'";
    const QString swave = "WAVE TYPE: 'S'";

    InstData::Wave return_property = PWave;

    QRegExp regexp( pwave );
    regexp.setPatternSyntax( QRegExp::Wildcard);
    int index = rawproperties.indexOf( regexp );
    if( index != -1 ){ //determine the rate
        return_property = PWave;
    }

    regexp.setPattern( swave );
    index = rawproperties.indexOf( regexp );
    if( index != -1 ){
            return_property = SWave;
    }

    return( return_property );
}

/******************************************************************************

  Function: GetADC
  Description:
  ============
  Access Iterator

******************************************************************************/
QStringList InstData::GetADC( Test* workingtest )
{
    QStringList returnlist;
    QString adcstring;

    for( QVector<long>::iterator i = workingtest->ADC.begin(); i != workingtest->ADC.end(); ++i ){
        adcstring = QString::number( (*i), 10 );
        returnlist.append(adcstring);
    }
    return( returnlist );
}

/******************************************************************************

  Function: GetAmpGain
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetAmpGain( Test* workingtest )
{
    const QString gain = "   Gain: ";
    QString returnvariable = gain + "50";

    returnvariable = gain
            + QString::number( int( workingtest->TestProp.PropAmpGain ), 10);

    return( returnvariable );
}

/******************************************************************************

  Function: GetCalc
  Description:
  ============
  Access Iterator

******************************************************************************/
QStringList InstData::GetCalc( Test* workingtest )
{
    const QString inches = " inches";
    const QString feetpersec = " feet per second";
    const QString mm = " mm";
    const QString meterspersecond = " meters per second";

    QString measured = "Measured: ";
    QString set = "Set: ";
    QStringList returnvariable;
    Calc workingvariable = workingtest->TestProp.PropCalc;
    const int precision = 4;
    bool usc = workingtest->TestProp.PropUnits == USC ? true : false;

    if( workingvariable == Velocity ){
        measured.append( " Distance ");
        measured.append( QString::number( workingtest->TestProp.PropDistance,'g', precision ));
        set.append( " Velocity ");
        set.append(QString::number( workingtest->TestProp.PropVelocity,'g', precision ));
        if( usc == true){
            measured.append( inches);
            set.append( feetpersec );
        }else{
            measured.append( mm );
            set.append( meterspersecond );
        }
    }else{
        set.append( " Distance ");
        set.append( QString::number( workingtest->TestProp.PropVelocity,'g', precision ));
        measured.append( " Velocity " );
        measured.append(QString::number( workingtest->TestProp.PropDistance,'g', precision ));
        if( usc == true){
            measured.append( feetpersec );
            set.append( inches );
        }else{
            measured.append( meterspersecond );
            set.append( mm );
        }
    }

    returnvariable.append( set );
    returnvariable.append( measured );

    return( returnvariable );
}

/******************************************************************************

  Function: GetCaptureRate
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetCaptureRate( Test* workingtest )
{
    const QString rate250 = " 250 kHz";
    const QString rate500 = " 500 kHz";
    const QString rate1000 = " 1 MHz";
    const QString rate2000 = " 2 MHz";
    const QString rate = " Rate: ";

    QString returnvariable = rate + rate500;
    CaptureRate workingvariable = workingtest->TestProp.PropCaptureRate;
    switch ( workingvariable ) {
    case RATE_250KHZ:
        returnvariable = rate + rate250;
        break;
    case RATE_500KHZ:
        returnvariable = rate + rate500;
        break;
    case RATE_1000KHZ:
        returnvariable = rate + rate1000;
        break;
    case RATE_2000KHZ:
        returnvariable = rate + rate2000;
        break;
    }

    return( returnvariable );
}

/******************************************************************************

  Function: GetDateTime
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetDateTime( Test* workingtest )
{
    QString returnvariable = "01/01/2001 00:00:01";
    QDateTime workingvariable = workingtest->TestTime;
    QString dateformat = "MM/dd/yyy hh:mm:ss AP";

    returnvariable = QString::QString( " Date and Time: " )
            + workingvariable.toString( dateformat );

    return( returnvariable );
}

/******************************************************************************

  Function: GetDensity
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetDensity( Test* workingtest )
{
    QString returnvariable = "   Density: 100";
    const int prec = 4;
    double workingvariable = workingtest->TestProp.PropDensity;

    returnvariable = "   Density: "
            + QString::number( workingvariable,'g', prec );

    if( workingtest->TestProp.PropUnits == USC){
        returnvariable.append( " pounds per cubic foot");
    }else{
        returnvariable.append( " Kg per cubic meter");
    }

    return( returnvariable );
}

/******************************************************************************

  Function: GetE
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetE( Test* workingtest )
{
    QString returnvariable = "Young's Modulus: ";
    double workingvariable = workingtest->TestProp.PropE;
    const int pres = 4;

    returnvariable.append( QString::number( workingvariable,'g', pres ));

    if( workingtest->TestProp.PropUnits == USC){
        returnvariable.append( " psi ");
    }else{
        returnvariable.append( " MPA ");
    }

    return( returnvariable );
}

/******************************************************************************

  Function: GetMu
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetMu( Test* workingtest )
{
    const QString pratio = "   Poisson's ratio: ";
    QString returnvariable = "Simple";
    double workingvariable = workingtest->TestProp.PropMu;
    const int pres = 4;
    EMethod emethod = workingtest->TestProp.PropEMethod;

    returnvariable = pratio +
            QString::number( workingvariable, 'g', pres );

    switch( emethod ){
    case ArbMu:
        returnvariable.append(" Arbitrary Mu ");
        break;
    case DerivedMu:
        returnvariable.append(" Derived Mu ");
        break;
    case SimpleE:
        returnvariable.append(" Simple ");
        break;
    }

    return( returnvariable );
}

/******************************************************************************

  Function: GetTestNumber
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetTestNumber( Test* workingtest )
{
    QString returnvariable = "Test Number: ";
    unsigned workingvariable = workingtest->TestNumber;

    returnvariable = returnvariable + QString::number( workingvariable, 10 )
                        + QString::QString(' ');

    return( returnvariable );
}

/******************************************************************************

  Function: GetTranistTime
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetTransitTime( Test* workingtest )
{
    QString returnvariable = "Test Transit Time: ";
    QString secondsstr = " Seconds";
    double workingvariable = workingtest->TransitTime;

    returnvariable = returnvariable + QString::number( workingvariable, 'g', 5 ) + secondsstr;

    return( returnvariable );
}

/******************************************************************************

  Function: GetBeginItr
  Description:
  ============
  Access Iterator

******************************************************************************/
QVector<InstData::Test>::iterator InstData::GetBeginItr()
{
    return( TestData.begin() );
}

/******************************************************************************

  Function: GetEndItr
  Description:
  ============
  Access Iterator

******************************************************************************/
QVector<InstData::Test>::iterator InstData::GetEndItr()
{
    return( TestData.end() );
}


/******************************************************************************

  Function: GetTest

  Description:
  ============
  Access Function

******************************************************************************/
InstData::Test InstData::GetTest(QVector<Test>::iterator itr_current)
{
    Test returntest = *itr_current;
    return(returntest);
}

/******************************************************************************

  Function: GetTest

  Description:
  ============
  Access Function overload to return a QStringList

******************************************************************************/
QStringList InstData::GetTest( unsigned gettestnumber, bool* ok )
{
    QStringList returnlist;
    QString adcstring;
    QString header1 = "James Instruments V-METER MK IV Data";
    QString header2 = "Veelinx 4.0 copyright 2018";
    QString testnumdatetime;
    QString rategaindensity;
    QString emu;

    (*ok) = false;
    QVector<Test>::iterator i;
    for(  i = TestData.begin(); i != TestData.end(); ++i ){
        if( (*i).TestNumber == gettestnumber ){
            (*ok) = true;
            break;
        }
    }

    testnumdatetime = GetTestNumber(i) + GetDateTime( i );
    rategaindensity = GetCaptureRate( i ) + GetAmpGain( i ) + GetDensity( i );
    emu = GetE( i ) + GetMu( i );

    returnlist.append( header1 );
    returnlist.append( header2 );
    returnlist.append( testnumdatetime );
    returnlist.append( GetTransitTime( i ));
    returnlist.append( GetCalc( i ));
    returnlist.append( GetWaveType ( i ));
    returnlist.append( rategaindensity );
    returnlist.append( emu );

    returnlist.append( GetADC( i ));

    return( returnlist );
}

/******************************************************************************

  Function: GetE
  Description:
  ============
  Access Iterator

******************************************************************************/
QString InstData::GetWaveType( Test* workingtest )
{
    const QString wavep = " Wave: 'P'";
    const QString waves = " Wave: 'S'";
    QString returnvariable = workingtest->TestProp.PropWave == PWave ?
                wavep : waves;
    return( returnvariable );
}

/******************************************************************************

  Function: NumTests
  Description:
  ============
  Access Iterator

******************************************************************************/
int InstData::NumTests( void )
{
    int numtests = TestData.size();

    return( numtests );
}
