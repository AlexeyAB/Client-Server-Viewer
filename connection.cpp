/**
 * @file   connection.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief PortMapping Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#include "connection.hpp"

// ----------------------------------------------------------------------------
#include <iostream>
#include <boost/move/move.hpp>
#include <boost/bind.hpp>
// ----------------------------------------------------------------------------
#include "seh_exception.hpp"
#include <boost/system/system_error.hpp>
// ----------------------------------------------------------------------------

	/// 
	/// Constructor for class, initilize socket for this connection
	/// 
	/// @param file_name reference to file name of data that are sent to the client
	/// @param io_service reference to io_service of executors in which this connection will work
	/// @param timeout time in seconds for timeout of connection
	/// 
	/// @return nothing
	///
	T_connection::T_connection(const std::string &file_name, ba::io_service& io_service, const unsigned int timeout) :
		io_service_(io_service), client_socket_(io_service)
		, load_file(file_name), deadline_(io_service), timeout_(boost::posix_time::seconds(timeout)), stopped_(false)
	{
		std::cout << "T_connection() \n";

		// start deadline timer
		check_deadline(bs::error_code());	
	}

	T_connection::~T_connection() { std::cout << "~T_connection() \n"; }
	// ----------------------------------------------------------------------------

	/// 
	/// Perform all input/output operations in async mode:
	/// for a start try to connect to endpoint by endpoint_iterator
	/// 
	/// @param shared_this shared pointer of this (current connection)
	///
	void T_connection::run(T_shared_this shared_this) {
		// try/catch and then output to std::cerr exception message .what()
		try {
			memorypool_shared_this_ = boost::move(shared_this);
			file_size = load_file.get_file_size();		// may be used htnoll/ntohll
			
			deadline_.expires_from_now(timeout_);
			ba::async_write(client_socket_, ba::buffer(&file_size, sizeof(file_size)),
							client_bind(boost::bind(&T_connection::handle_write_to_client, this,
													ba::placeholders::error,
													ba::placeholders::bytes_transferred)) );
		} catch(const seh::T_seh_exception& e) {
			std::cerr << "T_seh_exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
			shutdown(boost::system::error_code(), "");
		} catch(const bs::system_error& e) {
			std::cerr << "Boost system_error exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
			shutdown(boost::system::error_code(), "");
		} catch(const std::exception &e) {
			std::cerr << "Exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
			shutdown(boost::system::error_code(), "");
		} catch(...) {
			std::cerr << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
			shutdown(boost::system::error_code(), "");
		}			
	}
	// ----------------------------------------------------------------------------

	/// 
	/// Reading data from a server to server_buffer_
	/// after yet another write data to the client
	///
	/// @param err 
	/// @param len length of data in bytes, that have been writen
	///
	void T_connection::handle_write_to_client(const bs::error_code& err, const size_t len) {
		//std::cout << "handle_write_to_client, len= " << len << ", eof is: " << (err == ba::error::eof) << std::endl;
		if(!err) {
			deadline_.expires_from_now(timeout_);
			ba::async_read(client_socket_, ba::buffer(&block_position, sizeof(block_position)),
					   client_bind(boost::bind(&T_connection::handle_read_from_client, this,
											   ba::placeholders::error,
											   ba::placeholders::bytes_transferred)) );
		} else {
			shutdown(err, THROW_PLACE);
		}
	}
	// ----------------------------------------------------------------------------

	/// 
	/// Writing data to the server
	/// after read them from a client to client_buffer_
	/// 
	/// @param err 
	/// @param len length of data in bytes, that have been read
	///
	void T_connection::handle_read_from_client(const bs::error_code& err, const size_t len) {
		//std::cout << "handle_read_from_client, len= " << len << ", eof is: " << (err == ba::error::eof) << std::endl;
		if(!err)
		{
			const size_t read_size = load_file.load(block_position);
			
			deadline_.expires_from_now(timeout_);
			ba::async_write(client_socket_, ba::buffer(load_file.get_buffer(), T_load_file::c_block_size),
							client_bind(boost::bind(&T_connection::handle_write_to_client, this,
													ba::placeholders::error,
													ba::placeholders::bytes_transferred)) );
		} else {
			shutdown(err, THROW_PLACE);
		}
	}


	/// 
	/// Close both sockets: for client and server
	/// 
	/// @param err 
	/// @param throw_place string with filename, its datetime, line number in file and function name which call this shutdown() function 
	///
	inline void T_connection::shutdown(const bs::error_code& err, const std::string& throw_place) {
		// try/catch and then output to std::cerr exception message .what()
		try {
			std::cout << "shutdown \n";
			stopped_ = true;
			boost::system::error_code ignored_ec;
			deadline_.cancel(ignored_ec);
			//client_socket_.cancel(ignored_ec);
		} catch(const seh::T_seh_exception& e) {
			std::cerr << "T_seh_exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(const bs::system_error& e) {
			std::cerr << "Boost system_error exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(const std::exception &e) {
			std::cerr << "Exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(...) {
			std::cerr << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
		}	
	}
	// ----------------------------------------------------------------------------

	/// 
	/// Start deadline timer and check it
	/// 
	void T_connection::check_deadline(const bs::error_code& err) {
		try {
			// check that the connection was stopped or whether the deadline has passed
			if(!stopped_ && deadline_.expires_at() > ba::deadline_timer::traits_type::now()) {
				// what kind of error we got
				if(!err || err == ba::error::operation_aborted) {
					// Put the actor back to sleep
					deadline_.async_wait(boost::bind(&T_connection::check_deadline, this, ba::placeholders::error));
					return;
				}
				// serious error
				std::cerr << bs::system_error(err).what() << std::endl;
			}
			boost::system::error_code ignored_ec;
			client_socket_.close(ignored_ec);		// The deadline has passed. Stop IO-operations.
			memorypool_shared_this_.reset();		// object can destruct
		} catch(...) {
			std::cerr << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
		}
	}
	// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
