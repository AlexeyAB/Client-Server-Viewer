/**
 * @file   connection.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef CONNECTION_HPP
#define CONNECTION_HPP
// ----------------------------------------------------------------------------
#include "handler_allocator.hpp"
#include "load_file.hpp"
// ----------------------------------------------------------------------------
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

namespace ba = boost::asio;
namespace bs = boost::system;
// ----------------------------------------------------------------------------


///
/// Class for handling connection in sync mode
/// 
///
class T_connection {

	/// 
	/// Constructor for class, initilize socket for this connection
	/// 
	/// @param file_name reference to file name of data that are sent to the client
	/// @param io_service reference to io_service of executors in which this connection will work
	/// @param timeout time in seconds for timeout of connection
	/// 
	/// @return nothing
	///
	T_connection(const std::string &file_name, ba::io_service& io_service, const unsigned int timeout);

public:
	~T_connection();

	typedef boost::shared_ptr<T_connection> T_shared_this;
		
	/// bind for clients handler with optimized memory allocation
	template<typename T_handler>
	inline T_custom_alloc_handler<T_handler> client_bind(T_handler binded_handler) {
		return make_custom_alloc_handler(client_allocator_, binded_handler);
	}

	/// Simple getter of pointer to this object
	//inline T_connection const*const get_this() const { return memorypool_shared_this_.get(); }

	/// 
	/// Create new connection, throught placement new 
	/// 
	/// @param file_name reference to file name of data that are sent to the client
	/// @param memory_pool_ptr shared pointer to the allocated memory poll for connections
	/// @param i_connect index of current connection in memory pool 
	/// @param io_service io_service in which this connection will work
	/// @param timeout time in seconds for timeout of connection
	/// @return pointer to newly allocated object
	///
	static inline T_connection *const T_connection::create(const std::string &file_name, 
														   T_connection *const memory_pool_raw_ptr, 
														   const size_t i_connect, 
														   ba::io_service& io_service,
														   const unsigned int timeout) 
	{
		return new (&memory_pool_raw_ptr[i_connect]) T_connection(file_name, io_service, timeout);
	}

	/// 
	/// Return socket, associated with this connection. This socket used in accept operation.
	/// 
	/// 
	/// @return reference to socket
	///
	inline ba::ip::tcp::socket& T_connection::socket() {
		return client_socket_;
	}

	/// 
	/// Perform all input/output operations in async mode:
	/// for a start try to connect to endpoint by endpoint_iterator
	/// 
	/// @param shared_this shared pointer of this (current connection)
	/// @param endpoint_iterator forward iterable object, that points to the connection endpoints of remote server
	///
	void run(T_shared_this shared_this);//, const ba::ip::tcp::resolver::iterator endpoint_iterator);

private:
	/// 
	/// Try to connect to the server
	/// 
	/// @param err reference to error code returned by async_connect
	/// @param endpoint_iterator forward iterable object, that points to the connection endpoints of remote server
	/// @param first_time flag that show that function launched at first time or no
	///
	void handle_connect(const boost::system::error_code& err,
									ba::ip::tcp::resolver::iterator endpoint_iterator, const bool first_time);

	/// 
	/// Reading data from a server to server_buffer_
	/// after yet another write data to the client
	///
	/// @param err 
	/// @param len length of data in bytes, that have been writen
	///
	void handle_write_to_client(const bs::error_code& err, const size_t len);

	/// 
	/// Writing data to the server
	/// after read them from a client to client_buffer_
	/// 
	/// @param err 
	/// @param len length of data in bytes, that have been read
	///
	void handle_read_from_client(const bs::error_code& err, const size_t len);

	/// 
	/// Close both sockets: for client and server
	/// 
	/// @param err 
	/// @param throw_place string with filename, its datetime, line number in file and function name which call this shutdown() function 
	///
	inline void shutdown(const bs::error_code& err, const std::string& throw_place);

	/// 
	/// Start deadline timer and check it
	/// 
	void check_deadline(const bs::error_code& err);

	enum { allocator_size = 1024 };         ///< size of buffer for handler allocator for storage boost::bind()

	uint64_t block_position;                ///< current position of block(page) in file
	uint64_t file_size;                     ///< size of file, which is transmitted to client
	T_load_file load_file;                  ///< object which loads file from file system by parts
	T_shared_this memorypool_shared_this_;  ///< shared pointer of this with memory-pool counter
	ba::io_service& io_service_;            ///< reference to io_service, in which work this connection
	ba::ip::tcp::socket client_socket_;     ///< socket, associated with client
	T_handler_allocator<allocator_size> client_allocator_;	///< allocator, to use for handler-based custom memory allocation for clients handlers
	volatile bool stopped_;									///< stoped event loop and the deadline-timer can destruct object
	const boost::posix_time::time_duration timeout_;		///< object, that contain time for deadline
	ba::deadline_timer deadline_;							///< object, that set deadline for async operations
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#endif // CONNECTION_HPP
