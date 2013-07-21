/**
 * @file   seh_exception.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Windows SEH-exception. 
 * To use it - must compile with key: /EHa 
 *
 */
// ----------------------------------------------------------------------------
#ifndef SEH_EXCEPTION_HPP
#define SEH_EXCEPTION_HPP
// ----------------------------------------------------------------------------
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
// ----------------------------------------------------------------------------
#ifndef __FUNCTION__
	#define __FUNCTION__
#endif

/// macro to determine the location of catching an exception
#define THROW_PLACE std::string("(") + __FILE__ + ":" + boost::lexical_cast<std::string>(__LINE__) + ": " + __DATE__ + " " + __TIME__ + ", " + __FUNCTION__ +  ")\n\t"
// ----------------------------------------------------------------------------


namespace seh {
	/// Class wrapper, for SEH-exception
	struct T_seh_exception : std::runtime_error {
		T_seh_exception(const std::string& str) : runtime_error(str) {}
	};
	// ----------------------------------------------------------------------------
	/// SEH-exception init
	void seh_exception_init();
};

// ----------------------------------------------------------------------------
#endif // SEH_EXCEPTION_HPP