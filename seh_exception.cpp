/**
 * @file   seh_exception.cpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Windows SEH-exception.
 * To use it - must compile with key: /EHa 
 *
 */
// ----------------------------------------------------------------------------
#include "seh_exception.hpp"
// ----------------------------------------------------------------------------
#ifdef _MSC_VER

#include <windows.h>
#include <eh.h>
#include <string>
// ----------------------------------------------------------------------------

/// Get a string message by error code of the SEH-exception
static std::string get_error_str(const unsigned int m_error_code) {
	switch(m_error_code) {
	case EXCEPTION_FLT_DENORMAL_OPERAND: return std::string("DENORMAL OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:   return std::string("DIVIDE BY ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT:   return std::string("INEXACT RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION: return std::string("INVALID OPERATION");
	case EXCEPTION_FLT_OVERFLOW:	return std::string("OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK: return std::string("STACK CHECK");
	case EXCEPTION_FLT_UNDERFLOW:	return std::string("UNDERFLOW");

	case EXCEPTION_INT_DIVIDE_BY_ZERO: return std::string("INT DIVIDE BY ZERO");
	case EXCEPTION_INT_OVERFLOW:	return std::string("INT OVERFLOW");
		
	case EXCEPTION_ACCESS_VIOLATION: return std::string("ACCESS VIOLATION");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   return std::string("ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_BREAKPOINT:   return std::string("BREAKPOINT");
	case EXCEPTION_DATATYPE_MISALIGNMENT: return std::string("DATATYPE MISALIGNMENT");
	case EXCEPTION_ILLEGAL_INSTRUCTION:	return std::string("ILLEGAL INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR: return std::string("IN PAGE ERROR");
	case EXCEPTION_INVALID_DISPOSITION:	return std::string("INVALID DISPOSITION");

	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return std::string("NONCONTINUABLE EXCEPTION");
	case EXCEPTION_PRIV_INSTRUCTION:	return std::string("PRIV INSTRUCTION");
	case EXCEPTION_SINGLE_STEP: return std::string("SINGLE STEP");
	case EXCEPTION_STACK_OVERFLOW:	return std::string("STACK OVERFLOW");
	}
	return std::string("UNKNOWN");
}	

/// Generate exception of T_seh_exception type inherited from std::runtime_error
void trans_func(unsigned int error_code, EXCEPTION_POINTERS* pExp) {
    throw seh::T_seh_exception(get_error_str(error_code));
}

/// Windwos SEH-exception init
void seh::seh_exception_init() {
	::_set_se_translator( trans_func );
}
// ----------------------------------------------------------------------------
#else

/// In not-Windows OS nothing to do
void seh::seh_exception_init() {}
#endif
// ----------------------------------------------------------------------------