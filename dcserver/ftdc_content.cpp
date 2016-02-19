#include "ftdc_content.hpp"

#include <boost/format.hpp>

using namespace std;
using namespace boost;



//ftdc_variable
//parse_ftdc_content(const ftdc_header_st &ftdc_header, const char *content, /*int len,*/ ftd_protocalif &protocalif)
ftdc_variable 
parse_ftdc_content(const ftdc_buffer &buffer, const ftd_protocalif &protocalif)
{
	//ftdc_value output;
	ftdc_variable ftdc_var;
	map<int, int> fid_index;
	//int field_count = 0;
	//string tidname = protocalif.get_tidname(ftdc_header.tid);
	string tidname = protocalif.get_tidname(buffer.header()->tid);
	vector<ftd_protocalif::packagefieldinfo> pfis = protocalif.get_rsp_package_field_list(tidname);
	//for (const char *pos = content; pos < content + ftdc_header.ftdc_content_length; ) {
	for (unsigned short index = 0; index < buffer.field_count(); ++index){
	//for (const char *pos = buffer.content(); pos < content + ftdc_header.ftdc_content_length; ) {
		//const ftdc_field_st *field = reinterpret_cast<const ftdc_field_st*>(pos);
		//if(field->field_length != protocalif.get_field_length(field->fid))
		const ftdc_field_st *field = buffer.get_field(index);
		string fidname = protocalif.get_fidname(field->fid);

		if (!is_in_packagefields(pfis, fidname))
			BOOST_THROW_EXCEPTION(ftdc_content_error(str(format("%1%请求包中出现协议中未定义的%2%域") % tidname % fidname)));

		if (field->field_length != protocalif.get_field_size(fidname))
			BOOST_THROW_EXCEPTION(ftdc_content_error(str(format("%1%域内容长度与协议不符") % field->fid))); 

		if (fid_index.find(field->fid) == fid_index.end())
			fid_index[field->fid] = 0;

		//string fid_name = protocalif.get_fid_name(field->fid);
		//vector<ftd_protocalif::item_st> item_seq= protocalif.get_field_item_sequence(field->fid);
		//for (auto it = item_seq.begin(); it != item_seq.end(); ++it)
		//	output.field_item(fid_name, it->name, fid_index[field->fid]) = string(field->dataitem, field->field_length);

		vector<string> itemlist = protocalif.get_fielditem_list(fidname);
		const char *item_pos = field->dataitem;
		for each (string itemname in itemlist)
		{
			ftd_protocalif::itemtypeinfo info = protocalif.get_itemtypeinfo(itemname);
			//string itemvalue(item_pos, info.length);
			//ftdc_var.set_field_item(fidname, fid_index[field->fid], itemname, itemvalue);
			ftd_item_type_t type = get_ftd_item_type(info.basetype);
			itemvalue_ptr value(create_ftd_item_value_from_data(type, item_pos, info.length, info.precision));
			ftdc_var.set_field_item(fidname, fid_index[field->fid], itemname, value);

			if (!info.enumvals.empty()) {
				//type.enumvals.
				string itemvalue(item_pos, info.length);
				if (std::count(info.enumvals.begin(), info.enumvals.end(), itemvalue) == 0)
					BOOST_THROW_EXCEPTION(ftdc_content_error(str(format("%1%的值%2%未在枚举定义里") % itemname % itemvalue)));
			}
		}
		
		fid_index[field->fid]++;
		//pos += sizeof(ftdc_field_st) -1 + field->field_length;
		//field_count++;
	}

	return ftdc_var;
}

//vector<string>
//std::vector<ftdc_buff_t>
//vector<ftdc_buffer>

ftdc_buffer_vec
serialize_ftdc_content(const ftdc_buffer &buffer_i, const ftdc_variable &ftdc_var, const ftd_protocalif&protocalif)
{
	//vector<string> output;
	//vector<ftdc_buff_t> output;
	vector<ftdc_buffer> output;
	ftdc_buffer buffer;
	buffer.header()->seqno = buffer_i.header()->seqno;
	buffer.header()->seris = buffer_i.header()->seris;
	buffer.header()->tid = buffer_i.header()->tid;
	buffer.header()->version = buffer_i.header()->version;
	//ftdc_header_st *ftdc_header_o = reinterpret_cast<ftdc_header_st*>(buff);
	//ftdc_content_buff_t *ftdc_content
	//char *ftdc_content = buff + sizeof(ftdc_header_st);
	//string ftdc_content;
	//vector<ftd_protocalif::field_st> field_seq = protocalif.get_field_sequence(tid_name);
	//string tidname = protocalif.get_tidname(ftdc_header.tid);
	string tidname = protocalif.get_tidname(buffer_i.header()->tid);
	vector<ftd_protocalif::packagefieldinfo> pfis = protocalif.get_rsp_package_field_list(tidname);

	for each (ftd_protocalif::packagefieldinfo pfi in pfis)
	{
		if (ftdc_var.field_count(pfi.fidname) > pfi.maxoccur)
			BOOST_THROW_EXCEPTION(ftdc_content_error(str(format("%1%应答中域%2%个数超过协议的最大数%3%") % tidname % pfi.fidname % pfi.maxoccur)));

		if(ftdc_var.field_count(pfi.fidname) < pfi.minoccur)
			BOOST_THROW_EXCEPTION(ftdc_content_error(str(format("%1%应答中域%2%个数少于协议的最大数%3%") % tidname % pfi.fidname % pfi.minoccur)));

		int fidvalue = protocalif.get_fidvalue(pfi.fidname);
		int fidsize = protocalif.get_field_size(pfi.fidname);

		for (size_t fieldidx = 0; fieldidx < ftdc_var.field_count(pfi.fidname); ++fieldidx) {
			char fieldbuf[FTDC_CONTENT_MAX_LENGTH] = { 0 };
			ftdc_field_st *field = reinterpret_cast<ftdc_field_st*>(fieldbuf);
			field->fid = fidvalue;
			field->field_length = fidsize;
			char *pos = field->dataitem;
			for each (string itemname in protocalif.get_fielditem_list(pfi.fidname))
			{
				//ftd_protocalif::itemtypeinfo type = protocalif.get_itemtypeinfo(itemname);
				//string itemvalue = ftdc_var.get_field_item(pfi.fidname, fieldidx, itemname);
				//itemvalue.copy(pos, type.length);
				//pos += type.length;
				itemvalue_ptr value = ftdc_var.get_field_item(pfi.fidname, fieldidx, itemname);
				memcpy(pos, value->data(), value->size());
				pos += value->size();
				
			}
			//ftdc_header_o->field_count += 1;
			//ftdc_header_o->ftdc_content_length += ftdc_field_entity_size(*field);
			
			if (!buffer.add_field(*field)) {
				output.push_back(buffer);
				buffer.reset();
				buffer.header()->seqno = buffer_i.header()->seqno;
				buffer.header()->seris = buffer_i.header()->seris;
				buffer.header()->tid = buffer_i.header()->tid;
				buffer.header()->version = buffer_i.header()->version;
				buffer.add_field(*field);
			}
			//int fieldsize = ftdc_field_entity_size(*field);
			//if (ftdc_header_o->ftdc_content_length + fieldsize > FTDC_CONTENT_MAX_LENGTH) {
			//	output.push_back(buff);
			//	memset(buff, 0, sizeof(buff));
			//}
			//memcpy(ftdc_content + ftdc_header_o->ftdc_content_length, fieldbuf, fieldsize);
			//ftdc_header_o->field_count += 1;
			//ftdc_header_o->ftdc_content_length += ftdc_field_entity_size(*field);
			

			//if (ftdc_content.size() + ftdc_field_entity_size(*field) > FTDC_MAX_LENGTH) {
			//	output.push_back(ftdc_content);
			//	ftdc_content.clear();
			//}

			//ftdc_content.append(fieldbuf, ftdc_field_entity_size(*field));
		}

	}

	//for (auto it = field_seq.begin(); it != field_seq.end(); ++it) {
	//	int fid_value = protocalif.get_fid_value(it->name);
	//	int field_length = protocalif.get_field_length(fid_value);
	//	vector<ftd_protocalif::item_st> item_seq = protocalif.get_field_item_sequence(fid_value);
	//	char fieldbuf[FTDC_MAX_LENGTH];
	//	ftdc_field_st *field = reinterpret_cast<ftdc_field_st*>(fieldbuf);
	//	field->fid = fid_value;
	//	field->field_length = field_length;


	//	if(ftdc_val.field_size(it->name) < it->min_occur)
	//		throw ftdc_content_error("");

	//	if (ftdc_val.field_size(it->name) > it->max_occur)
	//		throw ftdc_content_error("");

	//	for (int fidindex = 0; fidindex < ftdc_val.field_size(it->name) && fidindex < it->max_occur; ++fidindex) {
	//		int pos = 0;
	//		for (auto item_it = item_seq.begin(); item_it != item_seq.end(); ++it) {
	//			string item_val = ftdc_val.field_item(it->name, item_it->name, fidindex);
	//			item_val.copy(field->dataitem + pos, item_it->length);
	//		}

	//		if (ftdc_content.size() + ftdc_field_entity_size(*field) > FTDC_MAX_LENGTH) {
	//			output.push_back(ftdc_content);
	//			ftdc_content.clear();
	//		}else {
	//			ftdc_content.append(fieldbuf, ftdc_field_entity_size(*field));
	//		}
	//	}
	//	
	//	output.push_back(ftdc_content);
	//}

	return output;
}

//ftdc_content_parser::ftdc_content_parser(const std::string & content)
//	:content_(content)
//{
//
//}
//
//ftdc_content_parser::~ftdc_content_parser()
//{
//}

//#include <mysql.h>
//#include "functionif.hpp"
//#include <sstream>
//#include "ftdc_variable.hpp"
//#include "ftdc_path.hpp"
//void 
//db_invoke(string tid_name, ftdc_value &in, ftdc_value &out)
//{
//	MYSQL mysql;
//	functionif protocalif("function");
//
//	ostringstream oss;
//	
//	oss << "CALL ";
//	oss << protocalif.get_sp_name(tid_name);
//	oss << "(";
//	
//	int offset = 0;
//	for each (string var in protocalif.get_input_items(tid_name))
//	{
//		if (offset++)
//			oss << ","; 
//		oss << in.field_item(var);
//	}
//
//	//oss_out_param << "SELECT ";
//	for each (string var in protocalif.get_output_items(tid_name)) {
//		if(offset++)
//			oss << ",";
//		oss << "@" << var;
//	}
//	oss << ");";
//	//protocalif.get
//	string str = oss.str();
//	mysql_real_query(&mysql, str.data(), str.size());
//
//	MYSQL_RES *res = mysql_store_result(&mysql);
//	MYSQL_ROW row;
//	MYSQL_FIELD *fields;
//	unsigned int field_num;
//	ftdc_variable ftdc_var;
//
//
//	fields = mysql_fetch_fields(res);
//	field_num = mysql_num_fields(res);
//	vector<string> result_items = protocalif.get_result_items(tid_name);
//
//	for (int idx = 0; row = mysql_fetch_row(res); idx++) {
//		for (int i = 0; i < result_items.size() && i < field_num; i++) {
//			ftdc_path path(result_items[i]);
//			ftdc_var.set_field_item(path.fid_name(), idx, path.item_name(), row[i]);
//		}
//	}
//	while (row = mysql_fetch_row(res)) {
//		
//	}
//
//	ostringstream oss_out_param;
//	oss_out_param << "SELECT ";
//	offset = 0;
//	for each (string var in protocalif.get_output_items(tid_name)){
//		if (offset++)
//			oss << ",";
//		oss << "@" << var;
//	}
//
//	str = oss_out_param.str();
//	mysql_real_query(&mysql, str.data(), str.size());
//	res = mysql_store_result(&mysql);
//	row = mysql_fetch_row(res);
//
//	vector<string> out_items = protocalif.get_result_items(tid_name);
//
//	
//
//	for (int i = 0; i < out_items.size(); i++)
//		ftdc_var.set_field_item(out_items[i], row[i]);
//}