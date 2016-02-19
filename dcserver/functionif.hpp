#ifndef _FUNCTIONIF_CHP_HPP_
#define _FUNCTIONIF_CHP_HPP_
//  (C) Copyright huiping.chen 2015.

#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

class functionif
{
public:
	functionif(const std::string &sqlitefile);
	~functionif();
	 
	std::string get_sp_name(const std::string &tid_name) const;
	std::vector<std::string> get_input_items(const std::string &tid_name) const;
	std::vector<std::string> get_output_items(const std::string &tid_name) const;
	std::vector<std::string> get_result_items(const std::string &tid_name) const;
	
private:
	void release();
	std::vector<std::string> get_items(sqlite3_stmt *stmt, const std::string &tid_name) const;
private:
	sqlite3 * db_;
	sqlite3_stmt *stmt0_, *stmt1_, *stmt2_, *stmt3_;
};

#endif