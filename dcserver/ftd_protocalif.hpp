#pragma once

#include <string>
#include <vector>
//#include <tuple>

struct sqlite3;
struct sqlite3_stmt;

class ftd_protocalif
{
public:
	ftd_protocalif(const std::string &sqlitefile);
	~ftd_protocalif();

	//struct item_st {
	//	std::string name;
	//	std::string basetype;
	//	int length;
	//	int precision;
	//	std::string enumval;
	//};

	struct itemtypeinfo {
		std::string basetype;
		int length;
		int precision;
		std::vector<std::string> enumvals;
	};

	struct packagefieldinfo {
		std::string fidname;
		unsigned int maxoccur;
		unsigned int minoccur;
	};

	//struct field_st {
	//	std::string name;
	//	int max_occur;
	//	int min_occur;
	//};


	//std::string get_fid_name(int fid_value) const;
	//int get_fid_value(std::string fid_name) const;
	//int get_field_length(int fid) const;
	//std::vector<field_st> get_field_sequence(const std::string &tid_name) const;
	//std::vector<item_st> get_field_item_sequence(int fid) const;

	std::string get_fidname(int fidvalue) const;
	int get_fidvalue(const std::string &fidname) const;
	std::string get_tidname(int tidvalue) const;
	std::vector<packagefieldinfo> get_package_field_list(const std::string &tidname) const;
	std::vector<packagefieldinfo> get_req_package_field_list(const std::string &tidname) const;
	std::vector<packagefieldinfo> get_rsp_package_field_list(const std::string &tidname) const;
	//std::pair<int, int> get_package_file_maxmin(const std::string &tidname, const std::string &fidname) const;
	std::vector<std::string> get_fielditem_list(const std::string &fidname) const;
	itemtypeinfo get_itemtypeinfo(const std::string &itemname) const;
	int get_field_size(const std::string &fidname) const;
	//std::string get_item_basetype(const std::string &itemname) const;
	//int get_item_length(const std::string &itemname) const;
	//int get_item_precision(const std::string &itemname) const;
	//std::vector<std::string> get_item_enumvalue(const std::string &itemname) const;
private:
	std::vector<ftd_protocalif::packagefieldinfo> get_package_field_list_private(const std::string &tidname, sqlite3_stmt *stmt) const;
	//std::tuple<std::string, int, int, std::string> get_item(const std::string &itemname) const;
	void release();

private:
	sqlite3 *db_;
	sqlite3_stmt *stmt_fidname_;
	sqlite3_stmt *stmt_fidvalue_;
	sqlite3_stmt *stmt_tidname_;
	sqlite3_stmt *stmt_field_list_;
	sqlite3_stmt *stmt_req_field_list_;
	sqlite3_stmt *stmt_rsp_field_list_;
	sqlite3_stmt *stmt_fielditem_list_;
	sqlite3_stmt *stmt_item_;
	sqlite3_stmt *stmt_type_;
};

bool is_in_packagefields(const std::vector<ftd_protocalif::packagefieldinfo> &pfis, const std::string fidname);