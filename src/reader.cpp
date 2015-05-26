/*
 * reader.cpp
 *
 *  Created on: 2015-5-24
 *      Author: tiankonguse
 */

#include "reader.h"

namespace Json {

// Implementation of class Reader
// ////////////////////////////////

// Class Reader
// //////////////////////////////////////////////////////////////////

Reader::Reader(int debug) :
		begin_(NULL), end_(NULL), current_(NULL), lastValueEnd_(NULL), lastValue_(
		NULL), debug(debug) {
}

bool Reader::parse(const std::string& document, Value& root) {
	document_ = document;
	if (debug) {
		printf("document : %s\n", document.c_str());
	}
	const char* begin = document_.c_str();
	const char* end = begin + document_.length();
	return parse(begin, end, root);
}

bool Reader::parse(const char* beginDoc, const char* endDoc, Value& root) {
	begin_ = beginDoc;
	end_ = endDoc;

	current_ = begin_;
	lastValueEnd_ = 0;
	lastValue_ = 0;

	errors_.clear();
	while (!nodes_.empty()) {
		nodes_.pop();
	}

	nodes_.push(&root);

	bool successful = readValue();
	Token token;

	if (successful && !root.isArray() && !root.isObject()) {
		// Set error location to start of doc, ideally should be first token found
		// in doc
		token.type_ = tokenError;
		token.start_ = beginDoc;
		token.end_ = endDoc;
		addError(
				"A valid JSON document must be either an array or an object value.",
				token);
		return false;
	}
	return successful;
}

bool Reader::readValue() {

	Token token;
	bool successful = true;

	readToken(token);

	if (debug) {
		printf("%d %s   readToken : token=%d current_=%p\n", __LINE__,
				__FUNCTION__, token.type_, current_);
	}

	switch (token.type_) {
	case tokenObjectBegin: {
		if (debug) {
			printf("%d %s   tokenObjectBegin :current_=%p\n", __LINE__,
					__FUNCTION__, current_);
		}

		successful = readObject(token);
		break;
	}
	case tokenArrayBegin: {
		successful = readArray(token);
		currentValue().setOffsetLimit(current_ - begin_);
		break;
	}
	case tokenNumber: {
		successful = decodeNumber(token);
		break;
	}
	case tokenString: {
		successful = decodeString(token);
		break;
	}
	case tokenTrue: {
		Value v(true);
		currentValue().swap(v);
		currentValue().setOffsetStart(token.start_ - begin_);
		currentValue().setOffsetLimit(token.end_ - begin_);
		break;
	}
	case tokenFalse: {
		Value v(false);
		currentValue().swap(v);
		currentValue().setOffsetStart(token.start_ - begin_);
		currentValue().setOffsetLimit(token.end_ - begin_);
		break;
	}
	case tokenNull: {
		Value v;
		currentValue().swap(v);
		currentValue().setOffsetStart(token.start_ - begin_);
		currentValue().setOffsetLimit(token.end_ - begin_);
		break;
	}
	case tokenArraySeparator:
	case tokenObjectEnd:
	case tokenArrayEnd: {
		// "Un-read" the current token and mark the current value as a null
		// token.
		current_--;
		Value v;
		currentValue().swap(v);
		currentValue().setOffsetStart(current_ - begin_ - 1);
		currentValue().setOffsetLimit(current_ - begin_);
		break;
	}
	default: {
		currentValue().setOffsetStart(token.start_ - begin_);
		currentValue().setOffsetLimit(token.end_ - begin_);
		return addError("Syntax error: value, object or array expected.", token);
	}
	}

	return successful;
}

bool Reader::readToken(Token& token) {
	skipSpaces();

	if (debug) {
		printf("%d %s   begin  current_=%p\n", __LINE__, __FUNCTION__,
				current_);
	}

	token.start_ = current_;
	Char c = getNextChar();
	bool ok = true;

	if (debug) {
		printf("%d %s   read=%c current_=%p\n", __LINE__, __FUNCTION__, c,
				current_);
	}

	switch (c) {
	case '{':
		token.type_ = tokenObjectBegin;
		break;
	case '}':
		token.type_ = tokenObjectEnd;
		break;
	case '[':
		token.type_ = tokenArrayBegin;
		break;
	case ']':
		token.type_ = tokenArrayEnd;
		break;
	case '"':
		token.type_ = tokenString;
		ok = readString();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		token.type_ = tokenNumber;
		readNumber();
		break;
	case 't':
		token.type_ = tokenTrue;
		ok = match("rue", 3);
		break;
	case 'f':
		token.type_ = tokenFalse;
		ok = match("alse", 4);
		break;
	case 'n':
		token.type_ = tokenNull;
		ok = match("ull", 3);
		break;
	case ',':
		token.type_ = tokenArraySeparator;
		break;
	case ':':
		token.type_ = tokenMemberSeparator;
		break;
	case 0:
		token.type_ = tokenEndOfStream;
		break;
	default:
		ok = false;
		break;
	}
	if (!ok) {
		token.type_ = tokenError;
	}
	if (debug) {
		printf("%d %s   token=%d current_=%p\n", __LINE__, __FUNCTION__,
				token.type_, current_);
	}

	token.end_ = current_;
	return true;
}

void Reader::skipSpaces() {
	while (current_ != end_) {
		Char c = *current_;
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			++current_;
		else
			break;
	}
}

bool Reader::match(Location pattern, int patternLength) {
	if (end_ - current_ < patternLength) {
		return false;
	}

	int index = patternLength;
	while (index--) {
		if (current_[index] != pattern[index]) {
			return false;
		}
	}
	current_ += patternLength;
	return true;
}

void Reader::readNumber() {
	if (!(current_ < end_)) {
		return;
	}

// integral part
	while (current_ < end_ && *current_ >= '0' && *current_ <= '9') {
		current_ = current_ + 1;
	}

// fractional part
	if (current_ < end_ && *current_ == '.') {
		current_ = current_ + 1;
		while (current_ < end_ && *current_ >= '0' && *current_ <= '9') {
			current_ = current_ + 1;
		}
	}
// exponential part
	if (current_ < end_ && (*current_ == 'e' || *current_ == 'E')) {
		current_ = current_ + 1;
		if (current_ < end_ && (*current_ == '+' || *current_ == '-')) {
			current_ = current_ + 1;
		}
		while (current_ < end_ && *current_ >= '0' && *current_ <= '9') {
			current_ = current_ + 1;
		}
	}
}

bool Reader::readString() {
	Char c = 0;
	while (current_ != end_) {
		c = getNextChar();
		if (c == '\\') {
			getNextChar();
		} else if (c == '"') {
			break;
		}
	}
	return c == '"';
}

bool Reader::readObject(Token& tokenStart) {
	if (debug) {
		printf("%d %s   begin :current_=%p\n", __LINE__, __FUNCTION__,
				current_);
	}

	Token tokenName;
	std::string name = "";
	Value init(objectValue);
	currentValue().swap(init);
	while (readToken(tokenName)) {
		// empty object
		if (tokenName.type_ == tokenObjectEnd && name.empty()) {
			if (debug) {
				printf("%d %s   object empty  :current_=%p\n", __LINE__,
						__FUNCTION__, current_);
			}
			return true;
		}

		name = "";
		if (tokenName.type_ == tokenString) {
			if (!decodeString(tokenName, name)) {
				return recoverFromError(tokenObjectEnd);
			}
		} else if (tokenName.type_ == tokenNumber) {
			Value numberName;
			if (!decodeNumber(tokenName, numberName))
				return recoverFromError(tokenObjectEnd);
			name = numberName.asString();
		} else {
			break;
		}

		Token colon;
		if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
			return addErrorAndRecover("Missing ':' after object member name",
					colon, tokenObjectEnd);
		}
		Value& value = currentValue()[name];
		nodes_.push(&value);
		bool ok = readValue();
		nodes_.pop();
		if (!ok) // error already set
			return recoverFromError(tokenObjectEnd);

		Token comma;
		if (!readToken(comma)
				|| (comma.type_ != tokenObjectEnd
						&& comma.type_ != tokenArraySeparator
						&& comma.type_ != tokenComment)) {
			return addErrorAndRecover(
					"Missing ',' or '}' in object declaration", comma,
					tokenObjectEnd);
		}
		bool finalizeTokenOk = true;
		while (comma.type_ == tokenComment && finalizeTokenOk)
			finalizeTokenOk = readToken(comma);
		if (comma.type_ == tokenObjectEnd)
			return true;
	}
	return addErrorAndRecover("Missing '}' or object member name", tokenName,
			tokenObjectEnd);
}

bool Reader::readArray(Token& tokenStart) {
	Value init(arrayValue);
	currentValue().swap(init);
	currentValue().setOffsetStart(tokenStart.start_ - begin_);
	skipSpaces();
	if (*current_ == ']') // empty array
			{
		Token endArray;
		readToken(endArray);
		return true;
	}
	int index = 0;
	for (;;) {
		Value& value = currentValue()[index++];
		nodes_.push(&value);
		bool ok = readValue();
		nodes_.pop();
		if (!ok) // error already set
			return recoverFromError(tokenArrayEnd);

		Token token;
		// Accept Comment after last item in the array.
		ok = readToken(token);
		while (token.type_ == tokenComment && ok) {
			ok = readToken(token);
		}
		bool badTokenType = (token.type_ != tokenArraySeparator
				&& token.type_ != tokenArrayEnd);
		if (!ok || badTokenType) {
			return addErrorAndRecover("Missing ',' or ']' in array declaration",
					token, tokenArrayEnd);
		}
		if (token.type_ == tokenArrayEnd)
			break;
	}
	return true;
}

bool Reader::decodeNumber(Token& token) {
	Value decoded;
	if (!decodeNumber(token, decoded)) {
		return false;
	}

	currentValue().swap(decoded);
	currentValue().setOffsetStart(token.start_ - begin_);
	currentValue().setOffsetLimit(token.end_ - begin_);
	return true;
}

bool Reader::decodeNumber(Token& token, Value& decoded) {
// Attempts to parse the number as an integer. If the number is
// larger than the maximum supported value of an integer then
// we decode the number as a double.
	Location current = token.start_;
	bool isNegative = *current == '-';
	if (isNegative) {
		++current;
	}

// TODO: Help the compiler do the div and mod at compile time or get rid of them.
	Value::LargestUInt maxIntegerValue = Value::maxLargestUInt;
	if (isNegative) {
		maxIntegerValue = Value::LargestUInt(-Value::minLargestInt);
	}

	Value::LargestUInt threshold = maxIntegerValue / 10;
	Value::LargestUInt value = 0;
	while (current < token.end_) {
		Char c = *current++;
		if (c < '0' || c > '9') {
			return decodeDouble(token, decoded);
		}
		Value::UInt digit(c - '0');
		if (value >= threshold) {
			if (value > threshold || current != token.end_
					|| digit > maxIntegerValue % 10) {
				return decodeDouble(token, decoded);
			}
		}
		value = value * 10 + digit;
	}
	if (isNegative) {
		decoded = -Value::LargestInt(value);
	} else if (value <= Value::LargestUInt(Value::maxInt)) {
		decoded = Value::LargestInt(value);
	} else {
		decoded = value;
	}
	return true;
}

bool Reader::decodeDouble(Token& token) {
	Value decoded;
	if (!decodeDouble(token, decoded)) {
		return false;
	}
	currentValue().swap(decoded);
	currentValue().setOffsetStart(token.start_ - begin_);
	currentValue().setOffsetLimit(token.end_ - begin_);
	return true;
}

bool Reader::decodeDouble(Token& token, Value& decoded) {
	double value = 0;
	const int bufferSize = 32;
	int count;
	int length = int(token.end_ - token.start_);

// Sanity check to avoid buffer overflow exploits.
	if (length < 0) {
		return addError("Unable to parse token length", token);
	}

// Avoid using a string constant for the format control string given to
// sscanf, as this can cause hard to debug crashes on OS X. See here for more
// info:
//
//     http://developer.apple.com/library/mac/#DOCUMENTATION/DeveloperTools/gcc-4.0.1/gcc/Incompatibilities.html
	char format[] = "%lf";

	if (length <= bufferSize) {
		Char buffer[bufferSize + 1];
		memcpy(buffer, token.start_, length);
		buffer[length] = 0;
		count = sscanf(buffer, format, &value);
	} else {
		std::string buffer(token.start_, token.end_);
		count = sscanf(buffer.c_str(), format, &value);
	}

	if (count != 1) {
		return addError(
				"'" + std::string(token.start_, token.end_)
						+ "' is not a number.", token);
	}
	decoded = value;
	return true;
}

bool Reader::decodeString(Token& token) {
	std::string decoded_string;
	if (!decodeString(token, decoded_string)) {
		return false;
	}

	Value decoded(decoded_string);
	currentValue().swap(decoded);
	currentValue().setOffsetStart(token.start_ - begin_);
	currentValue().setOffsetLimit(token.end_ - begin_);
	return true;
}

bool Reader::decodeString(Token& token, std::string& decoded) {
	decoded.reserve(token.end_ - token.start_ - 2);
	Location current = token.start_ + 1; // skip '"'
	Location end = token.end_ - 1;       // do not include '"'
	while (current != end) {
		Char c = *current++;
		if (c == '"') {
			break;
		} else if (c == '\\') {
			if (current == end) {
				return addError("Empty escape sequence in string", token,
						current);
			}
			Char escape = *current++;
			switch (escape) {
			case '"':
				decoded += '"';
				break;
			case '/':
				decoded += '/';
				break;
			case '\\':
				decoded += '\\';
				break;
			case 'b':
				decoded += '\b';
				break;
			case 'f':
				decoded += '\f';
				break;
			case 'n':
				decoded += '\n';
				break;
			case 'r':
				decoded += '\r';
				break;
			case 't':
				decoded += '\t';
				break;
			case 'u':
				unsigned int unicode;
				if (!decodeUnicodeCodePoint(token, current, end, unicode)) {
					return false;
				}
				decoded += codePointToUTF8(unicode);
				break;
			default:
				return addError("Bad escape sequence in string", token, current);
			}
		} else {
			decoded += c;
		}
	}
	return true;
}

bool Reader::decodeUnicodeCodePoint(Token& token, Location& current,
		Location end, unsigned int& unicode) {

	if (!decodeUnicodeEscapeSequence(token, current, end, unicode)) {
		return false;
	}
	if (unicode >= 0xD800 && unicode <= 0xDBFF) {
		// surrogate pairs
		if (end - current < 6) {
			return addError(
					"additional six characters expected to parse unicode surrogate pair.",
					token, current);
		}
		unsigned int surrogatePair;
		if (*(current++) == '\\' && *(current++) == 'u') {
			if (decodeUnicodeEscapeSequence(token, current, end,
					surrogatePair)) {
				unicode = 0x10000 + ((unicode & 0x3FF) << 10)
						+ (surrogatePair & 0x3FF);
			} else {
				return false;
			}
		} else {
			return addError(
					"expecting another \\u token to begin the second half of "
							"a unicode surrogate pair", token, current);
		}
	}
	return true;
}

bool Reader::decodeUnicodeEscapeSequence(Token& token, Location& current,
		Location end, unsigned int& unicode) {
	if (end - current < 4) {
		return addError(
				"Bad unicode escape sequence in string: four digits expected.",
				token, current);
	}
	unicode = 0;
	for (int index = 0; index < 4; ++index) {
		Char c = *current++;
		unicode *= 16;
		if (c >= '0' && c <= '9') {
			unicode += c - '0';
		} else if (c >= 'a' && c <= 'f') {
			unicode += c - 'a' + 10;
		} else if (c >= 'A' && c <= 'F') {
			unicode += c - 'A' + 10;
		} else {
			return addError(
					"Bad unicode escape sequence in string: hexadecimal digit expected.",
					token, current);
		}
	}
	return true;
}

bool Reader::addError(const std::string& message, Token& token,
		Location extra) {
	ErrorInfo info;
	info.token_ = token;
	info.message_ = message;
	info.extra_ = extra;
	errors_.push_back(info);
	return false;
}

bool Reader::recoverFromError(TokenType skipUntilToken) {
	int errorCount = int(errors_.size());
	Token skip;
	for (;;) {
		if (!readToken(skip)) {
			errors_.resize(errorCount); // discard errors caused by recovery
		}
		if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream) {
			break;
		}
	}
	errors_.resize(errorCount);
	return false;
}

bool Reader::addErrorAndRecover(const std::string& message, Token& token,
		TokenType skipUntilToken) {
	addError(message, token);
	return recoverFromError(skipUntilToken);
}

Value & Reader::currentValue() {
	return *(nodes_.top());
}

Reader::Char Reader::getNextChar() {
	if (current_ == end_) {
		return 0;
	}
	return *current_++;
}

void Reader::getLocationLineAndColumn(Location location, int& line,
		int& column) const {
	Location current = begin_;
	Location lastLineStart = current;
	line = 0;
	while (current < location && current != end_) {
		Char c = *current++;
		if (c == '\r') {
			if (*current == '\n') {
				++current;
			}
			lastLineStart = current;
			++line;
		} else if (c == '\n') {
			lastLineStart = current;
			++line;
		}
	}
// column & line start at 1
	column = int(location - lastLineStart) + 1;
	++line;
}

std::string Reader::getLocationLineAndColumn(Location location) const {
	int line, column;
	getLocationLineAndColumn(location, line, column);
	char buffer[18 + 16 + 16 + 1];
	snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
	return buffer;
}

std::string Reader::getFormattedErrorMessages() const {
	std::string formattedMessage;
	for (Errors::const_iterator itError = errors_.begin();
			itError != errors_.end(); ++itError) {
		const ErrorInfo& error = *itError;
		formattedMessage += "* " + getLocationLineAndColumn(error.token_.start_)
				+ "\n";
		formattedMessage += "  " + error.message_ + "\n";
		if (error.extra_) {
			formattedMessage += "See " + getLocationLineAndColumn(error.extra_)
					+ " for detail.\n";
		}
	}
	return formattedMessage;
}

std::vector<Reader::StructuredError> Reader::getStructuredErrors() const {
	std::vector<Reader::StructuredError> allErrors;
	for (Errors::const_iterator itError = errors_.begin();
			itError != errors_.end(); ++itError) {
		const ErrorInfo& error = *itError;
		Reader::StructuredError structured;
		structured.offset_start = error.token_.start_ - begin_;
		structured.offset_limit = error.token_.end_ - begin_;
		structured.message = error.message_;
		allErrors.push_back(structured);
	}
	return allErrors;
}

bool Reader::pushError(const Value& value, const std::string& message) {
	size_t length = end_ - begin_;
	if (value.getOffsetStart() > length || value.getOffsetLimit() > length) {
		return false;
	}
	Token token;
	token.type_ = tokenError;
	token.start_ = begin_ + value.getOffsetStart();
	token.end_ = end_ + value.getOffsetLimit();
	ErrorInfo info;
	info.token_ = token;
	info.message_ = message;
	info.extra_ = 0;
	errors_.push_back(info);
	return true;
}

bool Reader::pushError(const Value& value, const std::string& message,
		const Value& extra) {
	size_t length = end_ - begin_;
	if (value.getOffsetStart() > length || value.getOffsetLimit() > length
			|| extra.getOffsetLimit() > length) {
		return false;
	}

	Token token;
	token.type_ = tokenError;
	token.start_ = begin_ + value.getOffsetStart();
	token.end_ = begin_ + value.getOffsetLimit();
	ErrorInfo info;
	info.token_ = token;
	info.message_ = message;
	info.extra_ = begin_ + extra.getOffsetStart();
	errors_.push_back(info);
	return true;
}

bool Reader::good() const {
	return !errors_.size();
}

}
