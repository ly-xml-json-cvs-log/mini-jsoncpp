/*
 * value.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef VALUE_H_INCLUDE_MINI_JSONCPP_
#define VALUE_H_INCLUDE_MINI_JSONCPP_

#include "config.h"
#include "tools.h"

namespace Json {

/*
 *  brief Type of the value held by a Value object.
 */
enum ValueType {
	nullValue = 0, ///< 'null' value
	intValue,      ///< signed integer value
	uintValue,     ///< unsigned integer value
	//int64Value,      ///< signed integer64 value
	//uint64Value,     ///< unsigned integer64 value
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
public:
	typedef std::vector<std::string> Members;
	typedef Json::UInt UInt;
	typedef Json::Int Int;
	typedef Json::UInt64 UInt64;
	typedef Json::Int64 Int64;
	typedef Json::LargestInt LargestInt;
	typedef Json::LargestUInt LargestUInt;
	typedef Json::ArrayIndex ArrayIndex;
	typedef std::map<string, Value> ObjectValues;
	typedef std::vector<Value> ArrayValues;
	typedef std::string StringValues;

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
	 *  To create an empty object, pass objectValue.
	 *  Another Value can then be set to this one by assignment.
	 *  This is useful since clear() and resize() will not alter types.
	 *
	 *  Examples:
	 *    Json::Value null_value; // null
	 *    Json::Value arr_value(Json::arrayValue); // []
	 *    Json::Value obj_value(Json::objectValue); // {}
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

	void destructor();
	~Value();

	/*
	 *  Deep copy, then swap(other).
	 */
	Value& operator=(const Value& other);

	/*
	 * Swap everything.
	 */
	void swap(Value& other);

	ValueType type() const;

	/// Compare payload only, not comments etc.
	bool operator<(const Value& other) const;
	bool operator<=(const Value& other) const;
	bool operator>=(const Value& other) const;
	bool operator>(const Value& other) const;
	bool operator==(const Value& other) const;
	bool operator!=(const Value& other) const;

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

	/// Most general and efficient version of isMember()const, get()const,
	/// and operator[]const
	/// \note As stated elsewhere, behavior is undefined if (end-key) >= 2^30
	Value * find(const std::string& key);
	const Value * find(const std::string& key) const;
	Value * find(const char* key);
	const Value * find(const char* key) const;

	/// \brief Append value to array at the end.
	/// Equivalent to jsonvalue[jsonvalue.size()] = value;
	void append(const Value& value);

	/// Access an object value by name, create a null member if it does not exist.
	/// \note Because of our implementation, keys are limited to 2^30 -1 chars.
	///  Exceeding that will cause an exception.
	Value& operator[](const string& key);
	Value& operator[](const char* key);
	const Value& operator[](const string& key) const;
	const Value& operator[](const char* key) const;

	/// \brief Remove and return the named member.
	///
	/// Do nothing if it did not exist.
	/// \return the removed Value, or null.
	void removeMember(const std::string& key);

	/*
	 * \brief Remove the indexed array element.
	 */
	void removeIndex(ArrayIndex i);

	/// Return true if the object has a member named key.
	bool isMember(const std::string& key) const;

	/// \brief Return a list of the member names.
	/// If null, return an empty list.
	Members getMemberNames() const;

	std::string toStyledString() const;
private:
	void initBasic(ValueType type);

	StringValues*getStringVaule();
	ObjectValues* getObjectVaule();
	ArrayValues* getArrayVaule();

	int compare(const Value& other) const;
	Value& assignment(Value const& other);
	void transformType(ValueType newType);

	union ValueHolder {
		LargestInt int_;
		LargestUInt uint_;
		double real_;
		bool bool_;
		StringValues* string_;
		ObjectValues* map_;
		ArrayValues* array_;
	} value_;
	ValueType type_;

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
