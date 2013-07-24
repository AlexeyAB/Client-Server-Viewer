/**
 * @file   load_file.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef LOAD_FILE_H
#define LOAD_FILE_H
// ----------------------------------------------------------------------------
#include <fstream>
#include <string>

#include "get_data.hpp"
// ----------------------------------------------------------------------------

/// Loads file from file system by parts
class T_load_file : public T_get_data  {
private:
	std::ifstream infile;				///< the object of file stream, which file reads
	const std::string file_name;		///< source file name
public:
	/// Load data from file to static buffer
	virtual size_t load(const size_t i_block);

	/// Reposition on the different page and get data from this page from file system
	virtual size_t reposition_and_load(const int64_t delta_index);

	T_load_file(const std::string &in_file_name);
	virtual ~T_load_file() { infile.close(); }

	/// Get information about source of the file
	virtual std::string const& get_source() const {
		return file_name;
	}
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#endif	// LOAD_FILE_H