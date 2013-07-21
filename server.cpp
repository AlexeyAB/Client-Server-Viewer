/**
 * @file   server.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Server
 *
 *
 */
// ----------------------------------------------------------------------------
#include "server.hpp"
#include "seh_exception.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
// ----------------------------------------------------------------------------

/// 
/// Initialize all needed data
/// 
/// @param file_name to file of data that are sent to the client
/// @param io_service reference to io_service
/// @param thread_num_acceptors number of threads in thread pool for acceptors
/// @param thread_num_executors number of threads in thread pool for executors
/// @param local_port port to listen on, by default - 10001
/// @param local_interface_address local interface address to listen on
///
T_server::T_server(const std::string file_name,
				   ba::io_service& io_service_acceptors, ba::io_service& io_service_executors, 
				   unsigned int thread_num_acceptors, unsigned int thread_num_executors, 
				   unsigned int local_port, std::string local_interface_address,
				   const unsigned int timeout)
	: file_name_(file_name),
	  io_service_acceptors_(io_service_acceptors),
	  io_service_executors_(io_service_executors),
	  work_acceptors_(io_service_acceptors_),
	  work_executors_(io_service_executors_),
	  local_endpoint_(local_interface_address.empty()?	
				(ba::ip::tcp::endpoint(ba::ip::tcp::v4(), local_port)): // INADDR_ANY for v4 (in6addr_any if the fix to v6)
				ba::ip::tcp::endpoint(ba::ip::address().from_string(local_interface_address), local_port) ),   // specified ip address
	  acceptor_(io_service_acceptors_, local_endpoint_),                // By default set option to reuse the address (i.e. SO_REUSEADDR)
	  timeout_(timeout)
{
	std::clog << "Start listener: " << local_endpoint_ << std::endl << std::endl;

	// create threads in pool for executors
	for(size_t i = 0; i < thread_num_executors; ++i)
		thr_grp_executors_.emplace_back(boost::bind(&boost::asio::io_service::run, &io_service_executors_));

	// create threads in pool and start acceptors
	for(size_t i = 0; i < thread_num_acceptors; ++i) {
		// create threads in pool
		if(i != 0)	// one main thread already in pool from: int main() { ... io_service_acceptors.run(); ... }
			thr_grp_acceptors_.emplace_back(boost::bind(&boost::asio::io_service::run, &io_service_acceptors_));

		std::stringstream ss;
		try {
			// create memory pool for objects of connections	
			T_memory_pool_ptr memory_pool_ptr(new T_memory_pool, T_memory_pool_deleter() );			

			// create next connection, that will accepted next
			T_connection * const memory_pool_raw_ptr = reinterpret_cast<T_connection *>( memory_pool_ptr.get() );
			T_connection * const new_connection_raw_ptr = 
				T_connection::create(file_name_, memory_pool_raw_ptr, 0, io_service_executors_, timeout_);
		
			// start another acceptor in async mode
			acceptor_.async_accept(new_connection_raw_ptr->socket(),
									new_connection_raw_ptr->client_bind(
											   boost::bind(&T_server::handle_accept, this, 
														   boost::move(memory_pool_ptr),   // doesn't copy and doesn't use the atomic counter with memory barrier
														   0,
														   ba::placeholders::error)) );
		} catch(const seh::T_seh_exception& e) {
			ss << "T_seh_exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(const bs::system_error& e) {
			ss << "Boost system_error exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(const std::exception &e) {
			ss << "Exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
		} catch(...) {
			ss << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
		}	
		if(ss.str().size() > 0) {
			stop_services();
			throw std::runtime_error(ss.str());
		}
	}
}
// ----------------------------------------------------------------------------

/// 
/// Stop io_services and
/// wait for all executing threads
/// 
///
bool T_server::stop_services() {
	try {
		io_service_acceptors_.stop();
		io_service_executors_.stop();
		for(size_t i = 0; i < thr_grp_acceptors_.size(); ++i) thr_grp_acceptors_.at(i).join();
		for(size_t i = 0; i < thr_grp_executors_.size(); ++i) thr_grp_executors_.at(i).join();
	} catch(...) {
		return false;
	}
	return true;
}

/// Deinitialize services
T_server::~T_server() {
	std::cerr << "~T_server() \n";
	stop_services();
}
// ----------------------------------------------------------------------------

/// 
/// Run when new connection is accepted
/// 
/// @param memory_pool_ptr shared pointer to the allocated memory poll for connections
/// @param i_connect index of current connection in memory pool 
/// @param e reference to error object
///
void T_server::handle_accept( T_memory_pool_ptr memory_pool_ptr, size_t i_connect, const boost::system::error_code& e) {
	try {
		if (!e) {
			// get pointer of current connection
			T_connection * const current_memory_pool_raw_ptr = reinterpret_cast<T_connection *>( memory_pool_ptr.get() );
			T_connection * const current_connection_raw_ptr = &(current_memory_pool_raw_ptr[i_connect]);
			T_connection::T_shared_this current_connection_ptr(memory_pool_ptr, current_connection_raw_ptr );

			// schedule new task to thread pool
			current_connection_raw_ptr->run(boost::move(current_connection_ptr));	// sync launch of short-task: run()
		
			// increment index of connections
			++i_connect;	
		
			// if the limit of connections in the memory pool have been reached, then create a new memory pool
			if(i_connect == connections_in_memory_pool) {
				i_connect = 0;
				memory_pool_ptr.reset(new T_memory_pool, T_memory_pool_deleter() );
			}

			// create next connection, that will accepted
			T_connection * const new_memory_pool_raw_ptr = reinterpret_cast<T_connection *>( memory_pool_ptr.get() );
			T_connection * const new_connection_raw_ptr = 
				T_connection::create(file_name_, new_memory_pool_raw_ptr, i_connect, io_service_executors_, timeout_);

			// start new accept operation		
			acceptor_.async_accept(new_connection_raw_ptr->socket(),
									new_connection_raw_ptr->client_bind(
											   boost::bind(&T_server::handle_accept, this, 
														   boost::move(memory_pool_ptr),   // doesn't copy and doesn't use the atomic counter with memory barrier
														   i_connect,
														   ba::placeholders::error)) );
		}
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

