/*
 * writer.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef WRITER_H_INCLUDE_MINI_JSONCPP_
#define WRITER_H_INCLUDE_MINI_JSONCPP_

#include "value.h"
#include <vector>
#include <string>
#include <ostream>
#include <cstdio>
#include <cstdlib>
namespace Json {

class Value;

class Writer {
public:
	virtual ~Writer();

	virtual std::string write(const Value& root) = 0;
};

/** \brief Outputs a Value in <a HREF="http://www.json.org">JSON</a> format
 *without formatting (not human friendly).
 *
 * The JSON document is written in a single line. It is not intended for 'human'
 *consumption,
 * but may be usefull to support feature such as RPC where bandwith is limited.
 * \sa Reader, Value
 * \deprecated Use StreamWriterBuilder.
 */
class FastWriter: public Writer {

public:
	FastWriter();
	virtual ~FastWriter() {
	}

	void enableYAMLCompatibility();

	/** \brief Drop the "null" string from the writer's output for nullValues.
	 * Strictly speaking, this is not valid JSON. But when the output is being
	 * fed to a browser's Javascript, it makes for smaller output and the
	 * browser can handle the output just fine.
	 */
	void dropNullPlaceholders();

	void omitEndingLineFeed();

public:
	// overridden from Writer
	virtual std::string write(const Value& root);

private:
	void writeValue(const Value& value);

	std::string document_;
	bool yamlCompatiblityEnabled_;
	bool dropNullPlaceholders_;
	bool omitEndingLineFeed_;
};

enum {
	/// Constant that specify the size of the buffer that must be passed to
	/// uintToString.
	uintToStringBufferSize = 3 * sizeof(LargestUInt) + 1
};

// Defines a char buffer for use with uintToString().
typedef char UIntToStringBuffer[uintToStringBufferSize];

/// Returns true if ch is a control character (in range [0,32[).
static inline bool isControlCharacter(char ch) {
	return ch > 0 && ch <= 0x1F;
}
std::string valueToString(Int value);
std::string valueToString(UInt value);
std::string valueToString(LargestInt value);
std::string valueToString(LargestUInt value);
std::string valueToString(double value);
std::string valueToString(bool value);
std::string valueToQuotedString(const char* value);
std::ostream& operator<<(std::ostream&, const Value& root);

}
// namespace Json

#endif /* WRITER_H_INCLUDE_MINI_JSONCPP_ */
