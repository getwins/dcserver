// test_ftdc_variable.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <memory>
#include "ftdc_variable.hpp"
	
using namespace std;

BOOST_AUTO_TEST_SUITE(ftd_item_value_test)

BOOST_AUTO_TEST_CASE( char_value )
{
	itemvalue_ptr val(new ftd_item_char_value("1"));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 1);
	//
	char c;
	memcpy(&c, val->data(), val->size());
	BOOST_CHECK_EQUAL(c, '1');
 }

BOOST_AUTO_TEST_CASE(float_value)
{
	itemvalue_ptr val(new ftd_item_float_value("6.6", 3, 1));
	string data;
	BOOST_CHECK_EQUAL(val->str(), "6.6");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "6.6");

	val.reset(new ftd_item_float_value("6.632", 3, 1));
	BOOST_CHECK_EQUAL(val->str(), "6.632");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "6.6");

	val.reset(new ftd_item_float_value("-6.6", 4, 1));
	BOOST_CHECK_EQUAL(val->str(), "-6.6");
	BOOST_CHECK_EQUAL(val->size(), 4);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-6.6");

	val.reset(new ftd_item_float_value("006.61", 6, 2));
	BOOST_CHECK_EQUAL(val->str(), "6.61");
	BOOST_CHECK_EQUAL(val->size(), 6);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "006.61");

	val.reset(new ftd_item_float_value("006.61", 8, 2));
	BOOST_CHECK_EQUAL(val->str(), "6.61");
	BOOST_CHECK_EQUAL(val->size(), 8);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "00006.61");

	val.reset(new ftd_item_float_value("-006.61", 7, 2));
	BOOST_CHECK_EQUAL(val->str(), "-6.61");
	BOOST_CHECK_EQUAL(val->size(), 7);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-006.61");

}

BOOST_AUTO_TEST_CASE(int_value)
{
	itemvalue_ptr val(new ftd_item_int_value(5));
	int i;

	BOOST_CHECK_EQUAL(val->str(), "5");
	BOOST_CHECK_EQUAL(val->size(), 4);
	memcpy(&i, val->data(), val->size());
	BOOST_CHECK_EQUAL(i, 5);

	val.reset(new ftd_item_int_value("520"));
	BOOST_CHECK_EQUAL(val->str(), "520");
	BOOST_CHECK_EQUAL(val->size(), 4);
	memcpy(&i, val->data(), val->size());
	BOOST_CHECK_EQUAL(i, 520);
}

BOOST_AUTO_TEST_CASE(number_value)
{
	itemvalue_ptr val;
	string data;

	val.reset(new ftd_item_number_value("520", 3));
	BOOST_CHECK_EQUAL(val->str(), "520");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "520");

	val.reset(new ftd_item_number_value("520", 5));
	BOOST_CHECK_EQUAL(val->str(), "520");
	BOOST_CHECK_EQUAL(val->size(), 5);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "00520");

	val.reset(new ftd_item_number_value("520", 2));
	BOOST_CHECK_EQUAL(val->str(), "52");
	BOOST_CHECK_EQUAL(val->size(), 2);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "52");

	val.reset(new ftd_item_number_value("-520", 4));
	BOOST_CHECK_EQUAL(val->str(), "-520");
	BOOST_CHECK_EQUAL(val->size(), 4);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-520");

	val.reset(new ftd_item_number_value("-520", 6));
	BOOST_CHECK_EQUAL(val->str(), "-520");
	BOOST_CHECK_EQUAL(val->size(), 6);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-00520");

	val.reset(new ftd_item_number_value("-520", 3));
	BOOST_CHECK_EQUAL(val->str(), "-52");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-52");

	val.reset(new ftd_item_number_value("-00520", 6));
	BOOST_CHECK_EQUAL(val->str(), "-520");
	BOOST_CHECK_EQUAL(val->size(), 6);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "-00520");

}

BOOST_AUTO_TEST_CASE(string_value)
{
	itemvalue_ptr val;
	string data;

	val.reset(new ftd_item_string_value("test", 4));
	BOOST_CHECK_EQUAL(val->str(), "\'test\'");
	BOOST_CHECK_EQUAL(val->size(), 4);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "test");

	val.reset(new ftd_item_string_value("test", 5));
	BOOST_CHECK_EQUAL(val->str(), "\'test\'");
	BOOST_CHECK_EQUAL(val->size(), 5);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "test ");

	val.reset(new ftd_item_string_value("test", 3));
	BOOST_CHECK_EQUAL(val->str(), "\'tes\'");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "tes");

}

BOOST_AUTO_TEST_CASE(word_value)
{
	itemvalue_ptr val;
	unsigned short us;

	val.reset(new ftd_item_word_value(1));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 2);
	memcpy(&us, val->data(), val->size());
	BOOST_CHECK_EQUAL(us, 1);

	val.reset(new ftd_item_word_value("1"));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 2);
	memcpy(&us, val->data(), val->size());
	BOOST_CHECK_EQUAL(us, 1);

}

BOOST_AUTO_TEST_CASE(value_from_data)
{
	itemvalue_ptr val;
	
	char c = '1';
	val.reset(create_ftd_item_value_from_data(ftd_char_type, &c, 1, 0));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 1);
	memcpy(&c, val->data(), val->size());
	BOOST_CHECK_EQUAL(c, '1');

	char *f = "6.6";
	val.reset(create_ftd_item_value_from_data(ftd_float_type, f, 3, 1));
	string data;
	BOOST_CHECK_EQUAL(val->str(), "6.6");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "6.6");

	int i = 5;
	val.reset(create_ftd_item_value_from_data(ftd_int_type, &i, 4, 0));
	BOOST_CHECK_EQUAL(val->str(), "5");
	BOOST_CHECK_EQUAL(val->size(), 4);
	memcpy(&i, val->data(), val->size());
	BOOST_CHECK_EQUAL(i, 5);

	val.reset(create_ftd_item_value_from_data(ftd_number_type, "520", 3, 0));
	BOOST_CHECK_EQUAL(val->str(), "520");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "520");

	val.reset(create_ftd_item_value_from_data(ftd_string_type, "test", 4, 0));
	BOOST_CHECK_EQUAL(val->str(), "\'test\'");
	BOOST_CHECK_EQUAL(val->size(), 4);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "test");

	unsigned short us = 1;
	val.reset(create_ftd_item_value_from_data(ftd_word_type, &us, 2, 0));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 2);
	memcpy(&us, val->data(), val->size());
	BOOST_CHECK_EQUAL(us, 1);
}

BOOST_AUTO_TEST_CASE(value_from_str)
{
	itemvalue_ptr val;

	char c;
	val.reset(create_ftd_item_value_from_str(ftd_char_type, "1", 1, 0));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 1);
	memcpy(&c, val->data(), val->size());
	BOOST_CHECK_EQUAL(c, '1');

	char *f = "6.6";
	val.reset(create_ftd_item_value_from_str(ftd_float_type, f, 3, 1));
	string data;
	BOOST_CHECK_EQUAL(val->str(), "6.6");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "6.6");

	int i;
	val.reset(create_ftd_item_value_from_str(ftd_int_type, "5", 4, 0));
	BOOST_CHECK_EQUAL(val->str(), "5");
	BOOST_CHECK_EQUAL(val->size(), 4);
	memcpy(&i, val->data(), val->size());
	BOOST_CHECK_EQUAL(i, 5);

	val.reset(create_ftd_item_value_from_str(ftd_number_type, "520", 3, 0));
	BOOST_CHECK_EQUAL(val->str(), "520");
	BOOST_CHECK_EQUAL(val->size(), 3);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "520");

	val.reset(create_ftd_item_value_from_str(ftd_string_type, "test", 4, 0));
	BOOST_CHECK_EQUAL(val->str(), "\'test\'");
	BOOST_CHECK_EQUAL(val->size(), 4);
	data.assign(reinterpret_cast<const char*>(val->data()), val->size());
	BOOST_CHECK_EQUAL(data, "test");

	unsigned short us;
	val.reset(create_ftd_item_value_from_str(ftd_word_type, "1", 2, 0));
	BOOST_CHECK_EQUAL(val->str(), "1");
	BOOST_CHECK_EQUAL(val->size(), 2);
	memcpy(&us, val->data(), val->size());
	BOOST_CHECK_EQUAL(us, 1);
}


BOOST_AUTO_TEST_SUITE( ftdc_path_test)

BOOST_AUTO_TEST_CASE(path_test1)
{
	ftdc_path path("ReqUserLoginField.DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_name(), "ReqUserLoginField");
	BOOST_CHECK_EQUAL(path.item_name(), "DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_idx(), 0);
	BOOST_CHECK_EQUAL(path.invalid(), true);
	BOOST_CHECK_EQUAL(path.str(), "ReqUserLoginField.DataFlowFlag");
}

BOOST_AUTO_TEST_CASE(path_test2)
{
	ftdc_path path("ReqUserLoginField[0].DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_name(), "ReqUserLoginField");
	BOOST_CHECK_EQUAL(path.item_name(), "DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_idx(), 0);
	BOOST_CHECK_EQUAL(path.invalid(), true);
	BOOST_CHECK_EQUAL(path.str(), "ReqUserLoginField[0].DataFlowFlag");
}

BOOST_AUTO_TEST_CASE(path_test3)
{
	ftdc_path path("ReqUserLoginField[0]");
	BOOST_CHECK_EQUAL(path.fid_name(), "");
	BOOST_CHECK_EQUAL(path.item_name(), "");
	BOOST_CHECK_EQUAL(path.fid_idx(), 0);
	BOOST_CHECK_EQUAL(path.invalid(), false);
	BOOST_CHECK_EQUAL(path.str(), "ReqUserLoginField[0]");
}

BOOST_AUTO_TEST_CASE(path_test4)
{
	ftdc_path path("ReqUserLoginField", 0, "DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_name(), "ReqUserLoginField");
	BOOST_CHECK_EQUAL(path.item_name(), "DataFlowFlag");
	BOOST_CHECK_EQUAL(path.fid_idx(), 0);
	BOOST_CHECK_EQUAL(path.invalid(), true);
	BOOST_CHECK_EQUAL(path.str(), "ReqUserLoginField[0].DataFlowFlag");
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(ftdc_variable_test)

BOOST_AUTO_TEST_CASE(variable1)
{
	ftdc_variable var;
	string l, r;
	BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 0, "DataFlowFlag", itemvalue_ptr(new ftd_item_char_value("1"))));
	l = var.get_field_item("ReqUserLoginField", 0, "DataFlowFlag")->str();
	r = "1";
	BOOST_CHECK_EQUAL(l, r);
	ftdc_path path("ReqUserLoginField.DataFlowFlag");
	l = var.get_field_item(path)->str();
	BOOST_CHECK_EQUAL(l, r);

	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 0, "Data"), itemvalue_ptr());

	BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 1, "ParticipantId", itemvalue_ptr(new ftd_item_string_value("666", 3))));
	l = var.get_field_item("ReqUserLoginField", 1, "ParticipantId")->str();
	r = "\'666\'";
	BOOST_CHECK_EQUAL(l, r);
	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 1, "Part"), itemvalue_ptr());
}

BOOST_AUTO_TEST_SUITE_END()


//BOOST_AUTO_TEST_CASE(test1)
//{
//	ftdc_variable var;
//	string l, r;
//	BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 0, "DataFlowFlag",itemvalue_ptr(new ftd_item_char_value("1"))));
//	l = var.get_field_item("ReqUserLoginField", 0, "DataFlowFlag")->str();
//	r = "1";
//	BOOST_CHECK_EQUAL(l, r);
//	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 0, "Data"), itemvalue_ptr());
//
//	//BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 1, "ParticipantId", itemvalue_ptr(new ftd_item_string_value("666", 3))));
//	//l = var.get_field_item("ReqUserLoginField", 1, "ParticipantId")->str();
//	//r = "666\'";
//	//BOOST_CHECK_EQUAL(l, r);
//	//BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 1, "Part"), itemvalue_ptr());
//}

//BOOST_AUTO_TEST_CASE(test2)
//{
//	ftdc_variable var;
//	BOOST_CHECK_NO_THROW(var.set_field_item(string("ReqUserLoginField.DataFlowFlag"), "1"));
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField.DataFlowFlag")), "1");
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField.Data")), "");
//
//	BOOST_CHECK_NO_THROW(var.set_field_item(string("ReqUserLoginField[1].ParticipantId"), "1"));
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField[1].ParticipantId")), "1");
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField[1].Part")), "");
//}
//
//BOOST_AUTO_TEST_CASE(test3)
//{
//	ftdc_variable var;
//	BOOST_CHECK_NO_THROW(var.set_field_item(string("ReqUserLoginField.DataFlowFlag"), "1"));
//	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 0, "DataFlowFlag"), "1");
//	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 0, "Data"), "");
//
//	BOOST_CHECK_NO_THROW(var.set_field_item(string("ReqUserLoginField[1].DataFlowFlag"), "1"));
//	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 1, "DataFlowFlag"), "1");
//	BOOST_CHECK_EQUAL(var.get_field_item("ReqUserLoginField", 1, "Data"), "");
//
//	BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 0, "ParticipantId", "666"));
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField.ParticipantId")), "666");
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField.Part")), "");
//
//	BOOST_CHECK_NO_THROW(var.set_field_item("ReqUserLoginField", 1, "ParticipantId", "888"));
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField[1].ParticipantId")), "888");
//	BOOST_CHECK_EQUAL(var.get_field_item(string("ReqUserLoginField[1].Part")), "");
//
//}
//int main()
//{
//    return 0;
//}

