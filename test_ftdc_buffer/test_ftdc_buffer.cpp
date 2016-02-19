// test_ftdc_buffer.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "ftdc_buffer.hpp"

struct ErrorField
{
	char ErrorCode[10];
	char ErrorText[100];
	char TimeStamp[20];
};

struct ErrorTargetOrderField
{
	char OrderSysId[20];
	char OrderLocalId[15];
};

BOOST_AUTO_TEST_CASE(test1)
{
	ftdc_buffer buffer;
	//ftdc_header_st header = { 0 };
	ftdc_field_st field;
	//buffer.add_field()

}
