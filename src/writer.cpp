/*
 * writer.cpp
 *
 *  Created on: 2015-5-23
 *      Author: tiankonguse
 */

#include "writer.h"

namespace Json {

/** Converts an unsigned integer to string.
 * @param value Unsigned interger to convert to string
 * @param current Input/Output string buffer.
 *        Must have at least uintToStringBufferSize chars free.
 */
static inline void uintToString(LargestUInt value, char*& current) {
	*--current = 0;
	do {
		*--current = char(value % 10) + '0';
		value /= 10;
	} while (value != 0);
}

/** Change ',' to '.' everywhere in buffer.
 *
 * We had a sophisticated way, but it did not work in WinCE.
 * @see https://github.com/open-source-parsers/jsoncpp/pull/9
 */
static inline void fixNumericLocale(char* begin, char* end) {
	while (begin < end) {
		if (*begin == ',') {
			*begin = '.';
		}
		++begin;
	}
}

static bool containsControlCharacter(const char* str) {
	while (*str) {
		if (isControlCharacter(*(str++)))
			return true;
	}
	return false;
}

// Class Writer
// //////////////////////////////////////////////////////////////////
Writer::~Writer() {
}

// Class FastWriter
// //////////////////////////////////////////////////////////////////

FastWriter::FastWriter() :
		yamlCompatiblityEnabled_(false), dropNullPlaceholders_(false), omitEndingLineFeed_(
				false) {
}

void FastWriter::enableYAMLCompatibility() {
	yamlCompatiblityEnabled_ = true;
}

void FastWriter::dropNullPlaceholders() {
	dropNullPlaceholders_ = true;
}

void FastWriter::omitEndingLineFeed() {
	omitEndingLineFeed_ = true;
}

std::string FastWriter::write(const Value& root) {
	document_ = "";
	writeValue(root);
	if (!omitEndingLineFeed_) {
		document_ += "\n";
	}

	return document_;
}

void FastWriter::writeValue(const Value& value) {
	switch (value.type()) {
	case nullValue:
		if (!dropNullPlaceholders_) {
			document_ += "null";
		}
		break;
	case intValue:
		document_ += valueToString(value.asLargestInt());
		break;
	case uintValue:
		document_ += valueToString(value.asLargestUInt());
		break;
	case realValue:
		document_ += valueToString(value.asDouble());
		break;
	case stringValue:
		document_ += valueToQuotedString(value.asString().c_str());
		break;
	case booleanValue:
		document_ += valueToString(value.asBool());
		break;
	case arrayValue: {
		document_ += '[';
		int size = value.size();
		for (int index = 0; index < size; ++index) {
			if (index > 0) {
				document_ += ',';
			}
			writeValue(value[index]);
		}
		document_ += ']';
		break;
	}
	case objectValue: {
		Value::Members members(value.getMemberNames());
		document_ += '{';
		for (Value::Members::iterator it = members.begin(); it != members.end();
				++it) {
			const std::string& name = *it;
			if (it != members.begin()) {
				document_ += ',';
			}
			document_ += valueToQuotedString(name.c_str());
			document_ += yamlCompatiblityEnabled_ ? ": " : ":";
			writeValue(value[name]);
		}
		document_ += '}';
		break;
	}
	default: {
		break;
	}
	}
}

std::string valueToString(LargestInt value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	bool isNegative = value < 0;
	if (isNegative) {
		value = -value;
	}
	uintToString(LargestUInt(value), current);
	if (isNegative) {
		*--current = '-';
	}
	assert(current >= buffer);
	return current;
}

std::string valueToString(LargestUInt value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	uintToString(value, current);
	assert(current >= buffer);
	return current;
}

std::string valueToString(Int value) {
	return valueToString(LargestInt(value));
}

std::string valueToString(UInt value) {
	return valueToString(LargestUInt(value));
}

std::string valueToString(double value) {
	// Allocate a buffer that is more than large enough to store the 16 digits of
	// precision requested below.
	char buffer[32];
	int len = -1;

// Print into the buffer. We need not request the alternative representation
// that always has a decimal point because JSON doesn't distingish the
// concepts of reals and integers.
	if (isfinite(value)) {
		len = snprintf(buffer, sizeof(buffer), "%.17g", value);
	} else {
		// IEEE standard states that NaN values will not compare to themselves
		if (value != value) {
			len = snprintf(buffer, sizeof(buffer), "null");
		} else if (value < 0) {
			len = snprintf(buffer, sizeof(buffer), "-1e+9999");
		} else {
			len = snprintf(buffer, sizeof(buffer), "1e+9999");
		}
		// For those, we do not need to call fixNumLoc, but it is fast.
	}
	assert(len >= 0);
	fixNumericLocale(buffer, buffer + len);
	return buffer;
}

std::string valueToString(bool value) {
	return value ? "true" : "false";
}

std::string valueToQuotedString(const char* value) {
	if (value == NULL) {
		return "\"\"";
	}

	// Not sure how to handle unicode...
	if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL
			&& !containsControlCharacter(value))
		return std::string("\"") + value + "\"";
	// We have to walk value and escape any special characters.
	// Appending to std::string is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	std::string::size_type maxsize = strlen(value) * 2 + 3; // allescaped+quotes+NULL
	std::string result;
	result.reserve(maxsize); // to avoid lots of mallocs
	result += "\"";
	for (const char* c = value; *c != 0; ++c) {
		switch (*c) {
		case '\"':
			result += "\\\"";
			break;
		case '\\':
			result += "\\\\";
			break;
		case '\b':
			result += "\\b";
			break;
		case '\f':
			result += "\\f";
			break;
		case '\n':
			result += "\\n";
			break;
		case '\r':
			result += "\\r";
			break;
		case '\t':
			result += "\\t";
			break;
			// case '/':
			// Even though \/ is considered a legal escape in JSON, a bare
			// slash is also legal, so I see no reason to escape it.
			// (I hope I am not misunderstanding something.
			// blep notes: actually escaping \/ may be useful in javascript to avoid </
			// sequence.
			// Should add a flag to allow this compatibility mode and prevent this
			// sequence from occurring.
		default:
			if (isControlCharacter(*c)) {
				std::ostringstream oss;
				oss << "\\u" << std::hex << std::uppercase << std::setfill('0')
						<< std::setw(4) << static_cast<int>(*c);
				result += oss.str();
			} else {
				result += *c;
			}
			break;
		}
	}
	result += "\"";
	return result;
}

}
