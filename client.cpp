/**
 * @file   client.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#include "stdafx.h"
#include "client.hpp"

#include "seh_exception.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
// ----------------------------------------------------------------------------

/// Simple handler for the notification of the completion of asynchronous operations
static void handle_async(volatile bool &f_done, boost::system::error_code &out_err, const boost::system::error_code& err) {
	out_err = err;
	f_done = true;
}
// ----------------------------------------------------------------------------

/// Send index of page and receive page data
size_t T_client::load(const size_t i_block) {
	std::stringstream ss;
	try {
		block_position = (i_block <= max_block_position)? i_block : max_block_position;
		boost::system::error_code err;

		// Send number of page
		ba::write(client_socket_, ba::buffer(&block_position, sizeof(block_position)), err);
		if(!!err) throw std::runtime_error("Connection false!");

		// Get data of page
		ba::read(client_socket_, ba::buffer(temp_buffer, c_block_size), err);	
		if(!!err) throw std::runtime_error("Connection false!");

		for(size_t i = 0; i < c_block_size; ++i)
			if(temp_buffer[i] == 0) return i;
		temp_buffer[c_block_size] = 0;
		return c_block_size;
	} catch(const boost::system::system_error& e) {
		ss << "Boost system_error exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
	} catch(const std::exception &e) {
		ss << "Exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
	} catch(...) {
		ss << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
	}
	throw std::runtime_error(ss.str());
	return 0;
}
// ----------------------------------------------------------------------------

/// Reposition on the different page and get data from this page from the server
size_t T_client::reposition_and_load(const int64_t delta_index) {
		size_t current_block_position = block_position + delta_index;
		// If the new block number is in the valid range 
		if(current_block_position <= max_block_position && current_block_position >= 0) {
			block_position = current_block_position;
			return load(block_position);
		}

		// If the new block number is NOT in the valid range 
		if(delta_index < 0) {
			block_position = 0;
		} else if(delta_index > 0) {
			block_position = max_block_position;
		} else 
			return 0;
		return load(block_position);
}
// ----------------------------------------------------------------------------

/// Initialize io_service and socket, resolve the server, try to connect and get file size
T_client::T_client(ba::io_service& io_service, unsigned int remote_port, std::string remote_address, 
				   const unsigned int timeout) 
	: io_service_(io_service), client_socket_(io_service_), deadline_(io_service_), timeout_(boost::posix_time::seconds(timeout))
{
	std::stringstream ss;
	try {
		boost::system::error_code err;
		address_n_port_ = remote_address + ":" + boost::lexical_cast<std::string>(remote_port);

		//int32_t timeout_sec = 1 * 1000;
		//setsockopt(client_socket_.native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_sec, sizeof(timeout_sec));
		//setsockopt(client_socket_.native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout_sec, sizeof(timeout_sec));

		// Start deadline timer and its checker
		check_deadline();
	
		// Resolve remote address:port of server for IPv4 addresses
		ba::ip::tcp::resolver resolver(io_service_);
		ba::ip::tcp::resolver::query query(ba::ip::tcp::v4(), remote_address, boost::lexical_cast<std::string>(remote_port) );
		remote_endpoint_it_ = resolver.resolve(query, err);
		if(!!err || remote_endpoint_it_ == ba::ip::tcp::resolver::iterator()) 
			throw std::runtime_error(std::string("Can't resolve the server address and port: ") + address_n_port_ +
									 ", error: " + err.message());
		
		// Output resolved ip-addresses
		std::clog << "Remote resolved endpoints for: " << remote_address << std::endl;
		unsigned remote_number = 0;
		for(auto it = remote_endpoint_it_; it != ba::ip::tcp::resolver::iterator(); ++it, ++remote_number) {
			std::clog << remote_number << ": " << it->endpoint() << std::endl;
		}
		std::clog << std::endl;

		const ba::ip::tcp::endpoint remote_endpoint_ = *remote_endpoint_it_;
		std::clog << "Start with remote: " << remote_endpoint_ << std::endl;

		// Try to connect while is not success
		auto it = remote_endpoint_it_;
		do {
			ba::ip::tcp::endpoint endpoint = *it;
			//client_socket_.connect(endpoint, err);
			{
				volatile bool f_done = false;
				deadline_.expires_from_now(timeout_);
				client_socket_.async_connect(endpoint, boost::bind(&handle_async, boost::ref(f_done), boost::ref(err), ba::placeholders::error) );
				do { client_socket_.get_io_service().run_one(); } while (f_done != true);
			}
			if(!!err) ++it;			
			if(it == ba::ip::tcp::resolver::iterator())
				throw std::runtime_error(std::string("Can't connect to the server: ") + address_n_port_ +
										 ", error: " + err.message());
		} while(!!err);
		std::clog << "Connected to remote: " << it->endpoint() << std::endl;

		// Read file size from server
		//ba::read(client_socket_, ba::buffer(&file_size, sizeof(file_size)), err);
		{
			volatile bool f_done = false;
			deadline_.expires_from_now(timeout_);
			ba::async_read(client_socket_, 
						ba::buffer(&file_size, sizeof(file_size)), 
						ba::transfer_exactly(sizeof(file_size)), 
						boost::bind(&handle_async, boost::ref(f_done), boost::ref(err), ba::placeholders::error) );
			do { client_socket_.get_io_service().run_one(); } while (f_done != true);
		}

		if(!!err) 
			throw std::runtime_error(std::string("Can't read form the server: ") + address_n_port_ +
									 ", error: " + err.message());
		max_block_position = file_size/c_block_size;
	
	} catch(const boost::system::system_error& e) {
		ss << "Boost system_error exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
	} catch(const std::exception &e) {
		ss << "Exception: " << e.what() << "\n ->throw place: " << THROW_PLACE << std::endl;
	} catch(...) {
		ss << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
	}
	if(ss.str().size() > 0)
		throw std::runtime_error(ss.str());
}
// ----------------------------------------------------------------------------

/// Start deadline timer and check it
void T_client::check_deadline() {
	try {
		// Check whether the deadline has passed.
		if (deadline_.expires_at() <= ba::deadline_timer::traits_type::now()) {			  
			boost::system::error_code ignored_ec;
			client_socket_.close(ignored_ec);					// The deadline has passed. Stop IO-operations.
			deadline_.expires_at(boost::posix_time::pos_infin);	// There is no longer an active deadline.
		}
		// Put the actor back to sleep.
		deadline_.async_wait(boost::bind(&T_client::check_deadline, this));
	} catch(...) {
		std::cerr << "Unknown exception!" << "\n ->throw place: " << THROW_PLACE << std::endl;
	}
}
// ----------------------------------------------------------------------------
