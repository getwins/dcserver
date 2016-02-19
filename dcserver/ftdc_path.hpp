#ifndef _FTDC_PATH_CHP_HPP_
#define _FTDC_PATH_CHP_HPP_

//  (C) Copyright huiping.chen 2015.

#include <string>
#include <regex>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

class ftdc_path
{
public:
	ftdc_path(const std::string &ftdc_path_literal)
		: pathstr_(ftdc_path_literal)
		, fid_idx_(0)
		, invalid_(false)
	{
		static std::regex ftdc_path_reg("(\\w+)\\[(\\d+)\\]\\.(\\w+)");
		static std::regex ftdc_path_reg_short("(\\w+)\\.(\\w+)");

		std::smatch what;
		if (std::regex_match(pathstr_, what, ftdc_path_reg)) {
			fid_name_ = what[1];
			fid_idx_ = boost::lexical_cast<int>(what[2]);
			item_name_ = what[3];
			invalid_ = true;
		}
		else if (std::regex_match(pathstr_, what, ftdc_path_reg_short)) {

			fid_name_ = what[1];
			fid_idx_ = 0;
			item_name_ = what[2];
			invalid_ = true;
		}
	}
	

	ftdc_path(const std::string &fid_name, unsigned int fid_idx, const std::string &item_name)
		: fid_name_(fid_name)
		, fid_idx_(fid_idx)
		, item_name_(item_name)
		, invalid_(true)
		, pathstr_(boost::str(boost::format("%s[%d].%s") % fid_name % fid_idx % item_name))
	{
	}

	bool invalid() const
	{
		return invalid_;
	}

	std::string str() const
	{
		return pathstr_;
	}

	std::string fid_name() const
	{
		return fid_name_;
	}

	unsigned int fid_idx() const
	{
		return fid_idx_;
	}

	std::string item_name() const
	{
		return item_name_;
	}

private:
	std::string pathstr_;
	std::string fid_name_;
	unsigned int fid_idx_;
	std::string item_name_;
	bool invalid_;

};

#endif