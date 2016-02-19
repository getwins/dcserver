#ifndef _FTDSESSION_CHP_HPP_
#define _FTDSESSION_CHP_HPP_
//  (C) Copyright huiping.chen 2015.

//#include <string>
//#include <vector>

#include <boost/asio.hpp>
//#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <functional>

#include "ftd.hpp"
#include "ftdc_buffer.hpp"

//std::enable_shared_from_this
//  typedef std::vector<std::string> strvec_t;
//typedef std::function<strvec_t (const ftdc_header_st &, const void *)> handle_func_t;

typedef std::function<ftdc_buffer_vec (const ftdc_buffer&)> req_handle_func_t;

class ftdsession
	//: public boost::enable_shared_from_this<ftdsession>
	: public std::enable_shared_from_this<ftdsession>
	, private boost::noncopyable
{
public:
	explicit ftdsession(boost::asio::io_service &io_service, req_handle_func_t handler);
	//explicit ftdsession(boost::asio::io_service &io_service);

	/// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket();

	/// Start the first asynchronous operation for the connection.
	void start();

private:
	void handle_read_ftd_header(const boost::system::error_code& e);
	void handle_read_ftd_extendheader(const boost::system::error_code& e);
	
	//void handle_read_ftdc_header(const boost::system::error_code &e);
	//void handle_read_ftdc_content(const boost::system::error_code &e);
	void handle_read_ftdc(const boost::system::error_code &e);

	//void handle_request(const ftdc_header_st &ftdc_header, const void *ftdc_content);
	void response();
	void handle_write_ftd_header(const boost::system::error_code &e);
	void handle_write_ftd_extendheader(const boost::system::error_code &e);
	void handle_write_ftdc(const boost::system::error_code &e);

private:
	/// Strand to ensure the connection's handlers are not called concurrently.
	boost::asio::io_service::strand strand_;

	/// Socket for the connection.
	boost::asio::ip::tcp::socket socket_;
	req_handle_func_t handler_;

	ftd_header_st ftd_header_;
	ftd_extend_header_st ftd_extend_header_;
	//ftdc_header_st ftdc_prev_header_;
	//ftdc_header_st ftdc_header_;
	//char ftdc_buff_[FTDC_MAX_LENGTH];
	ftdc_buffer ftdc_buffer_;
	
	ftdc_buffer_vec rsp_ftdc_buffers_;
	int rsp_index_;
	//std::string ftdc_content_;
	
};

#endif
