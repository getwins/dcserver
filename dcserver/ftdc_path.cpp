#include "ftdc_path.hpp"

#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

static std::regex ftdc_path_reg("(\\w+)\\[(\\d+)\\]\\.(\\w+)");
static std::regex ftdc_path_reg_short("(\\w+)\\.(\\w+)");

ftdc_path::ftdc_path(const std::string & ftdc_path_literal)
	: pathstr_(ftdc_path_literal)
	, fid_idx_(0)
	, invalid_(false)
{
	std::smatch what;
	if (std::regex_match(pathstr_, what, ftdc_path_reg)){
		fid_name_ = what[1];
		fid_idx_ = boost::lexical_cast<int>(what[2]);
		item_name_ = what[3];
		invalid_ = true;
	}else if (std::regex_match(pathstr_, what, ftdc_path_reg_short)) {

		fid_name_ = what[1];
		fid_idx_ = 0;
		item_name_ = what[2];
		invalid_ = true;
	}
}

ftdc_path::ftdc_path(const std::string & fid_name, unsigned int fid_idx, const std::string & item_name)
	: fid_name_(fid_name)
	, fid_idx_(fid_idx)
	, item_name_(item_name)
	, invalid_(true)
	, pathstr_(boost::str(boost::format("%s[%d].%s") % fid_name % fid_idx % item_name))
{
}

bool ftdc_path::invalid() const
{
	return invalid_;
}

std::string ftdc_path::str() const
{
	return pathstr_;
}

std::string ftdc_path::fid_name() const
{
	return fid_name_;
}

unsigned int ftdc_path::fid_idx() const
{
	return fid_idx_;
}

std::string ftdc_path::item_name() const
{
	return item_name_;
}

	
