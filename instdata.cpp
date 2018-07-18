/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: Inst_Data.cpp
**
** parser.cpp - Veelinx software
** Divides the input stream up into data packets
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2017
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "instdata.h"


InstData::InstData()
{

}
/*
InstData::InstData(DataSet::Test Init_Test)
{
    TestData.push_back( Init_Test );
}
*/
void InstData::AddTest(InstData::Test test)
{
    TestData.push_back( test );
}

InstData::Test InstData::GetTest(std::vector<Test>::iterator itr_current)
{
    Test returntest = *itr_current;
    return(returntest);
}

std::vector<InstData::Test>::iterator InstData::GetBeginItr()
{
    return( TestData.begin() );
}

std::vector<InstData::Test>::iterator InstData::GetEndItr()
{
    return( TestData.end() );
}


