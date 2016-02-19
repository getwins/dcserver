#ifndef _FTDC_VARIABLE_CHP_HPP_
#define _FTDC_VARIABLE_CHP_HPP_

//  (C) Copyright huiping.chen 2015.

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "ftdc_path.hpp"
#include "ftd_item_value.hpp"

class ftdc_variable
{
public:
	//typedef std::shared_ptr<>
	//ftdc_variable();
	//~ftdc_variable();

	//void set_field_item(const std::string &fid_name, unsigned int fid_idx, const std::string &item_name, const std::string &item_value);
	//void set_field_item(const ftdc_path &ftdc_path, const std::string &value);
	//std::string get_field_item(const std::string &fid_name, unsigned int fid_idx, const std::string &item_name) const;
	//std::string get_field_item(const ftdc_path &ftdc_path) const;

	void set_field_item(const std::string &fid_name, size_t fid_idx, const std::string &item_name, itemvalue_ptr &item_value) 
	{
		field_vector_t &fv = val_[fid_name];
		if (fid_idx >= fv.size()) {
			for (size_t i = fv.size(); i < fid_idx + 1; ++i)
				fv.push_back(item_map_t());
		}
		fv[fid_idx][item_name] = item_value;
	}

	void set_field_item(const ftdc_path &ftdc_path, itemvalue_ptr &value) 
	{
		return set_field_item(ftdc_path.fid_name(), ftdc_path.fid_idx(), ftdc_path.item_name(), value);
	}

	itemvalue_ptr get_field_item(const std::string &fid_name, size_t fid_idx, const std::string &item_name) const
	{
		field_vector_t fv = val_.find(fid_name)->second;
		if (fid_idx >= fv.size()) {
			return itemvalue_ptr();
		}
		else {
			item_map_t &im = fv[fid_idx];
			item_map_t::iterator it = im.find(item_name);
			//return it->second;
			if (it != im.end())
				return it->second;
		}
		return itemvalue_ptr();
	}

	itemvalue_ptr get_field_item(const ftdc_path &ftdc_path) const
	{
		return get_field_item(ftdc_path.fid_name(), ftdc_path.fid_idx(), ftdc_path.item_name());
	}

	size_t field_count(const std::string &fid_name) const
	{
		auto it = val_.find(fid_name);
		if (it != val_.end())
			return it->second.size();
		return 0;
	}

	size_t count() const
	{
		return val_.size();
	}

private:
	//typedef std::map<std::string, std::string> item_map_t;
	typedef std::map<std::string, itemvalue_ptr> item_map_t;
	typedef std::vector<item_map_t> field_vector_t;
	typedef std::map<std::string, field_vector_t> field_map_t;

	field_map_t val_;
};

#endif