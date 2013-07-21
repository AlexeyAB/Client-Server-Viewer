/**
 * @file   main_boost_asio.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 * 
 * @brief Implementation of Server of Client-Server-Viewer with usage of threads pool for acceptors and executors for 
 * implementation of strategy of many connections per each thread from thread pool for number of threads limitation.
 * IO client->server and server->client operations are uses in async mode.
 *
 * 
 */
// ----------------------------------------------------------------------------
#include "server.hpp"
#include "seh_exception.hpp"
// ----------------------------------------------------------------------------
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
// ----------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <string>
#include <locale>


/// 
/// Main routine
/// 
/// @param argc number of arguments
/// @param argv pointers to arguments
/// 
/// @return error code
///
int main(int argc, char** argv) {
	std::ofstream file_log, file_error;
	try {
		std::locale::global(std::locale("rus"));
		std::cout << "Usage: asio_server.exe [file_name local_port local_address timeout_sec number_acceptors numer_executors language_locale]" << std::endl << std::endl;

#ifdef _MSC_VER
		std::cout << "_MSC_VER  = " << _MSC_VER  << std::endl; 
#endif
#ifdef __GNUC__
		std::cout << "__GNUC__  = " << __GNUC__  << ", __GNUC_MINOR__ = " << __GNUC_MINOR__ << std::endl;
#endif
		// File name for view
		std::string file_name = "list.txt";

		// Local interface address::port
		unsigned short local_port = 10001;
		std::string local_interface_address = "0.0.0.0";

		// Time in seconds for timeout of connections
		unsigned int timeout_sec = 120;

		// Number of thread for acceptors and executors
		unsigned int thread_num_acceptors = 2;
		unsigned int thread_num_executors = boost::thread::hardware_concurrency();

		std::cout << "(Default: asio_server.exe " << " " << file_name << " " <<
			local_port << " " << local_interface_address << " " << timeout_sec << " " <<
			thread_num_acceptors << " " << thread_num_executors << " " << std::locale::global(std::locale()).name() << ")" << std::endl;


		// read file name for view
		if(argc > 1)
			file_name = argv[1];

		// read local port number from command line, if provided
		if(argc > 2)
			local_port = boost::lexical_cast<unsigned short>(argv[2]);
		// read local interface address from command line, if provided
		if(argc > 3)
			local_interface_address = argv[3];

		// read time in second of timeout of connection from command line, if provided
		if(argc > 4)
			timeout_sec = boost::lexical_cast<unsigned int>(argv[4]);

		// read number of threads in thread pool from command line, if provided
		if(argc > 5)
			thread_num_acceptors = boost::lexical_cast<unsigned int>(argv[5]);
		if(argc > 6)
			thread_num_executors = boost::lexical_cast<unsigned int>(argv[6]);

		// set language locale
		if(argc > 7)
			setlocale(LC_ALL, argv[7]);
		// ----------------------------------------------------------------------------

		// Enable Windows SEH exceptions. Compile with key: /EHa 
		seh::seh_exception_init();

		if(true) std::cout.rdbuf(NULL);	// switch off std::cout
				
		/// Output errors(exceptions) to file instead of the console (std::cerr)
		if(false) {			
			file_error.open("log_errors.txt", std::ios_base::out | std::ios_base::trunc);	// std::ios_base::ate
			std::cerr.rdbuf(file_error.rdbuf());
		}

		/// Output to file instead of the console (std::clog)
		if(false) {			
			file_log.open("log.txt", std::ios_base::out | std::ios_base::app);
			std::clog.rdbuf(file_log.rdbuf());
		}
		std::clog << "----------------------------------------------------------------------------"  << std::endl;
		// ----------------------------------------------------------------------------

		boost::asio::io_service io_service_acceptors, io_service_executors;
		// construct new server object
		T_server s(file_name, io_service_acceptors, io_service_executors, thread_num_acceptors, thread_num_executors,
			local_port, local_interface_address, timeout_sec);
		// run io_service object, that perform all dispatch operations
		io_service_acceptors.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception!" << std::endl;
	}
	if(file_log.is_open()) file_log.close();
	if(file_error.is_open()) file_error.close();
	int b; std::cin >> b;

	return 0;
}
// ----------------------------------------------------------------------------

     

