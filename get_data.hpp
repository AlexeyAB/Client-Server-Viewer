/**
 * @file   get_data.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef GET_DATA_HPP
#define GET_DATA_HPP
// ----------------------------------------------------------------------------
#include <boost/array.hpp>
#include <fstream>
#include <cstdint>
#include <string>
// ----------------------------------------------------------------------------

/// Abstract class of getting data of file
class T_get_data {
public:
	static const size_t c_block_size = 1024*1024/16;	///< 1/16 MB
protected:
	boost::array<char, c_block_size + 1> temp_buffer;	///< temporary buffer to store single block of file
	uint64_t block_position, max_block_position;		///< current position of block(page) in file, and the maximum position of block in file
	uint64_t file_size;									///< size of file, which is need to get
public:
	inline char const*const get_buffer() const	{ return temp_buffer.data(); }
	inline uint64_t get_block_position() const	{ return block_position; }
	inline const uint64_t get_max_block_position() const { return max_block_position; }
	inline const uint64_t get_file_size() const { return file_size; }

	T_get_data() : block_position(0) {}
	virtual ~T_get_data() {}

	/// Get page data by index of page
	virtual size_t load(const size_t i_block) = 0;

	/// Reposition on the different page and get data from this page 
	virtual size_t reposition_and_load(const int64_t delta_index) = 0;

	/// Get information about source of the file
	virtual std::string const& get_source() const = 0;
};

// ----------------------------------------------------------------------------
#endif // GET_DATA_HPP