/*
 * tools.h
 *
 *  Created on: 2015-5-24
 *      Author: tiankonguse
 */

#ifndef TOOLS_H_INCLUDE_MINI_JSONCPP_
#define TOOLS_H_INCLUDE_MINI_JSONCPP_

/// Converts a unicode code-point to UTF-8.
static inline std::string codePointToUTF8(unsigned int cp) {
	std::string result;

	// based on description from http://en.wikipedia.org/wiki/UTF-8

	if (cp <= 0x7f) {
		result.resize(1);
		result[0] = static_cast<char>(cp);
	} else if (cp <= 0x7FF) {
		result.resize(2);
		result[1] = static_cast<char>(0x80 | (0x3f & cp));
		result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
	} else if (cp <= 0xFFFF) {
		result.resize(3);
		result[2] = static_cast<char>(0x80 | (0x3f & cp));
		result[1] = 0x80 | static_cast<char>((0x3f & (cp >> 6)));
		result[0] = 0xE0 | static_cast<char>((0xf & (cp >> 12)));
	} else if (cp <= 0x10FFFF) {
		result.resize(4);
		result[3] = static_cast<char>(0x80 | (0x3f & cp));
		result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
		result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
		result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
	}

	return result;
}

#endif /* TOOLS_H_INCLUDE_MINI_JSONCPP_ */
