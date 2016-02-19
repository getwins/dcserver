#include "ftd_protocalif.hpp"
#include "sqlitehelper.hpp"

#include <regex>
#include <boost/lexical_cast.hpp>


ftd_protocalif::ftd_protocalif(const std::string & sqlitefile)
	: db_(NULL)
	, stmt_fidname_(NULL)
	, stmt_fidvalue_(NULL)
	, stmt_tidname_(NULL)
	, stmt_field_list_(NULL)
	, stmt_req_field_list_(NULL)
	, stmt_rsp_field_list_(NULL)
	, stmt_fielditem_list_(NULL)
	, stmt_item_(NULL)
	, stmt_type_(NULL)
{
	try {
		SQLITE_CHECK_INVOKE(db_, sqlite3_open_v2(sqlitefile.c_str(), &db_, SQLITE_OPEN_READONLY, NULL));

		const char *sql = "SELECT name FROM field WHERE fidvalue=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_fidname_, NULL));

		sql = "SELECT fidvalue FROM field WHERE name=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_fidvalue_, NULL));

		sql = "SELECT name FROM tid WHERE value=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_tidname_, NULL));

		sql = "SELECT field_list FROM package WHERE tid=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_field_list_, NULL));

		sql = "SELECT field_list FROM package WHERE tid=? AND name LIKE \'Req%\';";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_req_field_list_, NULL));

		sql = "SELECT field_list FROM package WHERE tid=? AND name LIKE \'Rsp%\';";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_rsp_field_list_, NULL));

		sql = "SELECT item_list FROM field WHERE name=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_fielditem_list_, NULL));

		//sql = "SELECT type.basetype, type.length, type.precision, type.enum FROM type JOIN ON"
		sql = "SELECT type FROM item WHERE name=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_item_, NULL));

		sql = "SELECT basetype, length, precision, enum FROM type WHERE name=?;";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt_type_, NULL));
	}
	catch (sqlite_error &e) {
		release();
		BOOST_THROW_EXCEPTION(e);
	}
}

ftd_protocalif::~ftd_protocalif()
{
	release();
}

std::string ftd_protocalif::get_fidname(int fidvalue) const
{
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_fidname_));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_fidname_));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt_fidname_, 1, fidvalue));

	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_fidname_));
	return reinterpret_cast<const char*>(sqlite3_column_text(stmt_fidname_, 0));
}

int ftd_protocalif::get_fidvalue(const std::string & fidname) const
{
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_fidvalue_));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_fidvalue_));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt_fidvalue_, 1, fidname.data(), static_cast<int>(fidname.size()), NULL));

	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_fidvalue_));
	return sqlite3_column_int(stmt_fidvalue_, 0);
}

std::string 
ftd_protocalif::get_tidname(int tidvalue) const
{
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_tidname_));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_tidname_));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_int(stmt_tidname_, 1, tidvalue));
	
	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_tidname_));
	return reinterpret_cast<const char*>(sqlite3_column_text(stmt_tidname_, 0));
}

std::vector<ftd_protocalif::packagefieldinfo>
ftd_protocalif::get_package_field_list(const std::string & tidname) const
{
	return get_package_field_list_private(tidname, stmt_field_list_);
}

std::vector<ftd_protocalif::packagefieldinfo> 
ftd_protocalif::get_req_package_field_list(const std::string & tidname) const
{
	return get_package_field_list_private(tidname, stmt_req_field_list_);
}

std::vector<ftd_protocalif::packagefieldinfo> 
ftd_protocalif::get_rsp_package_field_list(const std::string & tidname) const
{
	return get_package_field_list_private(tidname, stmt_rsp_field_list_);
}

//std::pair<int, int> 
//ftd_protocalif::get_package_file_maxmin(const std::string & tidname, const std::string & fidname) const
//{
//	std::regex reg(fidname + "\\|(\\d+)\\|(\\d+)");
//	std::smatch what;
//	std::string listprivate = get_package_field_list_private(tidname);
//
//	if (std::regex_search(listprivate, what, reg))
//		return std::pair<int, int>(boost::lexical_cast<int>(what[1]), boost::lexical_cast<int>(what[2]));
//	return std::pair<int, int>(0,0);
//}

std::vector<std::string> 
ftd_protocalif::get_fielditem_list(const std::string & fidname) const
{
	static std::regex reg("(\\w+);");

	std::vector<std::string> output;
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_fielditem_list_));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_fielditem_list_));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt_fielditem_list_, 1, fidname.data(), static_cast<int>(fidname.size()), NULL));

	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_fielditem_list_));
	std::string list = reinterpret_cast<const char*>(sqlite3_column_text(stmt_fielditem_list_, 0));
	std::sregex_iterator rit(list.begin(), list.end(), reg), rend;
	for (; rit != rend; ++rit)
		output.push_back((*rit)[1]);

	return output;
}

ftd_protocalif::itemtypeinfo 
ftd_protocalif::get_itemtypeinfo(const std::string & itemname) const
{
	static std::regex reg_str_type("FTDStringType<(\\d+)>"),
		//reg_char_type("FTDCharType"),
		reg_float_type("FTDFloatType<(\\d+),(\\d+)>"),
		//reg_int_type("FTDIntType"),
		reg_number_type("FTDNumberType<(\\d+)>");
		//reg_world_type("FTDWordType");

	itemtypeinfo output;
	std::smatch what;

	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_item_));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_item_));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt_item_, 1, itemname.data(), static_cast<int>(itemname.size()), NULL));

	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_item_));
	
	std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt_item_, 0));
	if (std::regex_match(type, what, reg_str_type)) {
		output.basetype = what[0];
		output.length = boost::lexical_cast<int>(what[1]);
		output.precision = 0;
	}
	else if (type == "FTDCharType") {
		output.basetype = "FTDCharType";
		output.length = 1;
		output.precision = 0;
	}
	else if (std::regex_match(type, what, reg_float_type)) {
		output.basetype = what[0];
		output.length = boost::lexical_cast<int>(what[1]);
		output.precision = boost::lexical_cast<int>(what[2]);
	}
	else if (type == "FTDIntType") {
		output.basetype = "FTDIntType";
		output.length = 4;
		output.precision = 0;
	}
	else if (std::regex_match(type, what, reg_number_type)) {
		output.basetype = what[0];
		output.length = boost::lexical_cast<int>(what[1]);
		output.precision = 0;
	}
	else if (type == "FTDWordType") {
		output.basetype = "FTDWordType";
		output.length = 2;
		output.precision = 0;
	}
	else {
		SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt_type_));
		SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt_type_));
		SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt_type_, 1, type.data(), static_cast<int>(type.size()), NULL));
		SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt_type_));
		output.basetype = reinterpret_cast<const char*>(sqlite3_column_text(stmt_type_, 0));
		output.length = sqlite3_column_int(stmt_type_, 1);
		output.precision = sqlite3_column_int(stmt_type_, 2);
		char* enumlist = _strdup(reinterpret_cast<const char*>(sqlite3_column_text(stmt_type_, 3)));
		for (char *p = strtok(enumlist, ";"); p; p = strtok(NULL, ";")) {
			output.enumvals.push_back(p);
		}
		free(enumlist);
	}
	
	return output;
}

int 
ftd_protocalif::get_field_size(const std::string & fidname) const
{
	int size = 0;
	for each (std::string itemname in get_fielditem_list(fidname)){
		itemtypeinfo i = get_itemtypeinfo(itemname);
		size += i.length;
	}
	return size;
}

std::vector<ftd_protocalif::packagefieldinfo>
ftd_protocalif::get_package_field_list_private(const std::string & tidname, sqlite3_stmt *stmt) const
{
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));
	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));
	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, tidname.data(), static_cast<int>(tidname.size()), NULL));

	SQLITE_CHECK_INVOKE2(SQLITE_ROW, sqlite3_step(stmt));
	std::string listprivate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

	static std::regex field_item_reg("(\\w+)\\|(\\d+)\\|(\\d+);");
	std::vector<packagefieldinfo> output;

	std::sregex_iterator rit(listprivate.begin(), listprivate.end(), field_item_reg), rend;
	for (; rit != rend; ++rit) {
		packagefieldinfo info;
		info.fidname = (*rit)[1];
		info.maxoccur = boost::lexical_cast<unsigned int>((*rit)[2]);
		info.minoccur = boost::lexical_cast<unsigned int>((*rit)[3]);
		output.push_back(info);
	}
		//output.push_back((*rit)[1]);
	return output;
}

void 
ftd_protocalif::release()
{
	if (stmt_fidname_)
		sqlite3_finalize(stmt_fidname_);
	if (stmt_fidvalue_)
		sqlite3_finalize(stmt_fidvalue_);
	if (stmt_tidname_)
		sqlite3_finalize(stmt_tidname_);
	if (stmt_field_list_)
		sqlite3_finalize(stmt_field_list_);
	if (stmt_req_field_list_)
		sqlite3_finalize(stmt_req_field_list_);
	if (stmt_rsp_field_list_)
		sqlite3_finalize(stmt_rsp_field_list_);
	if (stmt_item_)
		sqlite3_finalize(stmt_item_);
	if (stmt_type_)
		sqlite3_finalize(stmt_type_);
	if (db_)
		sqlite3_close_v2(db_);
}

bool is_in_packagefields(const std::vector<ftd_protocalif::packagefieldinfo> &pfis, const std::string fidname)
{
	//for each (ftd_protocalif::packagefieldinfo &var in pfis){
	//	if (var.fidname == fidname)
	//		return true;
	//}
	for (std::vector<ftd_protocalif::packagefieldinfo>::const_iterator cit = pfis.begin();
		cit != pfis.end(); cit++) {
		if (cit->fidname == fidname)
			return true;
	}
	return false;
}
