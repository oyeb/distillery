#include <utility>

/**
 * @file 		exceptions.hh
 * @author		Ajeesh Kumar K (ajeeshmsd@gmail.com)
 * @brief 		Exceptions and errors
 * @version 	0.1
 * @date 		2019-10-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef RTL_TRANSLATION_VALIDATION_EXCEPTIONS_HH
#define RTL_TRANSLATION_VALIDATION_EXCEPTIONS_HH

#include "pre-compiled-header.hh"

#include <exception>
#include <string>

namespace distillery
{
	class Exception : public std::exception
	{
	private:
		const std::string m_message;
	public:
		Exception(std::string message)
			:m_message(std::move(message))
		{
		}

		const char* what() const throw() override
		{
			return m_message.c_str();
		}
	};
}

#endif // RTL_TRANSLATION_VALIDATION_EXCEPTIONS_HH