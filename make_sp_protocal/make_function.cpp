// make_sp_protocal.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <iostream>
#include <sqlite3.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

#define SQLITE3_CHECK_INVOKE(db, call) \
	if(call != SQLITE_OK) \
		throw std::exception(sqlite3_errmsg(db))

int main(int argc, char *argv[])
try
{
	program_options::options_description desc;
	program_options::variables_map vm;

	string in_xml;
	string out_sqlite;
	desc.add_options()
		("in", program_options::value(&in_xml)->default_value("function.xml"), "XML function configuration file")
		("out", program_options::value(&out_sqlite)->default_value("fuction"), "sqlite3 fuction result file")
		;
	program_options::positional_options_description p;
	p.add("in", 1).add("out", 1);

	program_options::parsed_options parsed = program_options::command_line_parser(argc, argv).options(desc).positional(p).run();
	program_options::store(parsed, vm);
	program_options::notify(vm);


	property_tree::ptree ptree;
	property_tree::read_xml(in_xml, ptree);

	sqlite3 *db;
	SQLITE3_CHECK_INVOKE(db, sqlite3_open(out_sqlite.c_str(), &db));

	//const char *drop_result_table = "DROP TABLE IF EXISTS result_item";
	//SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, drop_result_table, NULL, NULL, NULL));

	//const char *drop_out_table = "DROP TABLE IF EXISTS out_item";
	//SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, drop_out_table, NULL, NULL, NULL));

	char *drop_table[] = {
		"DROP TABLE IF EXISTS result_item",
		"DROP TABLE IF EXISTS out_item",
		"DROP TABLE IF EXISTS in_item",
		"DROP TABLE IF EXISTS tid_sp"
	};

	for (int i = 0; i < sizeof(drop_table) / sizeof(drop_table[0]); i++) {
		SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, drop_table[i], NULL, NULL, NULL));
	}


	char *create_table[] = {
		"CREATE TABLE tid_sp(tid_name TEXT PRIMARY KEY, sp_name TEXT);",
		"CREATE TABLE in_item("
			"tid_name TEXT,"
			"idx INTEGER,"
			"item TEXT,"
			"PRIMARY KEY(tid_name, idx),"
			"FOREIGN KEY(tid_name) REFERENCES tid_sp(tid_name));",
		"CREATE TABLE out_item("
			"tid_name TEXT,"
			"idx INTEGER,"
			"item TEXT,"
			"PRIMARY KEY(tid_name, idx),"
			"FOREIGN KEY(tid_name) REFERENCES tid_sp(tid_name));",
		"DROP TABLE IF EXISTS result_item; CREATE TABLE result_item("
			"tid_name TEXT,"
			"idx INTEGER,"
			"item TEXT,"
			"PRIMARY KEY(tid_name, idx)"
			"FOREIGN KEY(tid_name) REFERENCES tid_sp(tid_name));"
	};

	for (int i = 0; i < sizeof(create_table) / sizeof(create_table[0]); i++) {
		SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_table[i], NULL, NULL, NULL));
	}


	for each (property_tree::ptree::value_type var in ptree.get_child("tids")){
		string tid_name = var.second.get<string>("<xmlattr>.name");
		string sp_name = var.second.get<string>("sp");

		string sql = str(format("INSERT INTO tid_sp (tid_name, sp_name) VALUES (\'%1%\', \'%2%\')") % tid_name % sp_name);
		SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL));
		//property_tree::ptree::iterator it
		int idx = 0;
		for each (property_tree::ptree::value_type in_item in var.second.get_child("in_items"))
		{
			string sql1 = str(format("INSERT INTO in_item (tid_name, idx, item) VALUES (\'%1%\', %2%, \'%3%\')")% tid_name % ++idx % in_item.second.get_value<string>());
			SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, sql1.c_str(), NULL, NULL, NULL));
		}

		idx = 0;
		for each (property_tree::ptree::value_type out_item in var.second.get_child("out_items"))
		{
			string sql1 = str(format("INSERT INTO out_item (tid_name, idx, item) VALUES (\'%1%\', %2%, \'%3%\')") % tid_name % ++idx % out_item.second.get_value<string>());
			SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, sql1.c_str(), NULL, NULL, NULL));
		}

		idx = 0;
		for each (property_tree::ptree::value_type result_item in var.second.get_child("result_items"))
		{
			string sql1 = str(format("INSERT INTO result_item (tid_name, idx, item) VALUES (\'%1%\', %2%, \'%3%\')") % tid_name % ++idx % result_item.second.get_value<string>());
			SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, sql1.c_str(), NULL, NULL, NULL));
		}

	}
    return 0;
}
catch (std::exception &e) {
	cerr << e.what() << endl;
}

