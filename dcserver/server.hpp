#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "ftdsession.hpp"

class server : private boost::noncopyable
{
public:
	explicit server(const std::string& address, const std::string& port, std::size_t thread_pool_size, req_handle_func_t handler);
	~server();

	/// Run the server's io_service loop.
	void run();

private:
	/// Initiate an asynchronous accept operation.
	void start_accept();

	/// Handle completion of an asynchronous accept operation.
	void handle_accept(const boost::system::error_code& e);

	/// Handle a request to stop the server.
	void handle_stop();

	/// The number of threads that will call io_service::run().
	std::size_t thread_pool_size_;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;

	/// The signal_set is used to register for process termination notifications.
	boost::asio::signal_set signals_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	/// The next connection to be accepted.
	//connection_ptr new_connection_;
	std::shared_ptr<ftdsession> new_connection_;

	/// The handler for all incoming requests.
	//request_handler request_handler_;
	req_handle_func_t handler_;
};

