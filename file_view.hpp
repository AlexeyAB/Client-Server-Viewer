/**
 * @file   file_view.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Client-Server-Viewer file viewe
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef FILE_VIEW_HPP
#define FILE_VIEW_HPP
// ----------------------------------------------------------------------------
#include "get_data.hpp"

#include <boost/shared_ptr.hpp>
// ----------------------------------------------------------------------------

class T_file_view {
private:


	static const size_t c_block_size = T_get_data::c_block_size;	/// size of block which read from the server
	std::string buffer;								/// local buffer for rows
	volatile size_t buffer_pos;						/// offset into the file from which the block is loaded from the server
	volatile size_t prev_row_size, next_row_size;	/// size of prev/next rows - for the move by using scroll rows up/down
	const volatile size_t rows_in_memo;				/// number of rows to show in Edit(Memo)

	volatile size_t symbols_per_scroll;				/// symbols per single step of scroll bar (excluding scroll buttons-rows)	
	
	size_t calc_prev_row_size(const size_t relative_pos) const;
	size_t calc_next_row_size(const size_t relative_pos) const;
	std::string get_next_rows(const size_t start_pos, size_t &rows_number) const;
	std::string get_previous_rows(const size_t start_pos, size_t &rows_number) const;
	void load_buff(const size_t pos);
public:
	T_file_view(const size_t _rows_in_memo = 17) 
		: rows_in_memo(_rows_in_memo)
	{}

	boost::shared_ptr<T_get_data> client_ptr;	/// pointer to the object that given the data (T_load_file/T_client)

	void reset_position(const size_t _symbols_per_scroll) {
		symbols_per_scroll = _symbols_per_scroll;
		buffer_pos = 0;
		prev_row_size = 0;
		next_row_size = 0;
		buffer.clear();
	}

	const size_t get_buffer_size() const {
		return buffer.size();
	}

	const size_t get_prev_row_size() const {
		return prev_row_size;
	}

	const size_t get_next_row_size() const {
		return next_row_size;
	}

	const size_t get_file_size() const {
		return client_ptr->get_file_size();
	}

	std::string const& get_source() const {
		return client_ptr->get_source();
	}

	/// Get strings by index of position in file
	std::string get_strings_for_pos(size_t pos);
};
// ----------------------------------------------------------------------------
#endif // FILE_VIEW_HPP