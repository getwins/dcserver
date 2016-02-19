#ifndef _FTDC_CONTENT_PARSER_CHP_HPP_
#define _FTDC_CONTENT_PARSER_CHP_HPP_

//  (C) Copyright huiping.chen 2015.

#include <string>
//#include <map>
#include <exception>
//#include <regex>
#include <vector>

#include "ftd.hpp"
#include "ftdc_buffer.hpp"
#include "ftd_protocalif.hpp"
#include "ftdc_variable.hpp"

struct ftdc_content_error : public std::exception {
	ftdc_content_error(const std::string &message) : std::exception(message.data(), static_cast<int>(message.size())){}
};

//ftdc_variable 
//parse_ftdc_content(const ftdc_header_st &ftdc_header
//	, const char *content
//	//, int len
//	, ftd_protocalif &protocal);

ftdc_variable parse_ftdc_content(const ftdc_buffer &buffer, const ftd_protocalif &protocalif);

/*need field count*/
/*std::vector<std::string> */
//std::vector<ftdc_buffer>
//serialize_ftdc_content(const ftdc_header_st &ftdc_header
//	, ftdc_variable & ftdc_var
//	, ftd_protocalif &protocal);

ftdc_buffer_vec
serialize_ftdc_content(const ftdc_buffer &buffer_i, const ftdc_variable &ftdc_var, const ftd_protocalif&protocalif);

//class ftdc_value 
//{
//public:
//	std::string &field_item(const std::string &ftdc_expr) 
//	{
//		std::regex reg("(\\w+)\\[(\\d+)\\]\\.(\\w+)");
//		std::regex reg1("(\\w+)\\.(\\w+)");
//		std::smatch what;
//		if (std::regex_match(ftdc_expr, what, reg)) {
//			return field_item(what[1], what[3], what[2]);
//		}else if (std::regex_match(ftdc_expr, what, reg1) {
//			return (field_item(what[1], what[2]));
//		}
//		else
//			throw ftdc_content_error();
//	}
//	std::string &field_item(const std::string &fid, const std::string &item, int fidindex = 0) 
//	{
//		field_vector_t &fv = fm_[fid];
//		if (fv.size() <= fidindex) {
//			for (int i = fv.size(); i < fidindex + 1; ++i)
//				fv.push_back(item_map_t());
//		}
//		return fv[fidindex][item];
//	}
//
//	size_t field_size(const string &fid_name) const 
//	{
//		return fm_[fid_name].size();
//	}
//	//std::string & operator[][](const std::string &fid, const std::string &item);
//private:
//	typedef std::map<std::string, std::string> item_map_t;
//	typedef std::vector<item_map_t> field_vector_t;
//	typedef std::map<std::string, field_vector_t> field_map_t;
//
//	field_map_t fm_;
//
//};
//
//class ftdc_content_parser
//{
//public:
//	ftdc_content_parser(const std::string &content);
//	~ftdc_content_parser();
//
//	void run(ftdc_value &result);
//private:
//	std::string content_;
//};

#endif