#ifndef BASE64_H__
#define BASE64_H__

#include "macros.h"
#include <string>


/**
 * @brief An implementation of the Base64 data encoding (RFC 3548)
 * @since 0.8
 */
namespace Base64
{

	/**
	 * Base64-encodes the input according to RFC 3548.
	 * @param input The data to encode.
	 * @return The encoded string.
	 */
	GLOOX_API const std::string encode64( const std::string& input );

	/**
	 * Base64-decodes the input according to RFC 3548.
	 * @param input The encoded data.
	 * @return The decoded data.
	 */
	GLOOX_API const std::string decode64( const std::string& input );

}



#endif // BASE64_H__
