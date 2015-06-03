/*
 * reader.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef READER_H_INCLUDE_MINI_JSONCPP_
#define READER_H_INCLUDE_MINI_JSONCPP_

#include "value.h"

namespace Json {

/** \brief Unserialize a <a HREF="http://www.json.org">JSON</a> document into a
 *Value.
 *
 * \deprecated Use CharReader and CharReaderBuilder.
 */
class Reader {
public:
	typedef char Char;
	typedef const Char* Location;

	/** \brief Constructs a Reader allowing all features
	 * for parsing.
	 */
	Reader(int debug = 0);

	/** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a>
	 * document.
	 * \param document UTF-8 encoded string containing the document to read.
	 * \param root [out] Contains the root value of the document if it was
	 *             successfully parsed.
	 * \param collectComments \c true to collect comment and allow writing them
	 * back during
	 *                        serialization, \c false to discard comments.
	 *                        This parameter is ignored if
	 * Features::allowComments_
	 *                        is \c false.
	 * \return \c true if the document was successfully parsed, \c false if an
	 * error occurred.
	 */
	bool
	parse(const std::string& document, Value& root);

	/** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a>
	 document.
	 * \param beginDoc Pointer on the beginning of the UTF-8 encoded string of the
	 document to read.
	 * \param endDoc Pointer on the end of the UTF-8 encoded string of the
	 document to read.
	 *               Must be >= beginDoc.
	 * \param root [out] Contains the root value of the document if it was
	 *             successfully parsed.
	 * \param collectComments \c true to collect comment and allow writing them
	 back during
	 *                        serialization, \c false to discard comments.
	 *                        This parameter is ignored if
	 Features::allowComments_
	 *                        is \c false.
	 * \return \c true if the document was successfully parsed, \c false if an
	 error occurred.
	 */
	bool parse(const char* beginDoc, const char* endDoc, Value& root);

	/** \brief Returns a user friendly string that list errors in the parsed
	 * document.
	 * \return Formatted error message with the list of errors with their location
	 * in
	 *         the parsed document. An empty string is returned if no error
	 * occurred
	 *         during parsing.
	 */
	std::string getFormattedErrorMessages() const;

	std::vector<string> getStructuredErrors() const;

	/** \brief Return whether there are any errors.
	 * \return \c true if there are no errors to report \c false if
	 * errors have occurred.
	 */
	bool good() const;

private:
	enum TokenType {
		tokenEndOfStream = 0,
		tokenObjectBegin,
		tokenObjectEnd,
		tokenArrayBegin,
		tokenArrayEnd,
		tokenString,
		tokenNumber,
		tokenTrue,
		tokenFalse,
		tokenNull,
		tokenArraySeparator,
		tokenMemberSeparator,
		tokenComment,
		tokenError
	};

	class Token {
	public:
		TokenType type_;
		Location start_;
		Location end_;
		Token() :
				type_(tokenEndOfStream), start_(NULL), end_(NULL) {
		}
	};

	class ErrorInfo {
	public:
		Token token_;
		std::string message_;
	};

	typedef std::deque<ErrorInfo> Errors;

	bool readToken(Token& token);
	void skipSpaces();
	bool match(Location pattern, int patternLength);
	bool readString();
	void readNumber();
	bool readValue();
	bool readObject();
	bool readArray();
	bool decodeNumber(Token& token);
	bool decodeNumber(Token& token, Value& decoded);
	bool decodeString(Token& token);
	bool decodeString(Token& token, std::string& decoded);
	bool decodeDouble(Token& token);
	bool decodeDouble(Token& token, Value& decoded);
	bool decodeUnicodeCodePoint(Token& token, Location& current, Location end,
			unsigned int& unicode);
	bool decodeUnicodeEscapeSequence(Token& token, Location& current,
			Location end, unsigned int& unicode);
	bool addError(const std::string& message, Token& token);
	bool recoverFromError(TokenType skipUntilToken);
	bool addErrorAndRecover(const std::string& message, Token& token,
			TokenType skipUntilToken);
	void skipUntilSpace();
	Value& currentValue();
	Char getNextChar();
	void
	getLocationLineAndColumn(Location location, int& line, int& column) const;
	std::string getLocationLineAndColumn(Location location) const;

	typedef std::stack<Value*> Nodes;
	Nodes nodes_;
	Errors errors_;
	std::string document_;
	Location begin_;
	Location end_;
	Location current_;
	Location lastValueEnd_;
	Value* lastValue_;
	int debug;
};
// Reader

}// namespace Json

#endif /* READER_H_INCLUDE_MINI_JSONCPP_ */
