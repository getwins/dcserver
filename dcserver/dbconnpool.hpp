#pragma once

#include <string>
#include <vector>
#include <exception>

#include "ftdc_variable.hpp"
#include "functionif.hpp"
#include "ftd_protocalif.hpp"

struct db_error : public std::runtime_error {
	db_error(const std::string &message) : std::runtime_error(message){}
};

class dbconnpool
{
public:
	dbconnpool(const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, unsigned int port);
	~dbconnpool();

	void invoke(const std::string &tidname, const ftd_protocalif &protocalif, const functionif &funcif, ftdc_variable &ftdc_var);
private:
	std::string host_; 
	std::string user_;
	std::string passwd_;
	std::string db_;
	unsigned int port_;

	//std::vector
};

