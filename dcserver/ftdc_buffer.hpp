#pragma once

#include "ftd.hpp"
#include <vector>

class ftdc_buffer
{
public:
	ftdc_buffer() {
		memset(buff_, 0, sizeof(buff_));
	}

	ftdc_buffer(ftdc_buff_t buff) {
		memcpy(buff_, buff, sizeof(ftdc_buff_t));
	}

	void reset() {
		memset(buff_, 0, sizeof(buff_));
	}

	char *data() {
		return buff_;
	}

	ftdc_header_st *header() {
		return reinterpret_cast<ftdc_header_st *>(buff_);
	}

	const ftdc_header_st *header() const{
		return reinterpret_cast<const ftdc_header_st *>(buff_);
	}

	char *content() {
		return reinterpret_cast<char *>(buff_ + sizeof(ftdc_header_st));
	}

	const char *content() const {
		return reinterpret_cast<const char *>(buff_ + sizeof(ftdc_header_st));
	}

	unsigned short content_length() const {
		return header()->ftdc_content_length;
	}

	unsigned short field_count() const {
		return header()->field_count;
	}

	bool add_field(const ftdc_field_st &field) {
		int fieldsize = ftdc_field_entity_size(field);
		if (header()->ftdc_content_length + fieldsize <= FTDC_CONTENT_MAX_LENGTH) {
			memcpy(content() + content_length(), &field, fieldsize);
			header()->field_count += 1;
			header()->ftdc_content_length += fieldsize;		
			return true;
		}
		return false;
	}

	unsigned short size() const {
		return sizeof(ftdc_header_st) + content_length();
	}
	//ftdc_field_st *fetch_field() {

	//}

	const ftdc_field_st *get_field(unsigned short index) const{
		if (index >= field_count())
			return NULL;
		//ftdc_field_st *f = content(); 
		const char *p = content();
		for (unsigned short i = 0; i < index; ++i)
			p += ftdc_field_entity_size(*(reinterpret_cast<const ftdc_field_st*>(p)));
		return reinterpret_cast<const ftdc_field_st*>(p);
	}

	ftdc_field_st *get_field(unsigned short index) {
		if (index >= field_count())
			return NULL;
		char *p = content();
		for (unsigned short i = 0; i < index; ++i)
			p += ftdc_field_entity_size(*(reinterpret_cast<ftdc_field_st*>(p)));
		return reinterpret_cast<ftdc_field_st*>(p);
	}

private:
	ftdc_buff_t buff_;

};

typedef std::vector<ftdc_buffer> ftdc_buffer_vec;