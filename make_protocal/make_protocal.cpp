// make_protocal.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <sstream>
#include <map>
#include <sqlite3.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

sqlite3 *db;
property_tree::ptree ptree;

#define SQLITE3_CHECK_INVOKE(db, call) \
do{ \
	if(call != SQLITE_OK) \
		BOOST_THROW_EXCEPTION(std::exception(sqlite3_errmsg(db))); \
} while(0)

#define SQLITE_CHECK_INVOKE1(call) \
do { \
	int ret = call; \
	if (ret != SQLITE_OK){ \
		BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret))); \
	} \
} while (0)


void type_table()
{
	char *create_sql = //"DROP TABLE IF EXISTS type;"
		"CREATE TABLE type(name TEXT RPIMARY, basetype TEXT, length INT, precision INT, enum TEXT);";
	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_sql, NULL, NULL, NULL));

	sqlite3_stmt *stmt;
	char *insert_sql = "INSERT INTO type(name, basetype, length, precision, enum) VALUES (?, ?, ?, ?, ?);";
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));

	for each(property_tree::ptree::value_type var in ptree.get_child("ftd.types")) {
		if (var.first != "typeDefine")
			continue;
		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));
		//SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, tid_name.c_str(), tid_name.size(), NULL));

		string enumvalues;	
		for each(property_tree::ptree::value_type val in var.second) {
			if(val.first == "enumValue")
				enumvalues += val.second.get<string>("<xmlattr>.name") + ";";
		}

		string name = var.second.get<string>("<xmlattr>.name");
		string basetype = var.second.get<string>("<xmlattr>.baseType");
		int	length = var.second.get<int>("<xmlattr>.length");
		int precision = var.second.get<int>("<xmlattr>.precision");
			
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, name.data(), name.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 2, basetype.data(), basetype.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt, 3, length));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt, 4, precision));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 5, enumvalues.data(), enumvalues.size(), NULL));
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret)));
	}
	sqlite3_finalize(stmt);
}

void
item_table()
{
	char *create_sql = //"DROP TABLE IF EXISTS item;"
		//"CREATE TABLE item (name TEXT PRIMARY KEY, type TEXT, FOREIGN KEY(type) REFERENCES type(name));"
		"CREATE TABLE item (name TEXT PRIMARY KEY, type TEXT);";
	//char *create_sql = "DROP TABLE IF EXISTS item;"
	//	"CREATE TABLE item (name TEXT, type TEXT);";
	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_sql, NULL, NULL, NULL));

	char *insert_sql = "INSERT INTO item (name, type) VALUES (?, ?);";
	sqlite3_stmt *stmt;
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));
	for each(property_tree::ptree::value_type var in ptree.get_child("ftd.items")) {
		if (var.first != "itemDefine")
			continue;
		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));
		string name = var.second.get<string>("<xmlattr>.name");
		//cerr << name << endl;
		string type = var.second.get<string>("<xmlattr>.type");
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, name.data(), name.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 2, type.data(), type.size(), NULL));
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret)));
	}
	sqlite3_finalize(stmt);
}

void field_table()
{
	char *create_sql = //"DROP TABLE IF EXISTS field;"
		"CREATE TABLE field (name TEXT PRIMARY KEY, fid TEXT UNIQUE, fidvalue INT UNIQUE, item_list TEXT);"
		//"DROP INDEX IF EXISTS fidvalue_index;"
		"CREATE INDEX fidvalue_index ON field(fidvalue);";
	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_sql, NULL, NULL, NULL));

	char *insert_sql = "INSERT INTO field(name, fid, fidvalue, item_list) VALUES (?, ?, ?, ?);";
	sqlite3_stmt *stmt;
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));
	for each(property_tree::ptree::value_type var in ptree.get_child("ftd.fields")) {
		if (var.first != "fieldDefine")
			continue;
		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));

		string name = var.second.get<string>("<xmlattr>.name");
		string fid = var.second.get<string>("<xmlattr>.fid");
		
		//boost::algorithm::starts_with()
		string fidvalue = var.second.get<string>("<xmlattr>.fidValue");
		int ival = -1; 
		istringstream iss(fidvalue);
		if (algorithm::starts_with(fidvalue, "0x")) {
			iss >> std::hex >> std::showbase >> ival;
		}else if (algorithm::starts_with(fidvalue, "0")) {
			iss >> std::oct >> std::showbase >> ival;
		}else {
			iss >> ival;
		}
	//	int fidvalue = lexical_cast<int>(algorithm::unhex(var.second.get<string>("<xmlattr>.fidValue")));
		string item_list;
		for each (property_tree::ptree::value_type v in var.second){
			if (v.first == "item")
				item_list += v.second.get<string>("<xmlattr>.name") + ";";
		}
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, name.data(), name.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 2, fid.data(), fid.size(), NULL));
		//SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 3, fidvalue.data(), fidvalue.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt, 3, ival));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 4, item_list.data(), item_list.size(), NULL));
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret)));
	}
	sqlite3_finalize(stmt);
}

void
tid_table()
{
	char *create_sql = //"DROP TABLE IF EXISTS  tid;"
		"CREATE TABLE tid (name TEXT PRIMARY KEY, value TEXT UNIQUE);"
		//"DROP INDEX IF EXISTS tidvalue_index;"
		"CREATE INDEX tidvalue_index ON tid(value);";
	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_sql, NULL, NULL, NULL));

	char *insert_sql = "INSERT INTO tid(name, value) VALUES (?, ?);";
	sqlite3_stmt *stmt;
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));

	for each(property_tree::ptree::value_type var in ptree.get_child("ftd.tids")) {
		if (var.first != "tidDefine")
			continue;

		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));

		string name = var.second.get<string>("<xmlattr>.name");
		string value = var.second.get<string>("<xmlattr>.value");

		int ival = -1;
		istringstream iss(value);
		if (algorithm::starts_with(value, "0x")) {
			iss >> std::hex >> std::showbase >> ival;
		}
		else if (algorithm::starts_with(value, "0")) {
			iss >> std::oct >> std::showbase >> ival;
		}
		else {
			iss >> ival;
		}
		//cerr << name << endl;
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, name.data(), name.size(), NULL));
		//SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 2, value.data(), value.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt, 2, ival));
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret)));
	}
	sqlite3_finalize(stmt);

}

void
package_table()
{
	char *create_sql = //"DROP TABLE IF EXISTS package;"
		//"CREATE TABLE package (name TEXT PRIMARY KEY, tid TEXT UNIQUE, model TEXT, field_list TEXT, FOREIGN KEY(tid) REFERENCES tid(name));";
		"CREATE TABLE package (name TEXT PRIMARY KEY, tid TEXT , model TEXT, field_list TEXT, FOREIGN KEY(tid) REFERENCES tid(name));";

	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, create_sql, NULL, NULL, NULL));

	char *insert_sql = "INSERT INTO package(name, tid, model, field_list) VALUES (?, ?, ?, ?);";
	char *check_field_sql = "SELECT COUNT(*) FROM field WHERE name=?";
	sqlite3_stmt *stmt;
	sqlite3_stmt *check_field_stmt; //检验field必须在field表中存在
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));
	SQLITE3_CHECK_INVOKE(db, sqlite3_prepare_v2(db, check_field_sql, -1, &check_field_stmt, NULL));

	map<string, string> tid_model;

	for each(property_tree::ptree::value_type var in ptree.get_child("ftd.packages")) {
		if (var.first != "packageDefine")
			continue;
		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));

		string name = var.second.get<string>("<xmlattr>.name");
		//cerr << name << endl;
		string tid = var.second.get<string>("<xmlattr>.tid");
		string model = var.second.get<string>("<xmlattr>.model");

		//if(tid_model.fin)
		auto it = tid_model.find(tid);
		if (it != tid_model.end()) {
			if (it->second != model) {
				ostringstream oss;
				oss << "存在tid(" << tid << ")相同，而model值不同的包定义，插入package表异常。";
				BOOST_THROW_EXCEPTION(std::runtime_error(oss.str()));
			}
		}

		string field_list;
		for each(property_tree::ptree::value_type v in var.second) {
			if (v.first == "field") {
				std::string fieldname = v.second.get<string>("<xmlattr>.name");
				SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(check_field_stmt));
				SQLITE_CHECK_INVOKE1(sqlite3_reset(check_field_stmt));
				SQLITE_CHECK_INVOKE1(sqlite3_bind_text(check_field_stmt, 1, fieldname.data(), fieldname.size(), NULL));
				int ret = sqlite3_step(check_field_stmt);
				if (ret != SQLITE_ROW) {
					ostringstream oss;
					oss << "field表中不存在" << fieldname << ",插入package表异常。";
					BOOST_THROW_EXCEPTION(std::runtime_error(oss.str()));
				}
					//BOOST_THROW_EXCEPTION(std::exception());

				field_list += v.second.get<string>("<xmlattr>.name") + "|"
					+ v.second.get<string>("<xmlattr>.maxOccur") + "|"\
					+ v.second.get<string>("<xmlattr>.minOccur") + ";";
			}
		}

		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, name.data(), name.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 2, tid.data(), tid.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 3, model.data(), model.size(), NULL));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 4, field_list.data(), field_list.size(), NULL));
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			BOOST_THROW_EXCEPTION(std::exception(sqlite3_errstr(ret)));

		if (it == tid_model.end())
			tid_model[tid] = model;

	}
	sqlite3_finalize(stmt); 
	sqlite3_finalize(check_field_stmt);
}



int main(int argc, char *argv[])
try
{
	program_options::options_description desc;
	program_options::variables_map vm;

	string in_xml;
	string out_sqlite;
	desc.add_options()
		("in", program_options::value(&in_xml)->default_value("ftd_protocal.xml"), "XML function configuration file")
		("out", program_options::value(&out_sqlite)->default_value("protocal"), "sqlite3 fuction result file")
		;
	program_options::positional_options_description p;
	p.add("in", 1).add("out", 1);

	program_options::parsed_options parsed = program_options::command_line_parser(argc, argv).options(desc).positional(p).run();
	program_options::store(parsed, vm);
	program_options::notify(vm);

	property_tree::read_xml(in_xml, ptree);

	if (filesystem::exists(out_sqlite))
		filesystem::remove(out_sqlite);
	SQLITE3_CHECK_INVOKE(db, sqlite3_open_v2(out_sqlite.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE , NULL));

	SQLITE3_CHECK_INVOKE(db, sqlite3_exec(db, "PRAGMA foreign_keys=1", NULL, NULL, NULL));
	type_table();

	item_table();

	field_table();

	tid_table();

	package_table();

	sqlite3_close(db);

    return 0;
}
catch (boost::exception &e) {
	cerr << boost::diagnostic_information(e) << endl;
}
catch (std::exception &e) {
	cerr << e.what() << endl;
}

