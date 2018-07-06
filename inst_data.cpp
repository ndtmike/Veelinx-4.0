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

#include "inst_data.h"


DataSet::DataSet()
{

}

DataSet::DataSet(DataSet::Test Init_Test)
{
    TestData.push_back( Init_Test );
}

void DataSet::AddTest(DataSet::Test test)
{
    TestData.push_back( test );
}

DataSet::Test DataSet::GetTest(std::vector<Test>::iterator itr_current)
{
    Test returntest = *itr_current;

    return(returntest);
}

std::vector<DataSet::Test>::iterator DataSet::GetBeginItr()
{
    return( TestData.begin() );
}

std::vector<DataSet::Test>::iterator DataSet::GetEndItr()
{
    return( TestData.end() );
}


