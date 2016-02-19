// test_ftd_protocalif.cpp : 定义控制台应用程序的入口点。
//
#define BOOST_TEST_MAIN
//#include "stdafx.h"
#include <stdlib.h>
#include <boost/test/unit_test.hpp>
//#include <boost/assign.hpp>

#include "sqlitehelper.hpp"
#include "ftd_protocalif.hpp"

std::shared_ptr<ftd_protocalif> p;

BOOST_AUTO_TEST_CASE(test1)
{
	//std::shared_ptr<ftd_protocalif> p;
	//::system(make_protocal.exe FTD_protocal.xml)
	BOOST_CHECK_NO_THROW(p.reset(new ftd_protocalif("protocal")));
}

BOOST_AUTO_TEST_CASE(test2)
{
	BOOST_CHECK_EQUAL(p->get_fidname(0x0001), "BulletinField");
	BOOST_CHECK_THROW(p->get_fidname(0x0000), sqlite_error);
}

BOOST_AUTO_TEST_CASE(test3)
{
	BOOST_CHECK_EQUAL(p->get_fidvalue("BulletinField"), 0x0001);
	BOOST_CHECK_THROW(p->get_fidvalue("BulletinFiel"), sqlite_error);
}

BOOST_AUTO_TEST_CASE(test4)
{
	BOOST_CHECK_EQUAL(p->get_tidname(0x0001), "TID_Error");
	BOOST_CHECK_THROW(p->get_tidname(0x0000), sqlite_error);
}

BOOST_AUTO_TEST_CASE(test5)
{
	std::vector<ftd_protocalif::packagefieldinfo> v;
	BOOST_CHECK_NO_THROW(v = p->get_package_field_list("TID_Error"));
	BOOST_CHECK_EQUAL(v[0].fidname, "ErrorField");
	BOOST_CHECK_EQUAL(v[0].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[0].minoccur, 1);
	BOOST_CHECK_EQUAL(v[1].fidname, "ErrorTargetSequenceField");
	BOOST_CHECK_EQUAL(v[1].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[1].minoccur, 0);
	BOOST_CHECK_EQUAL(v[2].fidname, "ErrorTargetOrderField");
	BOOST_CHECK_EQUAL(v[2].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[2].minoccur, 0);
	BOOST_CHECK_THROW(p->get_package_field_list("Error"), sqlite_error);

	BOOST_CHECK_NO_THROW(v = p->get_req_package_field_list("TID_OrderAction"));
	BOOST_CHECK_EQUAL(v[0].fidname, "OrderActionField");
	BOOST_CHECK_EQUAL(v[0].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[0].minoccur, 1);
	BOOST_CHECK_EQUAL(v.size(), 1);
	BOOST_CHECK_THROW(p->get_req_package_field_list("TID_OrderActionXXXX"), sqlite_error);


	BOOST_CHECK_NO_THROW(v = p->get_rsp_package_field_list("TID_OrderAction"));
	BOOST_CHECK_EQUAL(v[0].fidname, "OrderActionField");
	BOOST_CHECK_EQUAL(v[0].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[0].minoccur, 1);
	BOOST_CHECK_EQUAL(v[1].fidname, "ErrorField");
	BOOST_CHECK_EQUAL(v[1].maxoccur, 1);
	BOOST_CHECK_EQUAL(v[1].minoccur, 0);
	BOOST_CHECK_EQUAL(v.size(), 2);
	BOOST_CHECK_THROW(p->get_rsp_package_field_list("TID_OrderActionXXXX"), sqlite_error);

	//BOOST_CHECK_EQUAL(p->get_package_field_list, "TID_Error");
	//BOOST_CHECK_THROW(p->get_tidname(0x0001), sqlite_error);
}

BOOST_AUTO_TEST_CASE(test_fieldlist)
{
	std::vector<std::string> v;
	BOOST_CHECK_NO_THROW(v = p->get_fielditem_list("BulletinField"));
	BOOST_CHECK_EQUAL(v.size(), 8);
	BOOST_CHECK_EQUAL(v[0], "NewsType");
	BOOST_CHECK_EQUAL(v[1], "NewsUrgency");
	BOOST_CHECK_EQUAL(v[2], "SendTime");
	BOOST_CHECK_EQUAL(v[3], "Abstract");
	BOOST_CHECK_EQUAL(v[4], "ComeFrom");
	BOOST_CHECK_EQUAL(v[5], "MarketId");
	BOOST_CHECK_EQUAL(v[6], "Content");
	BOOST_CHECK_EQUAL(v[7], "URLLink");
	BOOST_CHECK_THROW(p->get_fielditem_list("BulletinFieldXXXX"), sqlite_error);

}

BOOST_AUTO_TEST_CASE(test_item_info)
{
	ftd_protocalif::itemtypeinfo info;
	BOOST_CHECK_NO_THROW(info = p->get_itemtypeinfo("Abstract"));
	BOOST_CHECK_EQUAL(info.basetype, "FTDStringType<80>");
	BOOST_CHECK_EQUAL(info.length, 80);
	BOOST_CHECK_EQUAL(info.precision, 0);
	BOOST_CHECK_EQUAL(info.enumvals.size(), 0);
	BOOST_CHECK_THROW(p->get_itemtypeinfo("AbstractXXXX"), sqlite_error);

	BOOST_CHECK_NO_THROW(info = p->get_itemtypeinfo("AskPrice"));
	BOOST_CHECK_EQUAL(info.basetype, "FTDFloatType<10,2>");
	BOOST_CHECK_EQUAL(info.length, 10);
	BOOST_CHECK_EQUAL(info.precision, 2);
	BOOST_CHECK_EQUAL(info.enumvals.size(), 0);
	BOOST_CHECK_THROW(p->get_itemtypeinfo("AbstractXXXX"), sqlite_error);


	BOOST_CHECK_NO_THROW(info = p->get_itemtypeinfo("OrderActionCode"));
	BOOST_CHECK_EQUAL(info.basetype, "FTDCharType");
	BOOST_CHECK_EQUAL(info.length, 1);
	BOOST_CHECK_EQUAL(info.precision, 0);
	BOOST_CHECK_EQUAL(info.enumvals[0], "0");
	BOOST_CHECK_EQUAL(info.enumvals[1], "1");
	BOOST_CHECK_EQUAL(info.enumvals[2], "2");
	BOOST_CHECK_THROW(p->get_itemtypeinfo("AbstractXXXX"), sqlite_error);
}

BOOST_AUTO_TEST_CASE(test_fieldsize)
{
	BOOST_CHECK_EQUAL(p->get_field_size("ErrorTargetSequenceField"), 2 + 4);
	//BOOST_CHECK_EQUAL(p->get_field_size("ErrorTargetSequenceField"), 2 + 4);
}
//int main()
//{
//	//::system("make_protocal.exe FTD_protocal.xml protocal");
//
//	std::shared_ptr<ftd_protocalif> p;
//	BOOST_CHECK_NO_THROW(p.reset( new ftd_protocalif("protocal")));
//
//	//ftd_protocalif protocalif("protocal");
//
//	{
//		BOOST_CHECK_EQUAL(p->get_fidname(0x0001), "BulletinField");
//		BOOST_CHECK_THROW(p->get_fidname(0x0000), sqlite_error);
//	}
//
//	{
//		BOOST_CHECK_EQUAL(p->get_fidvalue("BulletinField"), 0x0001);
//		BOOST_CHECK_EQUAL(p->get_fidvalue("BulletinFiel"), 0x0001);
//	}
//    return 0;
//}

