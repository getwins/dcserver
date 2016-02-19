#include "dbconnpool.hpp"

//#include <thread>
//#include <memory>
#include <sstream>
//#include <boost/lexical_cast.hpp>
//#include <iomanip>
#include <boost/throw_exception.hpp>

#include <WinSock2.h> 

#include <mysql.h>


#define MYSQL_CHECK_INVOKE(call) \
do{ \
	MYSQL *con = (call); \
	if (mysql_errno(con) != 0) { \
		std::ostringstream oss; \
		oss << #call << ":" << mysql_error(con); \
		BOOST_THROW_EXCEPTION(db_error(oss.str())); \
	} \
} while(0)

//MYSQL *mysql = (call); \
//if (mysql_errno(mysql) != 0) {
//	\
//		std::ostringstream oss; \
//		oss << #call << ":" << mysql_error(mysql); \
//		BOOST_THROW_EXCEPTION(db_error(oss.str())); \
//} \

void 
free_mysql(MYSQL *mysql)
{
	if (mysql) {
		mysql_close(mysql);
		delete mysql;
	}
}

thread_local std::shared_ptr<MYSQL> mysql;



dbconnpool::dbconnpool(const std::string & host, const std::string & user, const std::string & passwd, const std::string & db, unsigned int port)
	: host_(host)
	, user_(user)
	, passwd_(passwd)
	, db_(db)
	, port_(port)
{
}

dbconnpool::~dbconnpool()
{
}


void dbconnpool::invoke(const std::string &tidname, const ftd_protocalif &protocalif, const functionif &funcif, ftdc_variable &ftdc_var)
{
	if (!mysql.get()) {
		mysql.reset(new MYSQL);
		MYSQL_CHECK_INVOKE(mysql_init(mysql.get()));
		MYSQL_CHECK_INVOKE(mysql_real_connect(mysql.get(), host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), port_, NULL, 0));
	}

	std::ostringstream oss_sp, oss_out;
	int idx = 0;
	oss_sp << "CALL " << funcif.get_sp_name(tidname) << "(";
	for each (std::string input in  funcif.get_input_items(tidname))
	{
		if (idx++)
			oss_sp << ',';
		ftdc_path path(input);
		itemvalue_ptr value = ftdc_var.get_field_item(path);
		oss_sp << value->str();
		/*ftd_protocalif::itemtypeinfo info = protocalif.get_itemtypeinfo(path.item_name);
		std::string val = ftdc_var.get_field_item(input);
		if (info.basetype == "FTDCharType") {
			oss_sp << val[0];
		}else if (info.basetype.find("FTDFloatType") != std::string::npos) {
			if (!val.empty()) {;
				size_t off = val[0] == '-' ? 1 : 0;
				size_t pos = val.find_first_not_of('0', off);
				val.erase(off, pos - off);
			}
			oss_sp << val;
		}else if(info.basetype == "FTDIntType"){ 
			oss_sp << *(reinterpret_cast<const int*>(val.data()));
		}else if (info.basetype.find("FTDNumberType") != std::string::npos){
			if (!val.empty()) {
				size_t off = val[0] == '-' ? 1 : 0;
				size_t pos = val.find_first_not_of('0', off);
				val.erase(off, pos - off);
			}
			oss_sp << val;
		}else if (info.basetype.find("FTDStringType") != std::string::npos) {
			size_t pos = val.find_first_of(" ", 0, info.length);
			val.erase(pos);
			oss_sp << '\'' << val << '\'';
		}else if (info.basetype == "FTDWordType") {
			oss_sp << *(reinterpret_cast<const unsigned short *>(val.data()));
		}*/

	}

	std::vector<std::string> outs = funcif.get_output_items(tidname);
	for each (std::string output in outs){
		if (idx++)
			oss_sp << ',';
		oss_sp << '@' << output;
	}
	oss_sp << ");";

	std::string sql = oss_sp.str();
	mysql_real_query(mysql.get(), sql.data(), static_cast<unsigned long>(sql.size()));
	
	MYSQL_RES *res = mysql_store_result(mysql.get());
	MYSQL_ROW row;
	MYSQL_FIELD *fields;
	unsigned int field_num;
	//ftdc_variable ftdc_var;
	
	fields = mysql_fetch_fields(res);
	field_num = mysql_num_fields(res);
	std::vector<std::string> result_items = funcif.get_result_items(tidname);
	
	for (int idx = 0; row = mysql_fetch_row(res); idx++) {
		for (size_t i = 0; i < result_items.size() && i < field_num; i++) {
			char *col = row[i];
			ftdc_path path(result_items[i]);
			ftd_protocalif::itemtypeinfo info = protocalif.get_itemtypeinfo(path.item_name());
			ftd_item_type_t type = get_ftd_item_type(info.basetype);
			itemvalue_ptr value(create_ftd_item_value_from_str(type, col, info.length, info.precision));
			ftdc_var.set_field_item(path.fid_name(), idx, path.item_name(), value);
			//std::string val;
			////std::istringstream iss;
			//if (info.basetype == "FTDCharType") {
			//	val.assign(col, 1);
			//}
			//else if (info.basetype.find("FTDFloatType") != std::string::npos) {
			//	//val.assign(col);
			//	float f; //= boost::lexical_cast<float>(col);
			//	std::istringstream iss(col);
			//	iss >> f;

			//	std::ostringstream oss;
			//	oss << std::setfill('0') << std::setw(info.length) << std::setprecision(info.precision) << f;
			//	val = oss.str();

			//}
			//else if (info.basetype == "FTDIntType") {
			//	//oss_sp << *(reinterpret_cast<const int*>(val.data()));
			//	int i;
			//	std::istringstream iss(col);
			//	iss >> i;
			//	val.assign(reinterpret_cast<char*>(&i), sizeof(i));
			//} 
			//else if (info.basetype.find("FTDNumberType") != std::string::npos) {
			//	std::istringstream iss(col);
			//	//iss >>
			//	int i = 0;
			//	iss >> i;
			//	std::ostringstream oss;
			//	oss << std::setw(info.length) << i;
			//	val = oss.str();
			//	//if (!val.empty()) {
			//	//	size_t off = val[0] == '-' ? 1 : 0;
			//	//	size_t pos = val.find_first_not_of('0', off);
			//	//	val.erase(off, pos - off);
			//	//}
			//	//oss_sp << val;
			//}
			//else if (info.basetype.find("FTDStringType") != std::string::npos) {

			//	val.assign(info.length, ' ');
			//	val.replace(0, strlen(col), col);
			//	//size_t pos = val.find_first_of(" ", 0, info.length);
			//	//val.erase(pos);
			//	//oss_sp << '\'' << val << '\'';
			//}
			//else if (info.basetype == "FTDWordType") {
			//	//oss_sp << *(reinterpret_cast<const unsigned short *>(val.data()));
			//	unsigned short us = 0;
			//	std::istringstream iss(col);
			//	iss >> us;
			//	val.assign(reinterpret_cast<char*>(&us), sizeof(us));
			//}

			//ftdc_var.set_field_item(path.fid_name(), idx, path.item_name(), val);
		}
	}

	if (!outs.empty()) {
		std::ostringstream oss;
		oss << "SELECT ";
		for (auto it = outs.begin(); it != outs.end(); ++it) {
			if (it != outs.begin())
				oss << ",";
			oss << "@" << *it;
		}
		oss << ";";
		mysql_real_query(mysql.get(), oss.str().data(), static_cast<unsigned long>(oss.str().size()));
		res = mysql_store_result(mysql.get());
		//fields = mysql_fetch_fields(res);
		//field_num = mysql_num_fields(res);

		row = mysql_fetch_row(res);
		for (size_t i = 0; i < outs.size(); i++) {
			char *col = row[i];
			ftdc_path path(result_items[i]);
			ftd_protocalif::itemtypeinfo info = protocalif.get_itemtypeinfo(path.item_name());
			ftd_item_type_t type = get_ftd_item_type(info.basetype);
			itemvalue_ptr value(create_ftd_item_value_from_str(type, col, info.length, info.precision));
			ftdc_var.set_field_item(path, value);
		}
	}


}
