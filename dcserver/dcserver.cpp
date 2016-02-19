// dcserver.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <thread> 
//#include <iostream>

#include <boost/program_options.hpp>
//#include <boost/bind.hpp>

#include "ftd.hpp"
#include "ftd_protocalif.hpp"
#include "functionif.hpp"
#include "dbconnpool.hpp"
#include "server.hpp"
#include "ftdc_content.hpp"


using namespace std;
using namespace boost;

ftdc_buffer_vec 
request_handle_fuc(const ftdc_buffer &ftdc_buffer_i, const ftd_protocalif &protocalif, const functionif &funcif, dbconnpool &dbconn)
{
	ftdc_buffer_vec buffers_o;
	ftdc_variable ftdc_var = parse_ftdc_content(ftdc_buffer_i, protocalif);
	string tidname = protocalif.get_tidname(ftdc_buffer_i.header()->tid);
	dbconn.invoke(tidname, protocalif, funcif, ftdc_var);

	buffers_o = serialize_ftdc_content(ftdc_buffer_i, ftdc_var, protocalif);
	return buffers_o;
}

int main(int argc, char *argv[])
{
	program_options::options_description cmd_desc("command line options");
	program_options::variables_map cmd_vm;

	string conf;
	string protocal;
	string function;

	cmd_desc.add_options()
		("conf", program_options::value(&conf)->default_value("conf.ini"), "ini configuration file")
		("protocal", program_options::value(&protocal)->default_value("ptorocal"), "ftd protocal sqlite file")
		("function", program_options::value(&function)->default_value("function"), "function sqlite file")
		;
	program_options::store(program_options::parse_command_line(argc, argv, cmd_desc), cmd_vm);
	cmd_vm.notify();

	program_options::options_description conf_desc("ini configuration options");
	program_options::variables_map conf_vm;

	string svr_address, svr_port;
	int svr_worker_num;

	string db_host, db_user, db_passwd, db_name;
	int db_port;

	conf_desc.add_options()
		("server.address", program_options::value(&svr_address)->implicit_value("127.0.0.1"), "dcserver listen address")
		("server.port", program_options::value(&svr_port)->implicit_value("12306"), "dcserver listen port")
		("server.worker", program_options::value(&svr_worker_num)->implicit_value(thread::hardware_concurrency()), "deserver worker thread pool size")
		("db.host", program_options::value(&db_host)->implicit_value("127.0.0.1"))
		("db.user", program_options::value(&db_user))
		("db.passwd", program_options::value(&db_passwd))
		("db.name", program_options::value(&db_name))
		("db.port", program_options::value(&db_port))
		;
	program_options::store(program_options::parse_config_file<char>(conf.c_str(), conf_desc, true), conf_vm);
	conf_vm.notify();

	ftd_protocalif protocalif(protocal);
	functionif funcif(function);
	dbconnpool dbconn(db_host, db_user, db_passwd, db_name, db_port);

	server svr(svr_address, svr_port, svr_worker_num, std::bind(request_handle_fuc, std::placeholders::_1, protocalif, funcif, dbconn));
	svr.run();

    return 0;
}

