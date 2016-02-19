#include "ftdsession.hpp"

//#include <memory>
#include <boost/bind.hpp>
//using namespace std;
//using namespace boost;

ftdsession::ftdsession(boost::asio::io_service & io_service, req_handle_func_t handler)
	: strand_(io_service)
	, socket_(io_service)
	, handler_(handler)
{

}

//ftdsession::ftdsession(boost::asio::io_service & io_service)
//	: strand_(io_service)
//	, socket_(io_service)
//	//, handler_(handler)
//{
//
//}

boost::asio::ip::tcp::socket & 
ftdsession::socket()
{
	return socket_;
}


void 
ftdsession::start()
{
	memset(&ftd_header_, 0, sizeof(ftd_header_));
	memset(&ftd_extend_header_, 0, sizeof(ftd_extend_header_));
	ftdc_buffer_.reset();
	rsp_ftdc_buffers_.clear();
	rsp_index_ = 0;

	boost::asio::async_read(socket_,
		boost::asio::buffer(&ftd_header_, sizeof(ftd_header_)),
		strand_.wrap(boost::bind(&ftdsession::handle_read_ftd_header, shared_from_this(), boost::asio::placeholders::error)));
}

void
ftdsession::handle_read_ftd_header(const boost::system::error_code &e)
{
	if (!e) {
		boost::asio::async_read(socket_,
			boost::asio::buffer(&ftd_extend_header_, ftd_header_.extend_length),
			strand_.wrap(boost::bind(&ftdsession::handle_read_ftd_extendheader, shared_from_this(), boost::asio::placeholders::error)));

		//if (ftd_header_.extend_length) {
		//	boost::asio::async_read(socket_,
		//		boost::asio::buffer(&ftd_extend_header_, ftd_header_.extend_length),
		//		strand_.wrap(bind(&ftdsession::handle_read_ftd_extendheader, shared_from_this(), boost::asio::placeholders::error)));
		//}else {
		//	//boost::asio::async_read(socket_,
		//	//	boost::asio::buffer(ftdc_buff_, ftd_header_.ftdc_length),
		//	//	strand_.wrap(bind(&ftdsession::handle_read_ftdc_header, shared_from_this(), boost::asio::placeholders::error)));
		//	boost::asio::async_read(socket_,
		//		boost::asio::buffer(ftdc_buffer_.data(), ftd_header_.ftdc_length),
		//		strand_.wrap(bind(&ftdsession::handle_read_ftdc_header, shared_from_this(), boost::asio::placeholders::error)));
		//}
	}
}

void 
ftdsession::handle_read_ftd_extendheader(const boost::system::error_code &e)
{
	if (!e) {
		//boost::asio::async_read(socket_,
		//	//boost::asio::buffer(ftdc_buff_, ftd_header_.ftdc_length),
		//	boost::asio::buffer(&ftdc_header_, sizeof(ftdc_header_)),
		//	strand_.wrap(bind(&ftdsession::handle_read_ftdc_header, shared_from_this(), boost::asio::placeholders::error)));

		boost::asio::async_read(socket_,
			boost::asio::buffer(ftdc_buffer_.data(), ftd_header_.ftdc_length),
			strand_.wrap(boost::bind(&ftdsession::handle_read_ftdc, shared_from_this(), boost::asio::placeholders::error)));
	}
}

//void
//ftdsession::handle_read_ftdc_header(const boost::system::error_code & e)
//{
//	if (!e) {
//		if (ftd_header_.ftdc_length != ftdc_header_.ftdc_content_length + sizeof(ftdc_header_)) {
//			socket_.close();
//
//		}else {
//			//ftdc_prev_header_ = ftdc_header_;
//			boost::asio::async_read(socket_,
//				boost::asio::buffer(ftdc_buff_, ftdc_header_.ftdc_content_length),
//				strand_.wrap(bind(&ftdsession::handle_read_ftdc_content, shared_from_this(), boost::asio::placeholders::error)));
//		}
//	}
//}

//#include "ftd_protocalif.hpp"
//#include "ftdc_content.hpp"
//#include "dbconnpool.hpp"
//#include "functionif.hpp"
//
//extern ftd_protocalif g_protocal;
//extern dbconnpool g_dbconn;
//extern functionif g_funcif;
//
//void 
//ftdsession::handle_read_ftdc_content(const boost::system::error_code & e)
//{
//	if (!e) {
//		if(ftdc_header_.chain == FTDC_CHAIN_SINGLE) {
//
//			strvec_t ret = handler_(ftdc_header_, ftdc_buff_);
//			if (ret.size() == 1) {
//				//ftdc_header_
//			}
//
//			//ftdc_variable ftdc_var = parse_ftdc_content(ftdc_header_, ftdc_buff_, g_protocal);
//			//string tidname = g_protocal.get_tidname(ftdc_header_.tid);
//			//g_dbconn.invoke(tidname, g_protocal, g_funcif, ftdc_var);
//			//std::vector<std::string> ret = serialize_ftdc_content(ftdc_header_, ftdc_var, g_protocal);
//			//strvec_t ret = handler_(ftdc_header_, ftdc_buff_);
//
//		}/*else if (ftdc_header_.chain == FTDC_CHAIN_FIRST) {
//			ftdc_prev_header_ = ftdc_header_;
//			ftdc_content_.append(ftdc_buff_, ftdc_header_.ftdc_content_length);
//
//			boost::asio::async_read(socket_,
//				boost::asio::buffer(&ftd_header_, sizeof(ftd_header_)),
//				strand_.wrap(bind(&ftdsession::handle_read_ftdc_header, shared_from_this(), boost::asio::placeholders::error)));
//		}
//		else if (ftdc_header_.chain == FTDC_CHAIN_MIDDLE) {
//
//			if (!(ftdc_header_.seqno == ftdc_prev_header_.seqno && ftdc_header_.seris == ftdc_prev_header_.seris)) {
//				socket_.close();
//				return;
//			}
//
//			ftdc_prev_header_.field_count += ftdc_header_.field_count;
//			ftdc_prev_header_.ftdc_content_length += ftdc_header_.ftdc_content_length;
//			ftdc_content_.append(ftdc_buff_, ftdc_header_.ftdc_content_length);
//			boost::asio::async_read(socket_,
//				boost::asio::buffer(&ftd_header_, sizeof(ftd_header_)),
//				strand_.wrap(bind(&ftdsession::handle_read_ftdc_header, shared_from_this(), boost::asio::placeholders::error)));
//		}
//		else if(ftdc_header_.chain == FTDC_CHAIN_LAST){
//
//			if (!(ftdc_header_.seqno == ftdc_prev_header_.seqno && ftdc_header_.seris == ftdc_prev_header_.seris)) {
//				socket_.close();
//				return;
//			}
//			ftdc_prev_header_.chain = FTDC_CHAIN_LAST;
//			ftdc_prev_header_.field_count += ftdc_header_.field_count;
//			ftdc_prev_header_.ftdc_content_length += ftdc_header_.ftdc_content_length;
//			ftdc_content_.append(ftdc_buff_, ftdc_header_.ftdc_content_length);
//
//		}*/else {
//			socket_.close();
//		}
//
//
//	}
//}

void 
ftdsession::handle_read_ftdc(const boost::system::error_code & e)
{
	if (!e) {
		if (ftd_header_.ftdc_length != ftdc_buffer_.size())
			socket_.close();
		else {
			rsp_ftdc_buffers_ = handler_(ftdc_buffer_);

			//if (ret.size() == 1) {
			//	//ftdc_buffer &buffer = ret[0];
			//	ftdc_buffer_ = ret[0];
			//	ftdc_buffer_.header()->chain = FTDC_CHAIN_SINGLE;
			//	//ftdc_buffer_.header()->
			//	ftd_header_.ftdc_length = ftdc_buffer_.size();
			//	boost::asio::async_write(socket_,
			//		boost::asio::buffer(&ftd_header_, sizeof(ftd_header_)),
			//		strand_.wrap(boost::bind(&ftdsession::handle_write_ftd_header, shared_from_this(), boost::asio::placeholders::error)));
			//}else if (ret.size() > 1) {
			//	for (auto it = ret.begin(); it != ret.end(); ++it) {

			//	}
			//}
		}
	}
}

void 
ftdsession::response()
{
	if (rsp_ftdc_buffers_.size() == 1) {
		ftdc_buffer_ = rsp_ftdc_buffers_[0];
		ftdc_buffer_.header()->chain = FTDC_CHAIN_SINGLE;
	}else if (rsp_ftdc_buffers_.size() > 1) {
		ftdc_buffer_ = rsp_ftdc_buffers_[rsp_index_];

		if (rsp_index_ == 0) 
			ftdc_buffer_.header()->chain = FTDC_CHAIN_FIRST;
		else if (rsp_index_ == rsp_ftdc_buffers_.size() - 1)
			ftdc_buffer_.header()->chain = FTDC_CHAIN_LAST;
		else
			ftdc_buffer_.header()->chain = FTDC_CHAIN_MIDDLE;

		rsp_index_++;

		boost::asio::async_write(socket_,
			boost::asio::buffer(&ftd_header_, sizeof(ftd_header_)),
			strand_.wrap(boost::bind(&ftdsession::handle_write_ftd_header, shared_from_this(), boost::asio::placeholders::error)));
	}
}

void
ftdsession::handle_write_ftd_header(const boost::system::error_code & e)
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(&ftd_extend_header_, ftd_header_.extend_length),
		strand_.wrap(boost::bind(&ftdsession::handle_write_ftd_extendheader, shared_from_this(), boost::asio::placeholders::error)));
}

void 
ftdsession::handle_write_ftd_extendheader(const boost::system::error_code & e)
{
	ftdc_header_st *ftdc_header = ftdc_buffer_.header();
	//ftdc_header->chain =
	boost::asio::async_write(socket_,
		boost::asio::buffer(ftdc_buffer_.data(), ftd_header_.ftdc_length),
		strand_.wrap(boost::bind(&ftdsession::handle_write_ftd_extendheader, shared_from_this(), boost::asio::placeholders::error)));
}

void 
ftdsession::handle_write_ftdc(const boost::system::error_code & e)
{
	if (ftdc_buffer_.header()->chain == FTDC_CHAIN_SINGLE || ftdc_buffer_.header()->chain == FTDC_CHAIN_LAST)
		start();
	else
		response();
}

//void 
//ftdsession::handle_request(const ftdc_header_st & ftdc_header, const void * ftdc_content)
//{
//	//strvec_t ret = handler_(ftdc_header, ftdc_content);
//
//}



