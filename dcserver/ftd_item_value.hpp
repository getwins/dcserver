#pragma once
#include <memory>
#include <sstream>
#include <iomanip>
#include <string>

#include "ftd.hpp"


class ftd_item_value
{
public:
	virtual const void *data() = 0;
	virtual int size() = 0;

	virtual std::string str() = 0;
//protected:
	ftd_item_value() {};
	virtual ~ftd_item_value() {};
};

class ftd_item_char_value : public ftd_item_value
{
public:
	ftd_item_char_value(const char *c)
		: c_(*c){}

	virtual const void *data() { return &c_; };
	virtual int size() { return 1; }

	virtual std::string str() {
		std::ostringstream oss;
		oss << c_;
		return oss.str();
	}
private:
	char c_;
};

class ftd_item_float_value : public ftd_item_value
{
public:
	ftd_item_float_value(const char *p, int length, int precision)
	: f_(0.0)
	, length_(length)
	, precision_(precision)
	, data_(NULL)
	{
		std::istringstream iss(p);
		//iss >> std::internal >> std::setw(length_) >> f_;
		iss >> std::internal >> std::setw(length_) >> f_;
	}

	virtual ~ftd_item_float_value() {
		if (data_)
			delete data_;
	}

	virtual const void *data() {
		if (!data_)
			data_ = new char[length_];
		std::ostringstream oss;
		oss 
			//<< std::setiosflags(std::ios::right)
			<< std::internal
			<< std::setfill('0')
			<< std::setw(length_) 
			<< std::fixed << std::setprecision(precision_)		
			<< f_;
		memcpy(data_, oss.str().data(), length_);
		return data_;
	}

	virtual int size() { return length_; }

	virtual std::string str() { 
		std::ostringstream oss;
		oss << f_;
		return oss.str(); 
	}

private:
	float f_;
	int length_;
	int precision_;
	char *data_;
};

class ftd_item_int_value : public ftd_item_value
{
public:
	ftd_item_int_value(const char *p)
		: i_(0)
	{
		std::istringstream iss(p);
		iss >> i_;
	}

	ftd_item_int_value(int i)
		: i_(i)
	{}

	virtual const void *data() { return &i_; }
	virtual int size() { return 4; }
	virtual std::string str() {
		std::ostringstream oss;
		oss << i_;
		return oss.str();
	}
private:
	int i_;
};

class ftd_item_number_value : public ftd_item_value
{
public:
	ftd_item_number_value(const char *p, int length)
		: i_(0)
		, length_(length)
		, data_(NULL)
	{
		std::istringstream iss(std::string(p, length_));
		iss >> i_;
	}

	virtual ~ftd_item_number_value(){
		if (data_)
			delete data_;
	}

	virtual const void *data() {
		if (!data_)
			data_ = new char[length_];
		std::stringstream oss;
		oss << std::internal << std::setfill('0') << std::setw(length_) << i_;
		oss.str().copy(data_, length_);
		//return oss.str().c_str();
		return data_;
	}

	virtual int size() { return length_; }

	virtual std::string str() {
		std::stringstream oss;
		oss << i_;
		return oss.str();
	}

private:
	int i_;
	int length_;
	char *data_;
};

class ftd_item_string_value : public ftd_item_value
{
public:
	ftd_item_string_value(const char *p, int length)
		: length_(length)
		//, data_(new char[length + 1])
		//, s_(p, std::min(static_cast<size_t>(length), strlen(p)))
		, data_(NULL)
	{
		std::istringstream iss(std::string(p, std::min(static_cast<size_t>(length_), strlen(p))));
		//iss >> std::setw(length) >> std::setfill(' ') >> s_;
		iss >> s_;
		//length_ = length; 
		//strcpy(data_, p);
	}

	virtual ~ftd_item_string_value() {
		if (data_)
			delete data_;
	}

	const void *data(){
		if (!data_)
			data_ = new char[length_];
		std::ostringstream oss;
		oss << std::left << std::setw(length_) << std::setfill(' ') << s_;
		//return oss.str().c_str();
		oss.str().copy(data_, length_);
		return data_;
	}

	int size() { return length_; }

	virtual std::string str() {
		std::stringstream oss;
		oss << std::noskipws << '\'' << s_ << '\'';
		return oss.str();
	}
private:
	int length_;
	std::string s_;
	char *data_;
};

class ftd_item_word_value : public ftd_item_value
{
public:
	ftd_item_word_value(const char *p)
		:us_(0)
	{
		std::istringstream iss(p);
		iss >> us_;
	}
	ftd_item_word_value(unsigned short us)
		: us_(us)
	{}

	virtual const void *data() { return &us_; }
	virtual int size() { return 2; }

	virtual std::string str() {
		std::ostringstream oss;
		oss << us_;
		return oss.str();
	}
private:
	unsigned short us_;
};

typedef std::shared_ptr<ftd_item_value> itemvalue_ptr;

inline ftd_item_type_t
get_ftd_item_type(const std::string &basetype) {
	if (basetype == "FTDCharType")
		return ftd_char_type;
	else if (basetype.find("FTDFloatType") != std::string::npos)
		return ftd_float_type;
	else if (basetype == "FTDIntType")
		return ftd_int_type;
	else if (basetype.find("FTDNumberType") != std::string::npos)
		return ftd_number_type;
	else if (basetype.find("FTDStringType") != std::string::npos)
		return ftd_string_type;
	else if (basetype == "FTDWordType")
		return ftd_word_type;
	else
		return ftd_unknown_type;
}

inline ftd_item_value *
create_ftd_item_value_from_data(ftd_item_type_t type, const void *data, int length, int precision)
{
	if (type == ftd_char_type)
		return new ftd_item_char_value(reinterpret_cast<const char*>(data));
	else if (type == ftd_float_type)
		return new ftd_item_float_value(reinterpret_cast<const char*>(data), length, precision);
	else if (type == ftd_int_type)
		return new ftd_item_int_value(*(reinterpret_cast<const int*>(data)));
	else if (type == ftd_number_type)
		return new ftd_item_number_value(reinterpret_cast<const char*>(data), length);
	else if (type == ftd_string_type)
		return new ftd_item_string_value(reinterpret_cast<const char*>(data), length);
	else if (type == ftd_word_type)
		return new ftd_item_word_value(*(reinterpret_cast<const unsigned short *>(data)));
	else
		return NULL;
}

inline ftd_item_value*
create_ftd_item_value_from_str(ftd_item_type_t type, const char *data, int length, int precision)
{
	if (type == ftd_char_type)
		return new ftd_item_char_value(data);
	else if (type == ftd_float_type)
		return new ftd_item_float_value(data, length, precision);
	else if (type == ftd_int_type)
		return new ftd_item_int_value(data);
	else if (type == ftd_number_type)
		return new ftd_item_number_value(data, length);
	else if (type == ftd_string_type)
		return new ftd_item_string_value(data, length);
	else if (type == ftd_word_type)
		return new ftd_item_word_value(data);
	else
		return NULL;
}