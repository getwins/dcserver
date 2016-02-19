// test_function.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
const char * xml = "";

#include <stdlib.h>
#include <sqlite3.h>
#include <exception>
#include <set>
#include <boost/assert.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#define SQLITE3_CHECK_INVOKE(db, call) \
	if(call != SQLITE_OK) \
		throw std::exception(sqlite3_errmsg(db))

sqlite3 *db;

void
test_table()
{
	std::set<std::string>  res, tres;
	tres.insert("tid_sp");
	tres.insert("in_item");
	tres.insert("out_item");
	tres.insert("result_item");

	sqlite3_stmt *stmt;
	char *sql = "SELECT name FROM sqlite_master "
		"WHERE type = 'table' "
		"ORDER BY name;";

	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare(db, sql, -1, &stmt, NULL));
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		res.insert(reinterpret_cast< const char*>(sqlite3_column_text(stmt, 0)));
	}

	sqlite3_finalize(stmt);

	BOOST_ASSERT(res == tres);
	//sqlite3_prepare_v2(db, )
}

void 
test_tid_sp_table()
{
	sqlite3_stmt *stmt;
	char *sql = "SELECT tid_name, sp_name FROM tid_sp;";
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare(db, sql, -1, &stmt, NULL));
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::string tid_name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		std::string sp_name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		if (tid_name == "TID_UserLogin") {
			BOOST_ASSERT(sp_name == "sp_userlogin");
		}
		else if (tid_name == "TID_UserLogout") {
			BOOST_ASSERT(sp_name == "sp_userlogout");
		}
		else {
			BOOST_ASSERT(0);
		}
	}
	sqlite3_finalize(stmt);
}

int main()
{
	::system("make_function function.xml function");
	
	//sqlite3_open("function", &db)
	SQLITE3_CHECK_INVOKE(db, sqlite3_open("function", &db));

	test_table();

	test_tid_sp_table();

    return 0;

}

