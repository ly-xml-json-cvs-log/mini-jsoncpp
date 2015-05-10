/*
 * value.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef VALUE_H_INCLUDE_MINI_JSONCPP_
#define VALUE_H_INCLUDE_MINI_JSONCPP_

#include "config.h"

namespace Json {

/*
 *  brief Type of the value held by a Value object.
 */
enum ValueType {
	nullValue = 0, ///< 'null' value
	intValue,      ///< signed integer value
	uintValue,     ///< unsigned integer value
	realValue,     ///< double value
	stringValue,   ///< UTF-8 string value
	booleanValue,  ///< bool value
	arrayValue,    ///< array value (ordered list)
	objectValue    ///< object value (collection of name/value pairs).
};

/*
 * brief Represents a <a HREF="http://www.json.org">JSON</a> value.
 *
 * This class is a discriminated union wrapper that can represents a:
 * - signed integer [range: Value::minInt - Value::maxInt]
 * - unsigned integer (range: 0 - Value::maxUInt)
 * - double
 * - UTF-8 string
 * - boolean
 * - 'null'
 * - an ordered list of Value
 * - collection of name/value pairs (javascript object)
 *
 * The type of the held value is represented by a #ValueType and can be obtained using type().
 *
 * Values of an #objectValue or #arrayValue can be accessed using operator[]() methods.
 * Non-const methods will automatically create the a #nullValue element  if it does not exist.
 * The sequence of an #arrayValue will be automatically resized and initialized with #nullValue.
 *  resize() can be used to enlarge or truncate an #arrayValue.
 *
 * The get() methods can be used to obtain default value in the case the  required element does not exist.
 *
 * It is possible to iterate over the list of a #objectValue values using the getMemberNames() method.
 *
 * note #Value string-length fit in size_t, but keys must be < 2^30.
 * (The reason is an implementation detail.)
 * A #CharReader will raise an exception if a bound is exceeded to avoid security holes in your app,
 * but the Value API does *not* check bounds.
 * That is the responsibility of the caller.
 */
class Value {
	friend class ValueIteratorBase;
public:
	typedef std::vector<std::string> Members;
	typedef ValueIterator iterator;
	typedef ValueConstIterator const_iterator;
	typedef Json::UInt UInt;
	typedef Json::Int Int;
	typedef Json::UInt64 UInt64;
	typedef Json::Int64 Int64;
	typedef Json::LargestInt LargestInt;
	typedef Json::LargestUInt LargestUInt;
	typedef Json::ArrayIndex ArrayIndex;
	typedef std::map<string, Value> ObjectValues;

public:
	///< We regret this reference to a global instance; prefer the simpler Value().
	static const Value& null;

	///< just a kludge for binary-compatibility; same as null
	static const Value& nullRef;

	/// Minimum signed integer value that can be stored in a Json::Value.
	static const LargestInt minLargestInt;
	/// Maximum signed integer value that can be stored in a Json::Value.
	static const LargestInt maxLargestInt;
	/// Maximum unsigned integer value that can be stored in a Json::Value.
	static const LargestUInt maxLargestUInt;

	/// Minimum signed int value that can be stored in a Json::Value.
	static const Int minInt;
	/// Maximum signed int value that can be stored in a Json::Value.
	static const Int maxInt;
	/// Maximum unsigned int value that can be stored in a Json::Value.
	static const UInt maxUInt;

	/// Minimum signed 64 bits int value that can be stored in a Json::Value.
	static const Int64 minInt64;
	/// Maximum signed 64 bits int value that can be stored in a Json::Value.
	static const Int64 maxInt64;
	/// Maximum unsigned 64 bits int value that can be stored in a Json::Value.
	static const UInt64 maxUInt64;

public:
	/*
	 * brief Create a default Value of the given type.
	 *  This is a very useful constructor.
	 *  To create an empty array, pass arrayValue.
	 *   To create an empty object, pass objectValue.
	 *    Another Value can then be set to this one by assignment.
	 *    This is useful since clear() and resize() will not alter types.
	 *
	 *     Examples:
	 *      Json::Value null_value; // null
	 *       Json::Value arr_value(Json::arrayValue); // []
	 *        Json::Value obj_value(Json::objectValue); // {}
	 */
	Value(ValueType type = nullValue);
	Value(Int value);
	Value(UInt value);
	Value(Int64 value);
	Value(UInt64 value);
	Value(double value);
	Value(const char* value);
	Value(const std::string& value);
	Value(bool value);
	Value(const Value& other);
	~Value();

	/*
	 *  Deep copy, then swap(other).
	 */
	Value& operator=(Value other);

	/*
	 * Swap everything.
	 */
	void swap(Value& other);
	/// Swap values but leave comments and source offsets in place.
	void swapPayload(Value& other);

	ValueType type() const;

	/// Compare payload only, not comments etc.
	bool operator<(const Value& other) const;
	bool operator<=(const Value& other) const;
	bool operator>=(const Value& other) const;
	bool operator>(const Value& other) const;
	bool operator==(const Value& other) const;
	bool operator!=(const Value& other) const;
	int compare(const Value& other) const;

	/*
	 *  Embedded zeroes could cause you trouble!
	 */
	const char* asCString() const;

	/*
	 * Embedded zeroes are possible.
	 */

	std::string asString() const;
	Int asInt() const;
	UInt asUInt() const;
	Int64 asInt64() const;
	UInt64 asUInt64() const;
	LargestInt asLargestInt() const;
	LargestUInt asLargestUInt() const;
	float asFloat() const;
	double asDouble() const;
	bool asBool() const;

	bool isNull() const;
	bool isBool() const;
	bool isInt() const;
	bool isInt64() const;
	bool isUInt() const;
	bool isUInt64() const;
	bool isIntegral() const;
	bool isDouble() const;
	bool isNumeric() const;
	bool isString() const;
	bool isArray() const;
	bool isObject() const;

	bool isConvertibleTo(ValueType other) const;

	/*
	 * Number of values in array or object
	 */
	ArrayIndex size() const;

	/*
	 * brief Return true if empty array, empty object, or null; otherwise, false.
	 */
	bool empty() const;

	/*
	 * Return isNull()
	 */
	bool operator!() const;

	/*
	 * Remove all object members and array elements.
	 * pre type() is arrayValue, objectValue, or nullValue post type() is unchanged
	 */
	void clear();

	/*
	 * Resize the array to size elements.
	 * New elements are initialized to null.
	 * May only be called on nullValue or arrayValue.
	 * pre type() is arrayValue or nullValue post type() is arrayValue
	 */
	void resize(ArrayIndex size);

	/// Access an array element (zero based index ).
	/// If the array contains less than index element, then null value are inserted
	/// in the array so that its size is index+1.
	Value& operator[](ArrayIndex index);
	Value& operator[](int index);
	const Value& operator[](ArrayIndex index) const;
	const Value& operator[](int index) const;

	/// If the array contains at least index+1 elements, returns the element
	/// value,
	/// otherwise returns defaultValue.
	Value get(ArrayIndex index, const Value& defaultValue) const;
	/// Return true if index < size().
	bool isValidIndex(ArrayIndex index) const;

	/// \brief Append value to array at the end.
	/// Equivalent to jsonvalue[jsonvalue.size()] = value;
	Value& append(const Value& value);

	/// Access an object value by name, create a null member if it does not exist.
	/// \note Because of our implementation, keys are limited to 2^30 -1 chars.
	///  Exceeding that will cause an exception.
	Value& operator[](const char* key);
	const Value& operator[](const char* key) const;
	Value& operator[](const std::string& key);
	const Value& operator[](const std::string& key) const;

	/// Return the member named key if it exist, defaultValue otherwise.
	/// \note deep copy
	Value get(const char* key, const Value& defaultValue) const;
	Value get(const std::string& key, const Value& defaultValue) const;

	/// \brief Remove and return the named member.
	///
	/// Do nothing if it did not exist.
	/// \return the removed Value, or null.
	Value removeMember(const char* key);
	Value removeMember(const std::string& key);

	/*
	 * brief Remove the named map member.
	 */
	bool removeMember(std::string const& key, Value* removed);
	bool removeMember(const char* key, Value* removed);

	/*
	 * \brief Remove the indexed array element.
	 */
	bool removeIndex(ArrayIndex i, Value* removed);

	/// Return true if the object has a member named key.
	bool isMember(const char* key) const;
	bool isMember(const std::string& key) const;

	/// \brief Return a list of the member names.
	/// If null, return an empty list.
	Members getMemberNames() const;

	std::string toStyledString() const;

	const_iterator begin() const;
	const_iterator end() const;

	iterator begin();
	iterator end();

private:
	void initBasic(ValueType type, bool allocated = false);

	Value& resolveReference(const char* key);
	Value& resolveReference(const char* key, const char* end);

	union ValueHolder {
		LargestInt int_;
		LargestUInt uint_;
		double real_;
		bool bool_;
		char* string_; // actually ptr to unsigned, followed by str, unless !allocated_
		ObjectValues* map_;
	} value_;
	ValueType type_ :8;

	/*
	 *Notes: if declared as bool, bitfield is useless.
	 *If not allocated_, string_ must be null-terminated.
	 */
	unsigned int allocated_ :1;
};

/** \brief Experimental and untested: represents an element of the "path" to
 * access a node.
 */
class PathArgument {
public:
	friend class Path;

	PathArgument();
	PathArgument(ArrayIndex index);
	PathArgument(const char* key);
	PathArgument(const std::string& key);

private:
	enum Kind {
		kindNone = 0, kindIndex, kindKey
	};
	std::string key_;
	ArrayIndex index_;
	Kind kind_;
};

/** \brief Experimental and untested: represents a "path" to access a node.
 *
 * Syntax:
 * - "." => root node
 * - ".[n]" => elements at index 'n' of root node (an array value)
 * - ".name" => member named 'name' of root node (an object value)
 * - ".name1.name2.name3"
 * - ".[0][1][2].name1[3]"
 * - ".%" => member name is provided as parameter
 * - ".[%]" => index is provied as parameter
 */
class Path {
public:
	Path(const std::string& path, const PathArgument& a1 = PathArgument(),
			const PathArgument& a2 = PathArgument(), const PathArgument& a3 =
					PathArgument(), const PathArgument& a4 = PathArgument(),
			const PathArgument& a5 = PathArgument());

			const Value& resolve(const Value& root) const;
	Value resolve(const Value& root, const Value& defaultValue) const;
	/// Creates the "path" to access the specified node and returns a reference on
	/// the node.
	Value& make(Value& root) const;

private:
	typedef std::vector<const PathArgument*> InArgs;
	typedef std::vector<PathArgument> Args;

	void makePath(const std::string& path, const InArgs& in);
	void addPathInArg(const std::string& path, const InArgs& in,
			InArgs::const_iterator& itInArg, PathArgument::Kind kind);
	void invalidPath(const std::string& path, int location);

	Args args_;
};

/** \brief base class for Value iterators.
 *
 */
class ValueIteratorBase {
public:
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef unsigned int size_t;
	typedef int difference_type;
	typedef ValueIteratorBase SelfType;

	bool operator==(const SelfType& other) const {
		return isEqual(other);
	}

	bool operator!=(const SelfType& other) const {
		return !isEqual(other);
	}

	difference_type operator-(const SelfType& other) const {
		return other.computeDistance(*this);
	}

	/// Return either the index or the member name of the referenced value as a
	/// Value.
	Value key() const;

	/// Return the index of the referenced Value, or -1 if it is not an arrayValue.
	UInt index() const;

	/// Return the member name of the referenced Value, or "" if it is not an
	/// objectValue.
	/// \note Avoid `c_str()` on result, as embedded zeroes are possible.
	std::string name() const;

	/// Return the member name of the referenced Value. "" if it is not an
	/// objectValue.
	/// \deprecated This cannot be used for UTF-8 strings, since there can be embedded nulls.
	JSONCPP_DEPRECATED("Use `key = name();` instead.")
	char const* memberName() const;
/// Return the member name of the referenced Value, or NULL if it is not an
/// objectValue.
/// \note Better version than memberName(). Allows embedded nulls.
	char const* memberName(char const** end) const;

protected:
	Value& deref() const;

	void increment();

	void decrement();

	difference_type computeDistance(const SelfType& other) const;

	bool isEqual(const SelfType& other) const;

	void copy(const SelfType& other);

private:
	Value::ObjectValues::iterator current_;
// Indicates that iterator is for a null value.
	bool isNull_;

public:
// For some reason, BORLAND needs these at the end, rather
// than earlier. No idea why.
	ValueIteratorBase();
	explicit ValueIteratorBase(const Value::ObjectValues::iterator& current);
};

/** \brief const iterator for object and array value.
 *
 */
class ValueConstIterator: public ValueIteratorBase {
	friend class Value;

public:
	typedef const Value value_type;
	typedef unsigned int size_t;
	typedef int difference_type;
	typedef const Value& reference;
	typedef const Value* pointer;
	typedef ValueConstIterator SelfType;

	ValueConstIterator();

private:
	/*! \internal Use by Value to create an iterator.
	 */
	explicit ValueConstIterator(const Value::ObjectValues::iterator& current);
public:
	SelfType& operator=(const ValueIteratorBase& other);

	SelfType operator++(int) {
		SelfType temp(*this);
		++*this;
		return temp;
	}

	SelfType operator--(int) {
		SelfType temp(*this);
		--*this;
		return temp;
	}

	SelfType& operator--() {
		decrement();
		return *this;
	}

	SelfType& operator++() {
		increment();
		return *this;
	}

	reference operator*() const {
		return deref();
	}

	pointer operator->() const {
		return &deref();
	}
};

/** \brief Iterator for object and array value.
 */
class ValueIterator: public ValueIteratorBase {
	friend class Value;

public:
	typedef Value value_type;
	typedef unsigned int size_t;
	typedef int difference_type;
	typedef Value& reference;
	typedef Value* pointer;
	typedef ValueIterator SelfType;

	ValueIterator();
	ValueIterator(const ValueConstIterator& other);
	ValueIterator(const ValueIterator& other);

private:
	/*! \internal Use by Value to create an iterator.
	 */
	explicit ValueIterator(const Value::ObjectValues::iterator& current);
public:
	SelfType& operator=(const SelfType& other);

	SelfType operator++(int) {
		SelfType temp(*this);
		++*this;
		return temp;
	}

	SelfType operator--(int) {
		SelfType temp(*this);
		--*this;
		return temp;
	}

	SelfType& operator--() {
		decrement();
		return *this;
	}

	SelfType& operator++() {
		increment();
		return *this;
	}

	reference operator*() const {
		return deref();
	}

	pointer operator->() const {
		return &deref();
	}
};

}

namespace std {
/// Specialize std::swap() for Json::Value.
template<>
inline void swap(Json::Value& a, Json::Value& b) {
	a.swap(b);
}
}

#endif /* VALUE_H_INCLUDE_MINI_JSONCPP_ */
