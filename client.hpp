/**
 * @file   client.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef CLIENT_HPP
#define CLIENT_HPP
// ----------------------------------------------------------------------------
#include <boost/asio.hpp>
#include <boost/bind.hpp>
namespace ba = boost::asio;
#include <iostream>

#include "get_data.hpp"
// ----------------------------------------------------------------------------

/// Loads file from the server by parts
class T_client : public T_get_data {
private:
	ba::io_service& io_service_;							///< reference to io_service
	ba::ip::tcp::socket client_socket_;						///< socket, associated with client
	ba::ip::tcp::resolver::iterator remote_endpoint_it_;	///< object, that points to the connection endpoint of remote server
	ba::deadline_timer deadline_;							///< object, that set deadline for async operations
	boost::posix_time::time_duration timeout_;				///< object, that contain time for deadline
	std::string address_n_port_ ;							///< string, that contain address and port of the remote server

	/// Start deadline timer and check it
	void check_deadline();
public:
	/// Send index of page and receive page data
	virtual size_t load(const size_t i_block);

	/// Reposition on the different page and get data from this page from the server
	virtual size_t reposition_and_load(const int64_t delta_index);

	/// Initialize io_service and socket, resolve the server, try to connect and get file size
	T_client(ba::io_service& io_service, unsigned int remote_port, std::string remote_address,
			const unsigned int timeout = 1);

	/// Close socket
	virtual ~T_client() { 
		boost::system::error_code ignored_ec;
		client_socket_.close(ignored_ec);
	}

	/// Get a string, that contain address and port of the remote server
	virtual std::string const& get_source() const {
		return address_n_port_;
	}
};

// ----------------------------------------------------------------------------
#endif // CLIENT_HPP