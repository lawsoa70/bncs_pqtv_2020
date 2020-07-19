
#include "except.h"

static void my_translator(unsigned, EXCEPTION_POINTERS *info)
{
	throw structured_exception(*info);
}

//
//  structured_exception::
//
structured_exception::structured_exception
(EXCEPTION_POINTERS const &info) throw()
{
	EXCEPTION_RECORD const &exception = *(info.ExceptionRecord);
	address_ = exception.ExceptionAddress;
	code_ = exception.ExceptionCode;
}

void structured_exception::install() throw()
{
	_set_se_translator(my_translator);
}

unsigned structured_exception::what() const throw()
{ 
	return code_;
}

void const *structured_exception::where() const throw()
{
	return address_;
}
