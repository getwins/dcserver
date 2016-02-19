#ifndef _SQLITEHELPER_CHP_HPP_

#define _SQLITEHELPER_CHP_HPP_

//  (C) Copyright huiping.chen 2015.

#include <stdexcept>
#include <string>
#include <boost/throw_exception.hpp>

#include <sqlite3.h>

struct sqlite_error : public std::runtime_error {
	sqlite_error(const std::string &message) : std::runtime_error (message){}
};

#define SQLITE_CHECK_INVOKE(db, call) \
do{\
	if(call != SQLITE_OK) \
		BOOST_THROW_EXCEPTION(sqlite_error(sqlite3_errmsg(db))); \
} while(0)

#define SQLITE_CHECK_INVOKE2(err, call) \
do { \
	int ret = (call); \
	if ((err) != ret){ \
		BOOST_THROW_EXCEPTION(sqlite_error(sqlite3_errstr(ret))); \
	} \
} while (0)

#define SQLITE_CHECK_INVOKE1(call) SQLITE_CHECK_INVOKE2(SQLITE_OK, call)

#endif