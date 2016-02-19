// test_functionif.cpp : 定义控制台应用程序的入口点。

#define BOOST_TEST_MAIN

//#include "stdafx.h"
#include <stdlib.h>
#include <boost/test/unit_test.hpp>
#include "functionif.hpp"


// most frequently you implement test cases as a free functions with automatic registration
BOOST_AUTO_TEST_CASE(test1)
{
	//functionif funcif("function")
	//BOOST_CHECK(2 == 1);
	::system("make_function.exe function.xml function");
}

//____________________________________________________________________________//

// each test file may contain any number of test cases; each test case has to have unique name
BOOST_AUTO_TEST_CASE(test2)
{
	functionif funcif("function");
	BOOST_CHECK_EQUAL(funcif.get_sp_name("TID_UserLogin"), "sp_userlogin");
	BOOST_CHECK_EQUAL(funcif.get_sp_name("TID_UserLogout"), "sp_userlogout");
	//int i = 0;

	//// reports 'error in "test2": check i == 2 failed [0 != 2]'
	//BOOST_CHECK_EQUAL(i, 2);

	//BOOST_CHECK_EQUAL(i, 0);
}

BOOST_AUTO_TEST_CASE(test3)
{
	functionif funcif("function");
	std::vector<std::string> v;
	v = funcif.get_input_items("TID_UserLogin");
	BOOST_CHECK_EQUAL(v[0], "ReqUserLoginField.DataFlowFlag");
	BOOST_CHECK_EQUAL(v[1], "ReqUserLoginField.UserId");
	BOOST_CHECK_EQUAL(v[2], "ReqUserLoginField.Password");

	v = funcif.get_input_items("TID_UserLogout");
	BOOST_CHECK_EQUAL(v[0], "ReqUserLoginField.DataFlowFlag");
	BOOST_CHECK_EQUAL(v[2], "ReqUserLogoutField.ParticipantId");
	BOOST_CHECK_EQUAL(v[1], "ReqUserLoginField.UserId");
}

BOOST_AUTO_TEST_CASE(test4)
{
	functionif funcif("function");
	std::vector<std::string> v;
	v = funcif.get_output_items("TID_UserLogin");
	BOOST_CHECK_EQUAL(v[0], "ErrorField.ErrorCode");
	BOOST_CHECK_EQUAL(v[1], "ErrorField.ErrorText");
	BOOST_CHECK_EQUAL(v[2], "ErrorField.TimeStamp");

	v = funcif.get_output_items("TID_UserLogout");
	BOOST_CHECK_EQUAL(v[0], "ErrorField.ErrorCode");
	BOOST_CHECK_EQUAL(v[1], "ErrorField.ErrorText");
	BOOST_CHECK_EQUAL(v[2], "ErrorField.TimeStamp");
}

BOOST_AUTO_TEST_CASE(test5)
{
	functionif funcif("function");
	std::vector<std::string> v;
	v = funcif.get_result_items("TID_UserLogin");
	BOOST_CHECK_EQUAL(v[0], "RspUserLoginField.DataFlowFlag");
	BOOST_CHECK_EQUAL(v[1], "RspUserLoginField.ParticipantId");

	v = funcif.get_result_items("TID_UserLogout");
	BOOST_CHECK_EQUAL(v[0], "RspUserLoginField.DataFlowFlag");
	BOOST_CHECK_EQUAL(v[1], "RspUserLoginField.ParticipantId");
}