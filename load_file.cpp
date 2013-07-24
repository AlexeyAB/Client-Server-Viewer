/**
 * @file   load_file.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer Connection
 *
 *
 */
// ----------------------------------------------------------------------------
#include "load_file.hpp"
// ----------------------------------------------------------------------------

/// Load data from file to static buffer
size_t T_load_file::load(const size_t i_block) {
	size_t read_size = 0;
	size_t current_i_position = (i_block <= max_block_position)? i_block : max_block_position;
	const size_t read_from = current_i_position * c_block_size;
	if(read_from < file_size) {
		size_t read_to = (current_i_position+1)*c_block_size;
		if(read_to > file_size) read_to = file_size;
		
		read_size = read_to - read_from;
		infile.seekg(read_from, std::ios::beg);
		infile.read(temp_buffer.data(), read_size);
		temp_buffer[read_size] = 0;
	} 
	return read_size;
}

/// Initialize temporary buffer, file stream, open file and get its size
T_load_file::T_load_file(const std::string &in_file_name) : file_name(in_file_name)
{
	temp_buffer[0] = 0;
	// Don't use std::ios:ate for opening large file (large than 4 GB), without std::ios:ate works in MSVS2010(VC10/_MSC_VER == 1600)
	// Bug: incorrect work and getting size of file large than 4 GB, bug fixed since from MSVS2012(VC11/_MSC_VER == 1700)
	// http://connect.microsoft.com/VisualStudio/feedback/details/627639/std-fstream-use-32-bit-int-as-pos-type-even-on-x64-platform
	infile.open(in_file_name.c_str(), std::ios::in | std::ios::binary);// | std::ios::ate);
	if(!infile.is_open()) {
		throw std::runtime_error("Can't open file: " + in_file_name);
	}
	infile.seekg(0, std::ios::end);
	file_size = infile.tellg();
	max_block_position = file_size/c_block_size;
	infile.seekg(0, std::ios::beg);
}

/// Reposition on the different page and get data from this page from file system
size_t T_load_file::reposition_and_load(const int64_t delta_index) {
	size_t current_block_position = block_position + delta_index;
	if(current_block_position <= max_block_position && current_block_position >= 0) {
		block_position = current_block_position;
		return load(block_position);
	}
	return 0;
}

// ----------------------------------------------------------------------------
