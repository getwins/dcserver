#include "functionif.hpp"

//#include <sqlite3.h>
#include "sqlitehelper.hpp"
using namespace std;

functionif::functionif(const std::string &sqlitefile)
	: db_(NULL)
	, stmt0_(NULL)
	, stmt1_(NULL)
	, stmt2_(NULL)
	, stmt3_(NULL)
{
	//if (sqlite3_open(sqlite_mdf, &db_) != SQLITE_OK)
	//	throw std::runtime_error(sqlite3_errmsg(db_));
	try {
		SQLITE_CHECK_INVOKE(db_, sqlite3_open_v2(sqlitefile.c_str(), &db_, SQLITE_OPEN_READONLY, NULL));

		const char *sql = "SELECT sp_name FROM tid_sp WHERE tid_name=?";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt0_, NULL));

		sql = "SELECT item FROM in_item WHERE tid_name=? ORDER BY idx";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt1_, NULL));

		sql = "SELECT item FROM out_item WHERE tid_name=? ORDER BY idx";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt2_, NULL));

		sql = "SELECT item FROM result_item WHERE tid_name=? ORDER BY idx";
		SQLITE_CHECK_INVOKE(db_, sqlite3_prepare_v2(db_, sql, -1, &stmt3_, NULL));

	}catch (sqlite_error &e) {
		release();
		BOOST_THROW_EXCEPTION(e);
	}
}

functionif::~functionif()
{
	release();
}

std::string functionif::get_sp_name(const std::string & tid_name) const
{
	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt0_));

	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt0_));

	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt0_, 1, tid_name.c_str(), static_cast<int>(tid_name.size()), NULL));


	if (sqlite3_step(stmt0_) != SQLITE_ROW)
		BOOST_THROW_EXCEPTION(sqlite_error(sqlite3_errmsg(db_)));

	return reinterpret_cast<const char *>(sqlite3_column_text(stmt0_, 0));

}

std::vector<std::string> functionif::get_input_items(const std::string & tid_name) const
{
	return get_items(stmt1_, tid_name);
}

std::vector<std::string> functionif::get_output_items(const std::string & tid_name) const
{
	return get_items(stmt2_, tid_name);
}

std::vector<std::string> functionif::get_result_items(const std::string & tid_name) const
{
	return get_items(stmt3_, tid_name);
}

void 
functionif::release()
{
	if (stmt3_)
		sqlite3_finalize(stmt3_);
	if (stmt2_)
		sqlite3_finalize(stmt2_);
	if (stmt1_)
		sqlite3_finalize(stmt1_);
	if (stmt0_)
		sqlite3_finalize(stmt0_);
	if (db_)
		sqlite3_close(db_);
}

std::vector<std::string> 
functionif::get_items(sqlite3_stmt * stmt, const std::string & tid_name) const
{
	vector<string> output;

	SQLITE_CHECK_INVOKE1(sqlite3_clear_bindings(stmt));

	SQLITE_CHECK_INVOKE1(sqlite3_reset(stmt));

	SQLITE_CHECK_INVOKE1(sqlite3_bind_text(stmt, 1, tid_name.c_str(), static_cast<int>(tid_name.size()), NULL));

	while (sqlite3_step(stmt) == SQLITE_ROW)
		output.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
	
	return output;
}
