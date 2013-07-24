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
#include "file_view.hpp"
// ----------------------------------------------------------------------------

/// Load buffer from the remote server
void T_file_view::load_buff(const size_t pos) {
	size_t low_block = pos/c_block_size;
	client_ptr->load(low_block);
	buffer = client_ptr->get_buffer();
	if (client_ptr->get_max_block_position() >= low_block+1) {
		client_ptr->load(low_block+1);
		buffer += std::string(client_ptr->get_buffer());
	}
	buffer_pos = low_block*c_block_size;
}


/// Calculate next row size
size_t T_file_view::calc_next_row_size(const size_t relative_pos) const {
	size_t i = relative_pos;
	for(; i < buffer.size(); ++i) {
		if(buffer[i] == '\n') break;
	}
	return i - relative_pos + 1;
}


/// Calculate prev row size
size_t T_file_view::calc_prev_row_size(const size_t relative_pos) const {
	size_t i = relative_pos;
	for(; i > 0; --i) {
		if(buffer[i] == '\n') break;
	}
	return relative_pos - i + 1;
}

/// Get the next strings
std::string T_file_view::get_next_rows(const size_t start_pos, size_t &rows_number) const {
	size_t i = start_pos;
	while(rows_number < rows_in_memo && i < buffer.size()) {
		++i;
		if(buffer[i] == '\n') ++rows_number;
	}
	return buffer.substr(start_pos, i - start_pos);
}

/// Get the previous strings
std::string T_file_view::get_previous_rows(const size_t start_pos, size_t &rows_number) const {
	size_t i = start_pos;
	while(rows_number < rows_in_memo && i > 0) {
		--i;
		if(buffer[i] == '\n') ++rows_number;
	}
	return buffer.substr(i, start_pos - i);
}

/// Get strings by index of position in file
std::string T_file_view::get_strings_for_pos(size_t pos) {
	if(pos > client_ptr->get_file_size())
		pos = client_ptr->get_file_size();

	std::string res_str;

	// Preload data if it need
	if(pos == 0 && buffer.size() == 0) load_buff(0);
	if(pos < buffer_pos) load_buff(pos);
	if(pos > buffer_pos + c_block_size + 4*rows_in_memo*symbols_per_scroll) load_buff(pos);	// may be optimized

	// Relative position in buffer
	const size_t relative_pos = pos - buffer_pos;

	// Calculate prev row size
	prev_row_size = calc_prev_row_size(relative_pos);

	// Calculate next row size
	next_row_size = calc_next_row_size(relative_pos);

	// Show strings starting from this index
	size_t i_pos = (pos == 0)?0:(next_row_size + relative_pos - 1);
	const size_t start_pos = i_pos;
	
	// Strings got
	size_t rows_number = 0;

	// Get next rows
	res_str = get_next_rows(start_pos, rows_number);

	// Get previous row if it need
	if(rows_number < rows_in_memo) {
		res_str = get_previous_rows(start_pos, rows_number) + res_str;
	}

	return res_str;
}